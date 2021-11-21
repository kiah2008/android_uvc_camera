//
// Created by zhaokai on 2020/6/28.
//

#include <sys/resource.h>
#include "cpu_manipulate.h"
#include "device_affinity.h"
#include <port/clog.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <ios>

namespace {
    inline void readContentFromFile(const char* path, char** content) {
        std::ifstream ifile;
        ifile.open(path, std::ios::binary);
        if (ifile) {
            int length = 1;
            *content = new char[length];
            ifile.seekg(0, ifile.beg);
            ifile.read(*content, length);
            ifile.close();
        }
    }

    bool core_is_reverse() {
        bool result = false;
        const char* path = "/sys/devices/system/cpu/cpu0/core_ctl/is_big_cluster";
        char* is_big_cluster = nullptr;
        readContentFromFile(path, &is_big_cluster);

        std::unique_ptr<char> wrapper_ptr{
                is_big_cluster};  //为了方便释放is_big_cluster

        if (nullptr != is_big_cluster) {
            if ('1' == (*is_big_cluster)) {
                result = true;
            }
        } else {
        }

        return result;
    }

}  // namespace


void cutils::bind_core_and_set_priority() {
    if(is_binder_core){
        int mask = BIG_CORE;
        if(is_binder_big_core) {
            CLOGD("bind big core");
        }else {
            mask = SMALL_CORE;
            CLOGD("bind small core");
        }
        set_current_thread_affinity_mask(mask);
    }
    set_current_thread_priority(HIGH_PRIORITY);
}

void cutils::set_current_thread_priority(int prio) {
    pid_t tid = gettid();
    int pri = getpriority(PRIO_PROCESS, tid);

    setpriority(PRIO_PROCESS, tid, prio);

//    pri = getpriority(PRIO_PROCESS, tid);
}

void cutils::set_current_thread_affinity_mask(int mask) {
    int syscallres;
    pid_t tid = gettid();

    if (core_is_reverse()) {
        mask = ~mask;
    }

    syscallres = syscall(__NR_sched_setaffinity, tid, sizeof(mask), &mask);
    if (syscallres) {
    }
}

int cutils::get_cpu_id() {
    int syscallres, cpu_id;
    syscallres = syscall(__NR_getcpu, &cpu_id, NULL, NULL);
    if (syscallres) {
        return -1;
    }

    return cpu_id;
}

float cutils::get_cpu_freq() {
    static char path[256];
    sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq",
            get_cpu_id());
    FILE* fin = fopen(path, "r");
    if (fin == nullptr)
        return -1;
    long long freq = -1;
    if (fscanf(fin, "%lld", &freq) != 1) {
        fclose(fin);
        return -1;
    }
    fclose(fin);
    return freq / 1000000.f;
}
