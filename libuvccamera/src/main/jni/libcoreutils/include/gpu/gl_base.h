//
// Created by zhaokai on 2020/7/14.
//

#ifndef HUMANVIDEOEFFECTS_GL_BASE_H
#define HUMANVIDEOEFFECTS_GL_BASE_H

#define HAS_EGL 1

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

//#ifdef __ANDROID__
//// Weak-link all GL APIs included from this point on.
//// TODO: Annotate these with availability attributes for the
//// appropriate versions of Android, by including gl{3,31,31}.h and resetting
//// GL_APICALL for each.
//#undef GL_APICALL
//#define GL_APICALL __attribute__((weak_import)) KHRONOS_APICALL
//#endif  // __ANDROID__

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

#include <port/clog.h>
// When using the Linux EGL headers, we may end up pulling a
#undef Status

// More crud from X
#undef None
#undef Bool
#undef Success

namespace cutils {
    // Doing this as an inline function allows us to avoid unwanted "pointer will
    // never be null" errors on certain platforms and compilers.
    template <typename T>
    inline bool SymbolAvailable(T* symbol) {
        return symbol != nullptr;
    }
}

#endif //HUMANVIDEOEFFECTS_GL_BASE_H
