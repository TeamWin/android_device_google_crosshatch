/*
 * Copyright 2016 The Android Open Source Project
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

#define LOG_TAG "dumpstate"

#include "DumpstateDevice.h"

#include <android-base/properties.h>
#include <android-base/unique_fd.h>
#include <cutils/properties.h>
#include <hidl/HidlBinderSupport.h>
#include <hidl/HidlSupport.h>

#include <log/log.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>

#define _SVID_SOURCE
#include <dirent.h>

#include "DumpstateUtil.h"

#define MODEM_LOG_PREFIX_PROPERTY "ro.radio.log_prefix"
#define MODEM_LOG_LOC_PROPERTY "ro.radio.log_loc"
#define MODEM_LOGGING_SWITCH "persist.radio.smlog_switch"

#define DIAG_MDLOG_PERSIST_PROPERTY "persist.vendor.sys.modem.diag.mdlog"
#define DIAG_MDLOG_PROPERTY "vendor.sys.modem.diag.mdlog"
#define DIAG_MDLOG_STATUS_PROPERTY "vendor.sys.modem.diag.mdlog_on"

#define DIAG_MDLOG_NUMBER_BUGREPORT "persist.vendor.sys.modem.diag.mdlog_br_num"

#define UFS_BOOTDEVICE "ro.boot.bootdevice"

#define TCPDUMP_NUMBER_BUGREPORT "persist.vendor.tcpdump.log.br_num"
#define TCPDUMP_PERSIST_PROPERTY "persist.vendor.tcpdump.log.alwayson"

#define MODEM_EFS_DUMP_PROPERTY "vendor.sys.modem.diag.efsdump"

#define VENDOR_VERBOSE_LOGGING_ENABLED_PROPERTY "persist.vendor.verbose_logging_enabled"

using android::os::dumpstate::CommandOptions;
using android::os::dumpstate::DumpFileToFd;
using android::os::dumpstate::PropertiesHelper;
using android::os::dumpstate::RunCommandToFd;

namespace android {
namespace hardware {
namespace dumpstate {
namespace V1_1 {
namespace implementation {

#define DIAG_LOG_PREFIX "diag_log_"
#define TCPDUMP_LOG_PREFIX "tcpdump"
#define EXTENDED_LOG_PREFIX "extended_log_"

#define BUFSIZE 65536
static void copyFile(std::string srcFile, std::string destFile) {
    uint8_t buffer[BUFSIZE];
    ssize_t size;

    int fdSrc = open(srcFile.c_str(), O_RDONLY);
    if (fdSrc < 0) {
        ALOGD("Failed to open source file %s\n", srcFile.c_str());
        return;
    }

    int fdDest = open(destFile.c_str(), O_WRONLY | O_CREAT, 0666);
    if (fdDest < 0) {
        ALOGD("Failed to open destination file %s\n", destFile.c_str());
        close(fdSrc);
        return;
    }

    ALOGD("Copying %s to %s\n", srcFile.c_str(), destFile.c_str());
    while ((size = TEMP_FAILURE_RETRY(read(fdSrc, buffer, BUFSIZE))) > 0) {
        TEMP_FAILURE_RETRY(write(fdDest, buffer, size));
    }

    close(fdDest);
    close(fdSrc);
}

struct PosixTarHeader {
    char name[100];               /*   0 */
    char mode[8];                 /* 100 */
    char uid[8];                  /* 108 */
    char gid[8];                  /* 116 */
    char size[12];                /* 124 */
    char mtime[12];               /* 136 */
    char chksum[8];               /* 148 */
    char typeflag;                /* 156 */
    char linkname[100];           /* 157 */
    char magic[6];                /* 257 */
    char version[2];              /* 259 */
    char uname[32];               /* 265 */
    char gname[32];               /* 297 */
    char devmajor[8];             /* 329 */
    char devminor[8];             /* 337 */
    char prefix[155];             /* 345 */
    char pad[12];                 /* 500 */
};

static unsigned int tarCheckSum(PosixTarHeader *header) {
    unsigned int sum = 0;
    char *p = (char *)header;
    char *q = p + sizeof(PosixTarHeader);
    for (int i = 0; i < 8; i++) {
        header->chksum[i] = ' ';
    }
    while (p < q) {
        sum += *p++ & 0xff;
    }
    return sum;
}

static PosixTarHeader *tarHeader(PosixTarHeader *header, char *fileName, ssize_t fileSize) {
    memset(header, 0, sizeof(PosixTarHeader));
    strcpy(header->name, fileName);
    sprintf(header->mode, "%07o", 0600);
    sprintf(header->size, "%011llo", (long long unsigned int)fileSize);
    sprintf(header->mtime, "%011o", 0);
    header->typeflag = '0';
    strcpy(header->magic, "ustar");
    strcpy(header->version, " ");
    sprintf(header->chksum, "%06o", tarCheckSum(header));
    return header;
}

static void createTarFile(std::string tarFile, std::string srcDir) {
    uint8_t buffer[BUFSIZE];
    struct dirent *dirent;
    struct stat st;
    PosixTarHeader header;
    ssize_t size, tarLen = 0, blockSize = sizeof(PosixTarHeader);
    char padding = '\0';

    DIR *dirp = opendir(srcDir.c_str());
    if (dirp == NULL) {
        ALOGD("Unable to open folder %s\n", srcDir.c_str());
        return;
    }

    int fdTar = open(tarFile.c_str(), O_WRONLY | O_CREAT, 0666);
    if (fdTar < 0) {
        ALOGD("Unable to open file %s\n", tarFile.c_str());
        closedir(dirp);
        return;
    }

    ALOGD("Creating tar file %s\n", tarFile.c_str());
    while ((dirent = readdir(dirp)) != NULL) {
        if (dirent->d_name[0] == '.') {
            continue;
        }

        std::string path = srcDir + "/" + dirent->d_name;
        int fd = open(path.c_str(), O_RDONLY);
        if (fd < 0) {
            ALOGD("Unable to open file %s\n", path.c_str());
            continue;
        }
        fstat(fd, &st);

        if (TEMP_FAILURE_RETRY(write(fdTar, tarHeader(
            &header, dirent->d_name, st.st_size), blockSize)) <= 0) {
            ALOGD("Error while writing file %s, errno=%d\n", tarFile.c_str(), errno);
            close(fd);
            continue;
        }
        tarLen += blockSize;

        while ((size = TEMP_FAILURE_RETRY(read(fd, buffer, BUFSIZE))) > 0) {
            write(fdTar, buffer, size);
            tarLen += size;
        }

        while (tarLen % blockSize != 0) {
            write(fdTar, &padding, 1);
            tarLen++;
        }
        close(fd);
    }
    close(fdTar);
    closedir(dirp);
}

static void dumpLogs(int fd, std::string srcDir, std::string destDir,
                     int maxFileNum, const char *logPrefix) {
    (void) fd;
    struct dirent **dirent_list = NULL;
    int num_entries = scandir(srcDir.c_str(),
                              &dirent_list,
                              0,
                              (int (*)(const struct dirent **, const struct dirent **)) alphasort);

    if (!dirent_list) {
        return;
    } else if (num_entries <= 0) {
        return;
    }

    int copiedFiles = 0;

    for (int i = num_entries - 1; i >= 0; i--) {
        ALOGD("Found %s\n", dirent_list[i]->d_name);

        if (0 != strncmp(dirent_list[i]->d_name, logPrefix, strlen(logPrefix))) {
            continue;
        }

        if ((copiedFiles >= maxFileNum) && (maxFileNum != -1)) {
            ALOGD("Skipped %s\n", dirent_list[i]->d_name);
            continue;
        }

        copiedFiles++;

        std::string srcLogFile = srcDir + "/" + dirent_list[i]->d_name;
        std::string destLogFile = destDir + "/" + dirent_list[i]->d_name;
        copyFile(srcLogFile, destLogFile);
    }

    while (num_entries--) {
        free(dirent_list[num_entries]);
    }

    free(dirent_list);
}

static void *dumpModemThread(void *data)
{
    long fdModem = (long)data;

    ALOGD("dumpModemThread started\n");

    std::string modemLogDir = android::base::GetProperty(MODEM_LOG_LOC_PROPERTY, "");
    if (modemLogDir.empty()) {
        ALOGD("No modem log place is set");
        return NULL;
    }

    std::string filePrefix = android::base::GetProperty(MODEM_LOG_PREFIX_PROPERTY, "");

    if (filePrefix.empty()) {
        ALOGD("Modem log prefix is not set");
        return NULL;
    }

    bool diagLogEnabled = android::base::GetBoolProperty(DIAG_MDLOG_PERSIST_PROPERTY, false);
    bool diagLogStarted = android::base::GetBoolProperty(DIAG_MDLOG_STATUS_PROPERTY, false);

    if (diagLogEnabled) {
        if (diagLogStarted) {
            android::base::SetProperty(DIAG_MDLOG_PROPERTY, "false");
            ALOGD("Stopping diag_mdlog...\n");
            if (android::base::WaitForProperty(DIAG_MDLOG_STATUS_PROPERTY, "false", std::chrono::seconds(10))) {
                ALOGD("diag_mdlog exited");
            } else {
                ALOGE("Waited mdlog timeout after 10 second");
            }
        } else {
            ALOGD("diag_mdlog is not running");
        }
    }

    sleep(1);
    ALOGD("Waited modem for 1 second to flush logs");

    const std::string modemLogCombined = modemLogDir + "/" + filePrefix + "all.tar";
    const std::string modemLogAllDir = modemLogDir + "/modem_log";

    RunCommandToFd(STDOUT_FILENO, "MKDIR MODEM LOG", {"/vendor/bin/mkdir", "-p", modemLogAllDir.c_str()}, CommandOptions::WithTimeout(2).Build());

    const std::string diagLogDir = "/data/vendor/radio/diag_logs/logs";
    const std::string diagPoweronLogPath = "/data/vendor/radio/diag_logs/logs/diag_poweron_log.qmdl";

    if (diagLogEnabled) {
        dumpLogs(STDOUT_FILENO, diagLogDir, modemLogAllDir, android::base::GetIntProperty(DIAG_MDLOG_NUMBER_BUGREPORT, 100), DIAG_LOG_PREFIX);

        if (diagLogStarted) {
            ALOGD("Restarting diag_mdlog...");
            android::base::SetProperty(DIAG_MDLOG_PROPERTY, "true");
        }
    }
    copyFile(diagPoweronLogPath, modemLogAllDir + "/" + basename(diagPoweronLogPath.c_str()));

    if (!PropertiesHelper::IsUserBuild()) {
        android::base::SetProperty(MODEM_EFS_DUMP_PROPERTY, "true");

        const std::string tcpdumpLogDir = "/data/vendor/tcpdump_logger/logs";
        const std::string extendedLogDir = "/data/vendor/radio/extended_logs";
        const std::vector<std::string> rilAndNetmgrLogs{
            "/data/vendor/radio/haldebug_ril0",
            "/data/vendor/radio/haldebug_ril1",
            "/data/vendor/radio/ril_log0",
            "/data/vendor/radio/ril_log0_old",
            "/data/vendor/radio/ril_log1",
            "/data/vendor/radio/ril_log1_old",
            "/data/vendor/radio/imsdatadaemon_log",
            "/data/vendor/radio/imsdatadaemon_log_old",
            "/data/vendor/radio/qmi_fw_log",
            "/data/vendor/radio/qmi_fw_log_old",
            "/data/vendor/netmgr/netmgr_log",
            "/data/vendor/netmgr/netmgr_log_old",
            "/data/vendor/radio/omadm_logs.txt",
            "/data/vendor/radio/power_anomaly_data.txt",
            "/data/vendor/radio/diag_logs/diag_trace.txt",
            "/data/vendor/radio/diag_logs/diag_trace_old.txt",
            "/data/vendor/radio/metrics_data",
            "/data/vendor/ssrlog/ssr_log.txt",
            "/data/vendor/ssrlog/ssr_log_old.txt",
            "/data/vendor/rfs/mpss/modem_efs",
        };

        bool tcpdumpEnabled = android::base::GetBoolProperty(TCPDUMP_PERSIST_PROPERTY, false);
        if (tcpdumpEnabled) {
            dumpLogs(STDOUT_FILENO, tcpdumpLogDir, modemLogAllDir, android::base::GetIntProperty(TCPDUMP_NUMBER_BUGREPORT, 5), TCPDUMP_LOG_PREFIX);
        }

        for (const auto& logFile : rilAndNetmgrLogs) {
            copyFile(logFile, modemLogAllDir + "/" + basename(logFile.c_str()));
        }

        dumpLogs(STDOUT_FILENO, extendedLogDir, modemLogAllDir, 100, EXTENDED_LOG_PREFIX);
        android::base::SetProperty(MODEM_EFS_DUMP_PROPERTY, "false");
    }

    createTarFile(modemLogCombined, modemLogAllDir);

    std::vector<uint8_t> buffer(65536);
    android::base::unique_fd fdLog(TEMP_FAILURE_RETRY(open(modemLogCombined.c_str(), O_RDONLY | O_CLOEXEC | O_NONBLOCK)));

    if (fdLog >= 0) {
        while (1) {
            ssize_t bytes_read = TEMP_FAILURE_RETRY(read(fdLog, buffer.data(), buffer.size()));

            if (bytes_read == 0) {
                break;
            } else if (bytes_read < 0) {
                ALOGD("read(%s): %s\n", modemLogCombined.c_str(), strerror(errno));
                break;
            }

            ssize_t result = TEMP_FAILURE_RETRY(write(fdModem, buffer.data(), bytes_read));

            if (result != bytes_read) {
                ALOGD("Failed to write %ld bytes, actually written: %ld", bytes_read, result);
                break;
            }
        }
    }

    RunCommandToFd(STDOUT_FILENO, "RM MODEM DIR", { "/vendor/bin/rm", "-r", modemLogAllDir.c_str()}, CommandOptions::WithTimeout(2).Build());
    RunCommandToFd(STDOUT_FILENO, "RM LOG", { "/vendor/bin/rm", modemLogCombined.c_str()}, CommandOptions::WithTimeout(2).Build());

    ALOGD("dumpModemThread finished\n");

    return NULL;
}

static void DumpTouch(int fd) {
    if (!access("/sys/devices/virtual/sec/tsp", R_OK)) {
        DumpFileToFd(fd, "LSI touch firmware version",
                     "/sys/devices/virtual/sec/tsp/fw_version");
        DumpFileToFd(fd, "LSI touch status",
                     "/sys/devices/virtual/sec/tsp/status");
        RunCommandToFd(fd, "Mutual Raw Data",
                       {"/vendor/bin/sh", "-c",
                        "echo run_rawdata_read_all > /sys/devices/virtual/sec/tsp/cmd"
                        " && cat /sys/devices/virtual/sec/tsp/cmd_result"});
        RunCommandToFd(fd, "Mutual Raw Cap",
                       {"/vendor/bin/sh", "-c",
                        "echo run_rawcap_read_all > /sys/devices/virtual/sec/tsp/cmd"
                        " && cat /sys/devices/virtual/sec/tsp/cmd_result"});
        RunCommandToFd(fd, "Mutual Strength",
                       {"/vendor/bin/sh", "-c",
                        "echo run_delta_read_all > /sys/devices/virtual/sec/tsp/cmd"
                        " && cat /sys/devices/virtual/sec/tsp/cmd_result"});
        RunCommandToFd(fd, "Self Raw",
                       {"/vendor/bin/sh", "-c",
                        "echo run_self_rawcap_read_all > /sys/devices/virtual/sec/tsp/cmd"
                        " && cat /sys/devices/virtual/sec/tsp/cmd_result"});
        RunCommandToFd(fd, "Self Strength",
                       {"/vendor/bin/sh", "-c",
                        "echo run_self_delta_read_all > /sys/devices/virtual/sec/tsp/cmd"
                        " && cat /sys/devices/virtual/sec/tsp/cmd_result"});
    }
    if (!access("/sys/devices/platform/soc/888000.i2c/i2c-2/2-0049", R_OK)) {
        DumpFileToFd(fd, "STM touch firmware version",
                     "/sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/appid");
        DumpFileToFd(fd, "STM touch status",
                     "/sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/status");
        RunCommandToFd(fd, "Mutual Raw",
                       {"/vendor/bin/sh", "-c",
                        "echo 13 00 > /sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/stm_fts_cmd"
                        " && cat /sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/stm_fts_cmd"});
        RunCommandToFd(fd, "Mutual Strength",
                       {"/vendor/bin/sh", "-c",
                        "echo 17 > /sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/stm_fts_cmd"
                        " && cat /sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/stm_fts_cmd"});
        RunCommandToFd(fd, "Self Raw",
                       {"/vendor/bin/sh", "-c",
                        "echo 15 00 > /sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/stm_fts_cmd"
                        " && cat /sys/devices/platform/soc/888000.i2c/i2c-2/2-0049/stm_fts_cmd"});
    }
}

static void DumpSensorLog(int fd) {
    const std::string logPath = "/data/vendor/sensors/log/sensor_log.txt";
    const std::string lastlogPath = "/data/vendor/sensors/log/sensor_lastlog.txt";

    if (!access(logPath.c_str(), R_OK)) {
        DumpFileToFd(fd, "sensor log", logPath);
    }
    if (!access(lastlogPath.c_str(), R_OK)) {
        DumpFileToFd(fd, "sensor lastlog", lastlogPath);
    }
}

static void DumpF2FS(int fd) {
    DumpFileToFd(fd, "F2FS", "/sys/kernel/debug/f2fs/status");
    RunCommandToFd(fd, "F2FS - fragmentation", {"/vendor/bin/sh", "-c",
                       "for d in $(ls /proc/fs/f2fs/); do "
                       "echo $d: /dev/block/mapper/`ls -l /dev/block/mapper | grep $d | awk '{print $8,$9,$10}'`; "
                       "cat /proc/fs/f2fs/$d/segment_info; done"});
}

static void DumpUFS(int fd) {
    DumpFileToFd(fd, "UFS model", "/sys/block/sda/device/model");
    DumpFileToFd(fd, "UFS rev", "/sys/block/sda/device/rev");
    DumpFileToFd(fd, "UFS size", "/sys/block/sda/size");
    DumpFileToFd(fd, "UFS show_hba", "/sys/kernel/debug/ufshcd0/show_hba");
    DumpFileToFd(fd, "UFS err_stats", "/sys/kernel/debug/ufshcd0/stats/err_stats");
    DumpFileToFd(fd, "UFS io_stats", "/sys/kernel/debug/ufshcd0/stats/io_stats");
    DumpFileToFd(fd, "UFS req_stats", "/sys/kernel/debug/ufshcd0/stats/req_stats");

    std::string bootdev = android::base::GetProperty(UFS_BOOTDEVICE, "");
    if (!bootdev.empty()) {
        DumpFileToFd(fd, "UFS Slow IO Read", "/sys/devices/platform/soc/" + bootdev + "/slowio_read_cnt");
        DumpFileToFd(fd, "UFS Slow IO Write", "/sys/devices/platform/soc/" + bootdev + "/slowio_write_cnt");
        DumpFileToFd(fd, "UFS Slow IO Unmap", "/sys/devices/platform/soc/" + bootdev + "/slowio_unmap_cnt");
        DumpFileToFd(fd, "UFS Slow IO Sync", "/sys/devices/platform/soc/" + bootdev + "/slowio_sync_cnt");

        std::string ufs_health = "for f in $(find /sys/devices/platform/soc/" + bootdev + "/health -type f); do if [[ -r $f && -f $f ]]; then echo --- $f; cat $f; echo ''; fi; done";
        RunCommandToFd(fd, "UFS health", {"/vendor/bin/sh", "-c", ufs_health.c_str()});
    }
}

static void DumpPower(int fd) {
    RunCommandToFd(fd, "Power Stats Times", {"/vendor/bin/sh", "-c",
                   "echo -n \"Boot: \" && /vendor/bin/uptime -s &&"
                   "echo -n \"Now: \" && date"});
    DumpFileToFd(fd, "Sleep Stats", "/sys/power/system_sleep/stats");
    DumpFileToFd(fd, "Power Management Stats", "/sys/power/rpmh_stats/master_stats");
    DumpFileToFd(fd, "WLAN Power Stats", "/d/wlan0/power_stats");
}

static void DumpVibrator(int fd) {
    const std::string dir = "/sys/class/leds/vibrator/device/";
    const std::vector<std::string> files {
        "comp_enable",
        "cp_dig_scale",
        "cp_trigger_index",
        "cp_trigger_queue",
        "dig_scale",
        "f0_stored",
        "heartbeat",
        "leds/vibrator/activate",
        "leds/vibrator/duration",
        "leds/vibrator/state",
        "num_waves",
        "redc_stored",
        "standby_timeout",
    };

    for (const auto &file : files) {
        DumpFileToFd(fd, "Vibrator", dir+file);
    }
}

// Methods from ::android::hardware::dumpstate::V1_0::IDumpstateDevice follow.
Return<void> DumpstateDevice::dumpstateBoard(const hidl_handle& handle) {
    // Ignore return value, just return an empty status.
    dumpstateBoard_1_1(handle, DumpstateMode::DEFAULT, 30 * 1000 /* timeoutMillis */);
    return Void();
}

// Methods from ::android::hardware::dumpstate::V1_1::IDumpstateDevice follow.
Return<DumpstateStatus> DumpstateDevice::dumpstateBoard_1_1(const hidl_handle& handle,
                                                            const DumpstateMode mode,
                                                            const uint64_t timeoutMillis) {
    // Unused arguments.
    (void) timeoutMillis;
    // Exit when dump is completed since this is a lazy HAL.
    addPostCommandTask([]() {
        exit(0);
    });

    if (handle == nullptr || handle->numFds < 1) {
        ALOGE("no FDs\n");
        return DumpstateStatus::ILLEGAL_ARGUMENT;
    }

    int fd = handle->data[0];
    if (fd < 0) {
        ALOGE("invalid FD: %d\n", handle->data[0]);
        return DumpstateStatus::ILLEGAL_ARGUMENT;
    }

    bool isModeValid = false;
    for (const auto dumpstateMode : hidl_enum_range<DumpstateMode>()) {
        if (mode == dumpstateMode) {
            isModeValid = true;
            break;
        }
    }
    if (!isModeValid) {
        ALOGE("Invalid mode: %d\n", mode);
        return DumpstateStatus::ILLEGAL_ARGUMENT;
    } else if (mode == DumpstateMode::WEAR) {
        // We aren't a Wear device.
        ALOGE("Unsupported mode: %d\n", mode);
        return DumpstateStatus::UNSUPPORTED_MODE;
    }

    RunCommandToFd(fd, "Notify modem", {"/vendor/bin/modem_svc", "-s"}, CommandOptions::WithTimeout(1).Build());

    pthread_t modemThreadHandle = 0;
    if (getVerboseLoggingEnabled()) {
        ALOGD("Verbose logging is enabled.\n");
        if (handle->numFds < 2) {
            ALOGE("no FD for modem\n");
        } else {
            int fdModem = handle->data[1];
            if (pthread_create(&modemThreadHandle, NULL, dumpModemThread, (void *)((long)fdModem)) != 0) {
                ALOGE("could not create thread for dumpModem\n");
            }
        }
    }

    RunCommandToFd(fd, "VENDOR PROPERTIES", {"/vendor/bin/getprop"});
    DumpFileToFd(fd, "SoC serial number", "/sys/devices/soc0/serial_number");
    DumpFileToFd(fd, "CPU present", "/sys/devices/system/cpu/present");
    DumpFileToFd(fd, "CPU online", "/sys/devices/system/cpu/online");
    DumpTouch(fd);

    DumpF2FS(fd);
    DumpUFS(fd);

    DumpSensorLog(fd);

    DumpFileToFd(fd, "INTERRUPTS", "/proc/interrupts");

    DumpPower(fd);

    DumpFileToFd(fd, "LL-Stats", "/d/wlan0/ll_stats");
    DumpFileToFd(fd, "WLAN Connect Info", "/d/wlan0/connect_info");
    DumpFileToFd(fd, "WLAN Offload Info", "/d/wlan0/offload_info");
    DumpFileToFd(fd, "WLAN Roaming Stats", "/d/wlan0/roam_stats");
    DumpFileToFd(fd, "ICNSS Stats", "/d/icnss/stats");
    DumpFileToFd(fd, "SMD Log", "/d/ipc_logging/smd/log");
    RunCommandToFd(fd, "ION HEAPS", {"/vendor/bin/sh", "-c", "for d in $(ls -d /d/ion/*); do for f in $(ls $d); do echo --- $d/$f; cat $d/$f; done; done"});
    DumpFileToFd(fd, "dmabuf info", "/d/dma_buf/bufinfo");
    RunCommandToFd(fd, "Temperatures", {"/vendor/bin/sh", "-c", "for f in /sys/class/thermal/thermal* ; do type=`cat $f/type` ; temp=`cat $f/temp` ; echo \"$type: $temp\" ; done"});
    RunCommandToFd(fd, "Cooling Device Current State", {"/vendor/bin/sh", "-c", "for f in /sys/class/thermal/cooling* ; do type=`cat $f/type` ; temp=`cat $f/cur_state` ; echo \"$type: $temp\" ; done"});
    RunCommandToFd(
        fd, "LMH info",
        {"/vendor/bin/sh", "-c",
         "for f in /sys/bus/platform/drivers/msm_lmh_dcvs/*qcom,limits-dcvs@*/lmh_freq_limit; do "
         "state=`cat $f` ; echo \"$f: $state\" ; done"});
    RunCommandToFd(fd, "CPU time-in-state", {"/vendor/bin/sh", "-c", "for cpu in /sys/devices/system/cpu/cpu*; do f=$cpu/cpufreq/stats/time_in_state; if [ ! -f $f ]; then continue; fi; echo $f:; cat $f; done"});
    RunCommandToFd(fd, "CPU cpuidle", {"/vendor/bin/sh", "-c", "for cpu in /sys/devices/system/cpu/cpu*; do for d in $cpu/cpuidle/state*; do if [ ! -d $d ]; then continue; fi; echo \"$d: `cat $d/name` `cat $d/desc` `cat $d/time` `cat $d/usage`\"; done; done"});
    RunCommandToFd(fd, "Easel debug info", {"/vendor/bin/sh", "-c", "for f in `ls /sys/devices/platform/soc/a88000.i2c/i2c-0/0-0066/@(*curr|temperature|vbat|total_power)`; do echo \"$f: `cat $f`\" ; done; file=/sys/devices/virtual/misc/mnh_sm/state; echo \"$file: `cat $file`\""});
    DumpFileToFd(fd, "MDP xlogs", "/data/vendor/display/mdp_xlog");
    DumpFileToFd(fd, "TCPM logs", "/d/tcpm/usbpd0");
    DumpFileToFd(fd, "PD Engine", "/d/logbuffer/usbpd");
    DumpFileToFd(fd, "smb-lib", "/d/logbuffer/smblib");
    DumpFileToFd(fd, "WLC logs", "/d/logbuffer/wireless");
    DumpFileToFd(fd, "ipc-local-ports", "/d/msm_ipc_router/dump_local_ports");
    DumpFileToFd(fd, "ipc-servers", "/d/msm_ipc_router/dump_servers");
    RunCommandToFd(fd, "ipc-logs",
                   {"/vendor/bin/sh", "-c",
                    "for f in `ls /d/ipc_logging/*_IPCRTR/log` ; do echo \"------ $f\\n`cat "
                    "$f`\\n\" ; done"});
    RunCommandToFd(fd, "USB Device Descriptors", {"/vendor/bin/sh", "-c", "cd /sys/bus/usb/devices/1-1 && cat product && cat bcdDevice; cat descriptors | od -t x1 -w16 -N96"});
    RunCommandToFd(fd, "Power supply properties", {"/vendor/bin/sh", "-c", "for f in `ls /sys/class/power_supply/*/uevent` ; do echo \"------ $f\\n`cat $f`\\n\" ; done"});
    RunCommandToFd(fd, "PMIC Votables", {"/vendor/bin/sh", "-c", "cat /sys/kernel/debug/pmic-votable/*/status"});
    DumpFileToFd(fd, "Battery cycle count", "/sys/class/power_supply/bms/device/cycle_counts_bins");
    DumpFileToFd(fd, "FG cycle count", "/sys/class/power_supply/maxfg/cycle_counts_bins");
    DumpFileToFd(fd, "Maxim FG History", "/dev/maxfg_history");
    DumpFileToFd(fd, "Maxim FG registers", "/d/regmap/4-0036/registers");
    DumpFileToFd(fd, "Maxim FG NV RAM", "/d/regmap/4-000b/registers");
    RunCommandToFd(fd, "QCOM FG SRAM", {"/vendor/bin/sh", "-c", "echo 0 > /d/fg/sram/address ; echo 500 > /d/fg/sram/count ; cat /d/fg/sram/data"});
    DumpFileToFd(fd, "WLC VER", "/sys/devices/platform/soc/a88000.i2c/i2c-0/0-0061/version");
    DumpFileToFd(fd, "WLC STATUS", "/sys/devices/platform/soc/a88000.i2c/i2c-0/0-0061/status");

    RunCommandToFd(fd, "eSIM Status", {"/vendor/bin/sh", "-c", "od -t x1 /sys/firmware/devicetree/base/chosen/cdt/cdb2/esim"});
    DumpFileToFd(fd, "Modem Stat", "/data/vendor/modem_stat/debug.txt");
    DumpFileToFd(fd, "Pixel trace", "/d/tracing/instances/pixel-trace/trace");

    /* Battery Defend setting */
    RunCommandToFd(
        fd, "TEMP-DEFEND Config",
        {"/vendor/bin/sh", "-c",
         " cd /sys/devices/platform/soc/soc:google,charger; for f in `ls bd_*` ; do echo "
         "\"$f: `cat $f`\" ; done"});
    RunCommandToFd(
        fd, "DWELL-DEFEND Config",
        {"/vendor/bin/sh", "-c",
         " cd /sys/devices/platform/soc/soc:google,charger; for f in `ls charge_s*` ; do echo "
         "\"$f: `cat $f`\" ; done"});

    // Slower dump put later in case stuck the rest of dump
    // Timeout after 3s as TZ log missing EOF
    RunCommandToFd(fd, "QSEE logs", {"/vendor/bin/sh", "-c", "/vendor/bin/timeout 3 cat /d/tzdbg/qsee_log"});

    // Citadel info
    RunCommandToFd(fd, "Citadel VERSION", {"/vendor/bin/hw/citadel_updater", "-lv"});
    RunCommandToFd(fd, "Citadel STATS", {"/vendor/bin/hw/citadel_updater", "--stats"});
    RunCommandToFd(fd, "Citadel BOARDID", {"/vendor/bin/hw/citadel_updater", "--board_id"});

    DumpVibrator(fd);

    // Dump various events in WiFi data path
    DumpFileToFd(fd, "WLAN DP Trace", "/d/wlan/dpt_stats/dump_set_dpt_logs");

    // Keep this at the end as very long on not for humans
    DumpFileToFd(fd, "WLAN FW Log Symbol Table", "/vendor/firmware/Data.msc");

    // Dump fastrpc dma buffer size
    DumpFileToFd(fd, "Fastrpc dma buffer", "/sys/kernel/fastrpc/total_dma_kb");

    // Dump page owner
    DumpFileToFd(fd, "Page Owner", "/sys/kernel/debug/page_owner");
    if (modemThreadHandle) {
        pthread_join(modemThreadHandle, NULL);
    }

    return DumpstateStatus::OK;
}

Return<void> DumpstateDevice::setVerboseLoggingEnabled(const bool enable) {
    android::base::SetProperty(VENDOR_VERBOSE_LOGGING_ENABLED_PROPERTY, enable ? "true" : "false");
    return Void();
}

Return<bool> DumpstateDevice::getVerboseLoggingEnabled() {
    return android::base::GetBoolProperty(VENDOR_VERBOSE_LOGGING_ENABLED_PROPERTY, false);
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace dumpstate
}  // namespace hardware
}  // namespace android
