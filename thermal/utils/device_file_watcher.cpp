/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <chrono>
#include <dirent.h>
#include <fstream>
#include <sys/inotify.h>
#include <sys/resource.h>
#include <sys/types.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include "device_file_watcher.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

using std::chrono_literals::operator""ms;

// iNotify constants.
constexpr int kiNotifyEventSize = sizeof(struct inotify_event);

void DeviceFileWatcher::registerFilesToWatch(
        const std::vector<std::string> files_to_watch) {
    inotify_fd_ = unique_fd(inotify_init());
    if (inotify_fd_ < 0) {
        LOG(ERROR) << "iNotify init error.";
        return;
    }

    for (const auto& path : files_to_watch) {
        std::lock_guard<std::mutex> _lock(watcher_mutex_);
        watch_to_file_path_map_.emplace(
            inotify_add_watch(inotify_fd_, path.c_str(), IN_MODIFY),
            path);
    }
}

bool DeviceFileWatcher::startWatchingDeviceFiles() {
    if (initializedOk()) {
        // Start the watcher thread.
        std::packaged_task<void(void)> watcher_task(
            std::bind(&DeviceFileWatcher::watchFilesForModificationsAndCallback,
                      this));
        watcher_future_ = watcher_task.get_future();
        watcher_thread_ = std::thread(std::move(watcher_task));
        watcher_thread_.detach();
        return true;
    }
    return false;
}

void DeviceFileWatcher::registerCallback(
        std::function<void(const std::pair<std::string, std::string>&)> cb) {
    std::lock_guard<std::mutex> _lock(watcher_mutex_);
    cb_ = cb;
}

void DeviceFileWatcher::registerQueueOverflowCallback(
        std::function<void(void)> cb) {
    std::lock_guard<std::mutex> _lock(watcher_mutex_);
    queue_overflow_cb_ = cb;
}

void DeviceFileWatcher::watchFilesForModificationsAndCallback() {
    // Increase the priority of the watcher thread so we minimize losing
    // data.
    if (int errval = setpriority(PRIO_PROCESS, 0, -5)) {
        LOG(ERROR) << "Failed to setpriority the watcher thread: " << errval;
    }

    // We use this while(1) because inotify read() blocks, thus stopping the
    // watcher thread from just spinning.
    while (initializedOk()) {
        char buffer[kiNotifyEventSize];
        TEMP_FAILURE_RETRY(read(inotify_fd_, buffer, sizeof(buffer)));
        struct inotify_event* event;
        event = reinterpret_cast<struct inotify_event*>(buffer);

        // Catch the case where we overflow the queue. In this case we probably
        // dropped some modifications so we re-read everything and re-notify.
        if (event->mask & IN_Q_OVERFLOW) {
            LOG(ERROR) << "iNotify queue overflowed! Modification events "
                       << "dropped.";
            {
                std::lock_guard<std::mutex> _lock(watcher_mutex_);
                if (queue_overflow_cb_) {
                    queue_overflow_cb_();
                }
            }
        }

        if (event->mask & IN_MODIFY) {
            std::lock_guard<std::mutex> _lock(watcher_mutex_);
            std::string path = watch_to_file_path_map_.at(event->wd);

            std::string data;
            android::base::ReadFileToString(path, &data);
            data = android::base::Trim(data);

            if (!data.empty()) {
                if (cb_) {
                    cb_(std::make_pair(path, data));
                } else {
                    LOG(ERROR) << "Consumer callback is null!";
                }
            }
        }

        // Rate limit our thread to be able to service changes at around a
        // second. We don't want to make the system do more work if things fall
        // off a cliff. We are watching eight files, one per CPU, so 125
        // milliseconds should be such that if all of them had a modification at
        // the same time it would take the thread a second to read all of them.
        std::this_thread::sleep_for(125ms);
    }

    // We should really not get here until DeviceFileWatcher is destroyed.
    LOG(ERROR) << "Watcher thread has stopped.";
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android
