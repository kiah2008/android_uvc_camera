//
// Created by zhaokai on 2020/6/28.
//

#ifndef COREUTILS_THREADPOOLEXECUTOROPTIONS_H
#define COREUTILS_THREADPOOLEXECUTOROPTIONS_H

#include <stdint.h>

namespace cutils {

    class ThreadPoolExecutorOptions {

    public:
        typedef enum {
            AUTO = 0,
            LOW = 1,
            HIGH = 2,
        } ThreadExecutorPerformance;

        int32_t num_thread_ = -1;
        ThreadExecutorPerformance performance_ = HIGH;
        const char *prefix_ = nullptr;

        ThreadPoolExecutorOptions(int32_t num_thread, ThreadExecutorPerformance perf,
                                  const char *prefix) : num_thread_(num_thread), performance_(perf),
                                                        prefix_(prefix) {}

        void setPerformance(ThreadExecutorPerformance performance) {
            performance_ = performance;
        }

        int nice_priority_level_;

        void setNicePriorityLevel(int nicePriorityLevel) {
            nice_priority_level_ = nicePriorityLevel;
        }
        // Nice priority level of the workers

        const int32_t num_threads() const {
            return num_thread_;
        }

        void setNumThread(int32_t numThread) {
            num_thread_ = numThread;
        }

        int nice_priority_level() const { return nice_priority_level_; }

        int require_processor_performance() const {
            return performance_;
        }
    };
}


#endif //COREUTILS_THREADPOOLEXECUTOROPTIONS_H
