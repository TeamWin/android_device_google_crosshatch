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

#ifndef DEVICE_GOOGLE_CROSSHATCH_HEALTH_CYCLECOUNTBACKUPRESTORE_H
#define DEVICE_GOOGLE_CROSSHATCH_HEALTH_CYCLECOUNTBACKUPRESTORE_H

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <string>

namespace device {
namespace google {
namespace crosshatch {
namespace health {

class CycleCountBackupRestore {
  public:
    CycleCountBackupRestore(int nb_buckets, const char *sysfs_path, const char *persist_path,
                            const char *serial_path = "");
    void Restore();
    void Backup(int battery_level);

  private:
    const char *kPersistSerial = "/persist/battery/serial_number";

    int nb_buckets_;
    int *sw_bins_;
    int *hw_bins_;
    int saved_soc_;
    int soc_inc_;
    std::string sysfs_path_;
    std::string persist_path_;
    std::string serial_path_;

    void Read(const std::string &path, int *bins);
    void Write(int *bins, const std::string &path);
    void UpdateAndSave();
    bool CheckSerial();
};

}  // namespace health
}  // namespace crosshatch
}  // namespace google
}  // namespace device

#endif  // #ifndef DEVICE_GOOGLE_CROSSHATCH_HEALTH_CYCLECOUNTBACKUPRESTORE_H
