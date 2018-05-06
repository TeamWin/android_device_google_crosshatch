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
#ifndef __DEVICE_FILE_WATCHER_H__
#define __DEVICE_FILE_WATCHER_H__

#include <chrono>
#include <condition_variable>
#include <future>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <android-base/unique_fd.h>

#include "cooling_devices.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_1 {
namespace implementation {

using android::base::unique_fd;
using std::chrono_literals::operator""ms;
using WatcherCallback =
    std::function<void(const std::pair<std::string, std::string>&)>;

/* A helper class to watch modifications to files. */
class DeviceFileWatcher {
    public:
     DeviceFileWatcher() = default;
     ~DeviceFileWatcher() = default;

     // Disallow copy and assign.
     DeviceFileWatcher(const DeviceFileWatcher&) = delete;
     void operator=(const DeviceFileWatcher&) = delete;

     // Start the thread and return true if it succeeds. This function will
     // check that there are files registered to watch and that there is a
     // callback registered. If either conditions are not met this function will
     // return false.
     bool startWatchingDeviceFiles();
     // Give the file watcher a list of files to start watching. This helper
     // class will by default wait for modifications to the file.
     void registerFilesToWatch(const std::vector<std::string> files_to_watch);
     // Give the file watcher a callback to be called when processing the data
     // read from the watched changes list.
     void registerCallback(std::function<void(
         const std::pair<std::string, std::string>&)> cb);
     // Register a function called back if we see a queue overflow event from
     // inotify.
     void registerQueueOverflowCallback(std::function<void(void)> cb);
     // This class is properly initialized if there is a callback that's
     // registered and if we have files to watch.
     bool initializedOk() const {
       return watch_to_file_path_map_.size() && cb_;
     }

     // Get the status of the threads.  watcher_future_ can be invalid here, if
     // we call getWatcherThreadStatus() without first calling
     // startWatchingDeviceFiles(). In the case that we haven't spawned any
     // threads, we'll just return std::future_status::ready which indicates
     // that the thread is no longer running.
     std::future_status getWatcherThreadStatus() const {
        return watcher_future_.valid() ?
            watcher_future_.wait_for(0ms) : std::future_status::ready;
     }

    private:
     // The work done by the watcher thread. This will use inotify to check for
     // modifications to the files to watch. If any modification is seen this
     // will callback the registered function with the new data read from the
     // modified file.
     void watchFilesForModificationsAndCallback();

     unique_fd inotify_fd_;
     // Maps iNotify watch descriptor to cooling device state file path.
     std::unordered_map<int, std::string> watch_to_file_path_map_;

     // This thread watches the files for modifications and fills out the values
     // buffer.
     std::thread watcher_thread_;
     std::future<void> watcher_future_;

     // The callback function. Called whenever a modification is seen.  The
     // function passed in should expect a pair of strings in the form
     // (path, data). Where path is the path of the file that saw a modification
     // and data was the modification.
     WatcherCallback cb_;

     // A callback function to use if we ever see the queue overflow. This is
     // not required to start watching files. However, if no callback is
     // registered and we see the queue overflow we will just log that
     // modification events were dropped and do nothing.
     std::function<void(void)> queue_overflow_cb_;

     // Variables shared between the watcher and consumer threads.
     std::mutex watcher_mutex_;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // __DEVICE_FILE_WATCHER_H__
