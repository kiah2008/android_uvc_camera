//
// Created by zhaokai on 2020/6/30.
//

#ifndef COREUTILS_LOGGER_H
#define COREUTILS_LOGGER_H

#define DF_SEGMENT_TEST_ENABLED

#ifdef DF_SEGMENT_TEST_ENABLED

#if defined(__ANDROID__)

#include <android/log.h>

#endif

#ifndef LOG_TAG
#define LOG_TAG  "kiah/cutils"
#endif


#define CLOG(fmt, ...)    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ## __VA_ARGS__)
#define CLOGD(fmt, ...)   __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ## __VA_ARGS__)
#define CLOGW(fmt, ...)   __android_log_print(ANDROID_LOG_WARN, LOG_TAG, fmt, ## __VA_ARGS__)
#define CLOGE(fmt, ...)   __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
#define CLOGF(fmt, ...)   __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, fmt, ## __VA_ARGS__)


#define LOCAL_VARIABLE(x, y) _l##x(y)
#define CUTILS_TIMER(func) ScopedTimer LOCAL_VARIABLE(__LINE__, func)

#define CUTILS_CHECK_LOG(x, fmt...) if(__builtin_expect(!(x), 0)) {CLOGW(fmt); }
#define CUTILS_RETURN_IF_ERROR(x, fmt...) if(__builtin_expect(!(x), 0)) {CLOG(fmt); return; }
#define CUTILS_RETURN_STATUS_IF_ERROR(x, err) if(__builtin_expect(!(x), 0)) {return err; }
#define CUTILS_CHECK_RETURN_STATUS(x, fail_code, fmt...) if(__builtin_expect(!(x), 0)) {CLOG(fmt); return fail_code;}
#define CUTILS_CHECK_STATUS_RET(x, fmt...) if(__builtin_expect((x.ok())==CUTILS_STATUS_OK, 0)) {CLOG(fmt); return x;}
#define CUTILS_CHECK(x) if(__builtin_expect(!(x), 0)) {CLOGE("Assert Failed %s at %s:%d", #x, __FUNCTION__, __LINE__); assert(x);}
#define CUTILS_ASSERT_LOG(x, fmt...) if(__builtin_expect(!(x), 0)) {CLOGE(fmt); assert(x);}
#define CUTILS_ASSERT_OK(x) assert(x == CUTILS_STATUS_OK);

#define CUTILS_CHECK_EQ(val1, val2) CUTILS_CHECK(val1 == val2)
#define CUTILS_CHECK_NE(val1, val2) CUTILS_CHECK(val1 != val2)
#define CUTILS_CHECK_LE(val1, val2) CUTILS_CHECK(val1 <= val2)
#define CUTILS_CHECK_LT(val1, val2) CUTILS_CHECK(val1 < val2)
#define CUTILS_CHECK_GE(val1, val2) CUTILS_CHECK(val1 >= val2)
#define CUTILS_CHECK_GT(val1, val2) CUTILS_CHECK(val1 >  val2)

#define likely(x) __builtin_expect(!!(x), 1) //x很可能为真
#define unlikely(x) __builtin_expect(!!(x), 0) //x很可能为假
#else
#define CUTILS_CLOG(...)
#define CUTILS_TIMER(func)
#define CUTILS_CHECK_LOG(x, fmt...)
#define CUTILS_CHECK(x)
#define CUTILS_ASSERT(x, fmt...)
#endif // DF_SEGMENT_TEST_ENABLED


#ifdef SUPPORT_TRACE
#define CUTILS_TRACE_BEGIN(name)  ATrace_beginSection(name)
#define CUTILS_TRACE_END()  ATrace_endSection()
#else
#define CUTILS_TRACE_BEGIN(name)
#define CUTILS_TRACE_END()
#endif
#endif //COREUTILS_LOGGER_H
