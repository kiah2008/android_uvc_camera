//
// Created by zhaokai on 2020/6/16.
//

#ifndef COREUTILS_EXECUTOR_H
#define COREUTILS_EXECUTOR_H

#include <thread/thread_option.h>
#include <base/Status.h>
#include <functional>
#include <pthread.h>
#include <vector>
#include <deque>
#include <condition_variable>
#include <string>

namespace cutils {

#define DEFAULT_EXECUTOR ""

    struct SchedulerShared {
        // When a non-source node returns StatusStop() or
        // CalculatorGraph::CloseAllPacketSources is called, the graph starts to
        // terminate: all source nodes are closed (at the next scheduling
        // opportunity), and the graph continues running until it is done. This
        // flag indicates that the graph is in that mode.
        std::atomic<bool> stopping;
        std::atomic<bool> has_error = ATOMIC_VAR_INIT(false);
        std::function<void(const Status& error)> error_callback;
        // Collects timing information for measuring overhead.
//            internal::SchedulerTimer timer;
    };

    // Abstract base class for the task queue.
// NOTE: The task queue orders the ready tasks by their priorities. This
// enables the executor to run ready tasks in priority order.
    class TaskQueue {
    public:
        virtual ~TaskQueue();

        // Runs the next ready task in the current thread. Should be invoked by the
        // executor. This method should be called exactly as many times as AddTask
        // was called on the executor.
        virtual void RunNextTask() = 0;
    };

// Abstract base class for the Executor.
    class Executor {
    public:
        virtual ~Executor();

        // A registered Executor subclass must implement the static factory method
        // Create.  The Executor subclass cannot be registered without it.
        //
        // static ::cutils::StatusOr<Executor*> Create(
        //     const MediaPipeOptions& extendable_options);
        //
        // Create validates extendable_options, then calls the constructor, and
        // returns the newly allocated Executor object.

        // The scheduler queue calls this method to tell the executor that it has
        // a new task to run. The executor should use its execution mechanism to
        // invoke task_queue->RunNextTask.
        virtual void AddTask(TaskQueue* task_queue) {
            Schedule([task_queue] { task_queue->RunNextTask(); });
        }

        // Schedule the specified "task" for execution in this executor.
        virtual void Schedule(std::function<void()> task) = 0;
    };


}

#endif //COREUTILS_EXECUTOR_H
