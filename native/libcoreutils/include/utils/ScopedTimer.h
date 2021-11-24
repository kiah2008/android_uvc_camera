//
// Created by kiah on 2021/11/21.
//

#ifndef UVCCAMERA_MASTER_SCOPEDTIMER_H
#define UVCCAMERA_MASTER_SCOPEDTIMER_H

#include <chrono>

#ifdef SUPPORT_TRACE
#include <android/trace.h>
#endif

namespace cutils {

    class Timer {
        std::chrono::high_resolution_clock::time_point m_start;

    public:
        Timer() { reset(); }

        void reset() { m_start = std::chrono::high_resolution_clock::now(); }

        double get_secs() const {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(now -
                                                                        m_start)
                           .count() *
                   1e-9;
        }

        double get_msecs() const { return get_secs() * 1e3; }

        double get_secs_reset() {
            auto ret = get_secs();
            reset();
            return ret;
        }

        double get_msecs_reset() { return get_secs_reset() * 1e3; }
    };

    class ScopedTimer {
        const char *m_name;
        Timer m_timer;
        bool finished;

    public:
        ScopedTimer(const char *name);

        ~ScopedTimer() noexcept;

        void reset(const char *name);

        void finish();
    };
}

#define SUBCOMBINE(x, y) x##y
#define COMBINE(x, y) COMBINE(x,y)

#define SCOPED_TIMER(x) COMBINE(x, __LINE__)
#define SCOPED_TIMER_FUNC() COMBINE(__FUNCTION__, __LINE__)

#endif //UVCCAMERA_MASTER_SCOPEDTIMER_H
