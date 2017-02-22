/*
 * Copyright 2017 The Android Open Source Project
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

#include <dumpstate.h>
#include <cutils/properties.h>
#include <libgen.h>
#include <stdlib.h>
#include <string>
#include <android-base/properties.h>

#include <cutils/log.h>

#define MODEM_LOG_PREFIX_PROPERTY "ro.radio.log_prefix"
#define MODEM_LOGGING_SWITCH "persist.radio.smlog_switch"

static void getModemLogs(Dumpstate &ds)
{
    bool modemLogsEnabled = 0;

    /* Check if smlog_dump tool exist */
    if (!ds.IsUserBuild() && !access("/system/bin/smlog_dump", F_OK)) {
        modemLogsEnabled = android::base::GetBoolProperty(MODEM_LOGGING_SWITCH, false);

        /* Execute SMLOG DUMP if SMLOG is enabled */
        if (modemLogsEnabled && !ds.GetPath("").empty()) {
            std::string bugreportDir = dirname(ds.GetPath("").c_str());
            CommandOptions options = CommandOptions::WithTimeout(120).AsRoot().Build();
            ds.RunCommand("SMLOG DUMP", { "smlog_dump", "-d", "-o", bugreportDir.c_str() }, options);

            // Remove smlog folders older than 10 days.
            std::string filePrefix = android::base::GetProperty(MODEM_LOG_PREFIX_PROPERTY, "");
            if (!filePrefix.empty()) {

                std::string removeCommand = "/system/bin/find " +
                    bugreportDir + "/" + filePrefix + "* -mtime +10 -delete";

                ds.RunCommand("RM OLD SMLOG",
                              { "/system/bin/sh", "-c", removeCommand.c_str()},
                              CommandOptions::AS_ROOT_5);
            }
        }
    }
}


void dumpstate_board()
{
    Dumpstate& ds = Dumpstate::GetInstance();

    getModemLogs(ds);
    ds.DumpFile("CPU present", "/sys/devices/system/cpu/present");
    ds.DumpFile("CPU online", "/sys/devices/system/cpu/online");
    ds.DumpFile("RPM Stats", "/d/rpm_stats");
    ds.DumpFile("Power Management Stats", "/d/rpm_master_stats");
    ds.DumpFile("SMD Log", "/d/ipc_logging/smd/log");
    ds.RunCommand("ION HEAPS", {"/system/bin/sh", "-c", "for d in $(ls -d /d/ion/*); do for f in $(ls $d); do echo --- $d/$f; cat $d/$f; done; done"}, CommandOptions::AS_ROOT_5);
    ds.DumpFile("dmabuf info", "/d/dma_buf/bufinfo");
    ds.RunCommand("Temperatures", {"/system/bin/sh", "-c", "for f in `ls /sys/class/thermal` ; do type=`cat /sys/class/thermal/$f/type` ; temp=`cat /sys/class/thermal/$f/temp` ; echo \"$type: $temp\" ; done"}, CommandOptions::AS_ROOT_5);
    ds.DumpFile("cpu0-3 time-in-state", "/sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state");
    ds.RunCommand("cpu0-3 cpuidle", {"/system/bin/sh", "-c", "for d in $(ls -d /sys/devices/system/cpu/cpu0/cpuidle/state*); do echo \"$d: `cat $d/name` `cat $d/desc` `cat $d/time` `cat $d/usage`\"; done"}, CommandOptions::AS_ROOT_5);
    ds.DumpFile("cpu4-7 time-in-state", "/sys/devices/system/cpu/cpu4/cpufreq/stats/time_in_state");
    ds.RunCommand("cpu4-7 cpuidle", {"/system/bin/sh", "-c", "for d in $(ls -d /sys/devices/system/cpu/cpu4/cpuidle/state*); do echo \"$d: `cat $d/name` `cat $d/desc` `cat $d/time` `cat $d/usage`\"; done"}, CommandOptions::AS_ROOT_5);
    ds.DumpFile("MDP xlogs", "/d/mdp/xlog/dump");
    ds.RunCommand("RAMDUMP LIST", {"/system/bin/sh", "-c", "cat /data/data/com.android.ramdump/files/RAMDUMP_LIST"}, CommandOptions::AS_ROOT_5);

    /* Check if qsee_logger tool exists */
    if (!access("/vendor/bin/qsee_logger", F_OK)) {
        ds.RunCommand("FP LOGS", {"qsee_logger", "-d"});
    }
};
