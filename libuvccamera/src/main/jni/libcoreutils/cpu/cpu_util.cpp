//
// Created by zhaokai on 2020/6/28.
//

#include "cpu_util.h"
#include <port/base_types.h>
#include <inttypes.h>
#include <string>
#include <fstream>
#include <cctype>
#include <vector>
#include <algorithm>
#include <bits/sysconf.h>
#include <port/clog.h>

namespace cutils {
    namespace {

        constexpr uint32 kBufferLength = 64;

        std::string GetFilePath(int cpu) {
            char path[128];
            snprintf(path, 128, "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", cpu);
            return std::string(path);
        }

        uint64 GetCpuMaxFrequency(int cpu) {
            auto path_or_status = GetFilePath(cpu);
            if (path_or_status.empty()) {
                return 0;
            }
            std::ifstream file;
            file.open(path_or_status);
            if (file.is_open()) {
                char buffer[kBufferLength];
                file.getline(buffer, kBufferLength);
                file.close();
                uint64 frequency = atoll(buffer);
                return frequency;
            } else {
                CLOGE("couldn't read %s", path_or_status.c_str());
                return 0;
            }
        }

        std::set<int> InferLowerOrHigherCoreIds(bool lower) {
            std::vector<std::pair<int, uint64>> cpu_freq_pairs;
            int cores = NumCPUCores();
            CLOGD("num cores %d", cores);
            for (int cpu = 0; cpu < cores; ++cpu) {
                auto freq = GetCpuMaxFrequency(cpu);
                if (freq > 0) {
                    cpu_freq_pairs.push_back({cpu, freq});
                }
            }
            if (cpu_freq_pairs.empty()) {
                CLOGW("fail to query cores' info");
                return {};
            }

            std::sort(cpu_freq_pairs.begin(), cpu_freq_pairs.end(),
                      [lower](const std::pair<int, uint64> &left,
                              const std::pair<int, uint64> &right) {
                          return (lower && left.second < right.second) ||
                                 (!lower && left.second > right.second);
                      });

            uint64 edge_freq = cpu_freq_pairs[0].second;

            std::set<int> inferred_cores;
            for (const auto &cpu_freq_pair : cpu_freq_pairs) {
                if ((lower && cpu_freq_pair.second > edge_freq) ||
                    (!lower && cpu_freq_pair.second < edge_freq)) {
                    break;
                }
                inferred_cores.insert(cpu_freq_pair.first);
            }

            // If all the cores have the same frequency, there are no "lower" or "higher"
            // cores.
            if (inferred_cores.size() == cpu_freq_pairs.size()) {
                return {};
            } else {
                return inferred_cores;
            }
        }
    }  // namespace

    int NumCPUCores() {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }

    std::set<int> InferLowerCoreIds() {
        return InferLowerOrHigherCoreIds(/* lower= */ true);
    }

    std::set<int> InferHigherCoreIds() {
        return InferLowerOrHigherCoreIds(/* lower= */ false);
    }
}