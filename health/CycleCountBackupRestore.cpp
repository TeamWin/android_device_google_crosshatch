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

#include "CycleCountBackupRestore.h"

namespace device {
namespace google {
namespace crosshatch {
namespace health {

static constexpr int kBackupTrigger = 20;

CycleCountBackupRestore::CycleCountBackupRestore(int nb_buckets, const char *sysfs_path,
                                                 const char *persist_path, const char *serial_path)
    : nb_buckets_(nb_buckets),
      saved_soc_(-1),
      soc_inc_(0),
      sysfs_path_(sysfs_path),
      persist_path_(persist_path),
      serial_path_(serial_path) {
    sw_bins_ = new int[nb_buckets]();
    hw_bins_ = new int[nb_buckets]();
}

void CycleCountBackupRestore::Restore() {
    if (CheckSerial()) {
        Read(persist_path_, sw_bins_);
    }
    Read(sysfs_path_, hw_bins_);
    UpdateAndSave();
}

bool CycleCountBackupRestore::CheckSerial() {
    std::string device_battery_serial;
    std::string persist_battery_serial;

    if (serial_path_.empty())
        return true;

    if (!android::base::ReadFileToString(serial_path_, &device_battery_serial)) {
        LOG(ERROR) << "Failed to read " << serial_path_;
        return true;
    }

    if (!android::base::ReadFileToString(kPersistSerial, &persist_battery_serial)) {
        LOG(ERROR) << "Failed to read " << kPersistSerial;
    }

    if (device_battery_serial != persist_battery_serial) {
        // Battery pack has been changed or first time,
        // cycle counts on the pack are the ones to save
        if (!android::base::WriteStringToFile(device_battery_serial, kPersistSerial)) {
            LOG(ERROR) << "Write to " << kPersistSerial << " error: " << strerror(errno);
        }
        return false;
    }

    return true;
}

void CycleCountBackupRestore::Backup(int battery_level) {
    if (saved_soc_ == -1) {
        saved_soc_ = battery_level;
        return;
    }
    // Cycle counts only increases on increasing level
    if (battery_level > saved_soc_) {
        soc_inc_ += battery_level - saved_soc_;
    }
    saved_soc_ = battery_level;
    // To avoid writting file too often just rate limit it
    if (soc_inc_ >= kBackupTrigger) {
        Read(sysfs_path_, hw_bins_);
        UpdateAndSave();
        soc_inc_ = 0;
    }
}

void CycleCountBackupRestore::Read(const std::string &path, int *bins) {
    std::string buffer;

    if (!android::base::ReadFileToString(path, &buffer)) {
        LOG(ERROR) << "Failed to read " << path;
        return;
    }

    buffer = ::android::base::Trim(buffer);
    std::vector<std::string> counts = android::base::Split(buffer, " ");
    if (counts.size() != (size_t)nb_buckets_) {
        LOG(ERROR) << "data format \"" << buffer << "\" is wrong in " << path;
    } else {
        LOG(INFO) << "Read: \"" << buffer << "\" from " << path;
        for (int i = 0; i < nb_buckets_; ++i) {
            bins[i] = std::stoi(counts[i]);
        }
    }
}

void CycleCountBackupRestore::Write(int *bins, const std::string &path) {
    std::string str_data = "";

    for (int i = 0; i < nb_buckets_; ++i) {
        if (i) {
            str_data += " ";
        }
        str_data += std::to_string(bins[i]);
    }

    LOG(INFO) << "Write: \"" << str_data << "\" to " << path;
    if (!android::base::WriteStringToFile(str_data, path))
        LOG(ERROR) << "Write to " << path << " error: " << strerror(errno);
}

void CycleCountBackupRestore::UpdateAndSave() {
    bool backup = false;
    bool restore = false;
    for (int i = 0; i < nb_buckets_; i++) {
        if (hw_bins_[i] < sw_bins_[i]) {
            hw_bins_[i] = sw_bins_[i];
            restore = true;
        } else if (hw_bins_[i] > sw_bins_[i]) {
            sw_bins_[i] = hw_bins_[i];
            backup = true;
        }
    }
    if (restore)
        Write(hw_bins_, sysfs_path_);
    if (backup)
        Write(sw_bins_, persist_path_);
}

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device
