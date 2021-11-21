//
// Created by zhaokai on 2020/6/28.
//

#include <cpu/cpu_util.h>
#include <thread/ThreadPoolExecutor.h>
#include <port/clog.h>

namespace cutils {
// static
    std::shared_ptr<Executor> ThreadPoolExecutor::Create(
            const ThreadPoolExecutorOptions &extendable_options) {
//        if (options.num_threads() <= 0) {
//            return InvalidArgumentErrorBuilder(MEDIAPIPE_LOC)
//                    << "The num_threads field in ThreadPoolExecutorOptions should be "
//                       "positive but is "
//                    << options.num_threads();
//        }
//
        ThreadOptions thread_options;
//        if (options.has_stack_size()) {
//            // thread_options.set_stack_size() takes a size_t as input, so we must not
//            // pass a negative value. 0 has a special meaning (the default thread
//            // stack size for the system), so we also avoid that.
//            if (options.stack_size() <= 0) {
//                return InvalidArgumentErrorBuilder(MEDIAPIPE_LOC)
//                        << "The stack_size field in ThreadPoolExecutorOptions should be "
//                           "positive but is "
//                        << options.stack_size();
//            }
//            thread_options.set_stack_size(options.stack_size());
//        }
        if (extendable_options.nice_priority_level() != 0) {
            thread_options.set_nice_priority_level(extendable_options.nice_priority_level());
        }
/*        if (extendable_options.has_thread_name_prefix()) {
            thread_options.set_name_prefix(extendable_options.thread_name_prefix());
        }*/
#if defined(__linux__)
        switch (extendable_options.require_processor_performance()) {
            case ThreadPoolExecutorOptions::LOW:
                thread_options.set_cpu_set(InferLowerCoreIds());
                break;
            case  ThreadPoolExecutorOptions::AUTO:
                //ignore
                break;
            case ThreadPoolExecutorOptions::HIGH:
                thread_options.set_cpu_set(InferHigherCoreIds());
                break;
            default:
                break;
        }
#endif
        return std::shared_ptr<Executor>(new ThreadPoolExecutor(thread_options, extendable_options.num_threads()));
    }

    ThreadPoolExecutor::ThreadPoolExecutor(int num_threads)
            : thread_pool_("cutils", num_threads) {
        Start();
    }

    ThreadPoolExecutor::ThreadPoolExecutor(const ThreadOptions &thread_options,
                                           int num_threads)
            : thread_pool_(thread_options,
                           thread_options.name_prefix().empty()
                           ? "cutils"
                           : thread_options.name_prefix(),
                           num_threads) {
        Start();
    }

    ThreadPoolExecutor::~ThreadPoolExecutor() {
        CLOGD("Thread pool destroyed.");
    }

    void ThreadPoolExecutor::Schedule(std::function<void()> task) {
        thread_pool_.Schedule(std::move(task));
    }

    void ThreadPoolExecutor::Start() {
        stack_size_ = thread_pool_.thread_options().stack_size();
        thread_pool_.StartWorkers();
        CLOGD("Started thread pool with %d threads", thread_pool_.num_threads());
    }

} //NS cutils