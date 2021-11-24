//
// Created by kiah on 2021/11/21.
//
#include <utils/ScopedTimer.h>
#include <port/clog.h>

namespace cutils {
    ScopedTimer::ScopedTimer(const char* name) {
        finished = true;
        reset(name);
    }

    ScopedTimer::~ScopedTimer() noexcept {
        if (!finished) {
            finish();
        }
    }

    void ScopedTimer::finish() {
        CLOGD("leave %s %.3fms", m_name, m_timer.get_msecs());
        finished = true;
        CUTILS_TRACE_END();
    }

    void ScopedTimer::reset(const char *name) {
        if (!finished) {
            finish();
        }
        CUTILS_TRACE_BEGIN(name);
        m_name = name;
        finished = false;
        m_timer.reset();
        CLOGD("enter %s", name);
    }
}