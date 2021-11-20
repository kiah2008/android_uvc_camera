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


#define FSP_LOG(fmt, ...)    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ## __VA_ARGS__)
#define FSP_LOGD(fmt, ...)   __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ## __VA_ARGS__)
#define FSP_LOGW(fmt, ...)   __android_log_print(ANDROID_LOG_WARN, LOG_TAG, fmt, ## __VA_ARGS__)
#define FSP_LOGE(fmt, ...)   __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
#define FSP_LOGF(fmt, ...)   __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, fmt, ## __VA_ARGS__)


#define LOCAL_VARIABLE(x, y) _l##x(y)
#define FSP_TIMER(func) ScopedTimer LOCAL_VARIABLE(__LINE__, func)

#define FSP_CHECK_LOG(x, fmt...) if(__builtin_expect(!(x), 0)) {FSP_LOGW(fmt); }
#define FSP_RETURN_IF_ERROR(x, fmt...) if(__builtin_expect(!(x), 0)) {FSP_LOG(fmt); return; }
#define FSP_RETURN_STATUS_IF_ERROR(x, err) if(__builtin_expect(!(x), 0)) {return err; }
#define FSP_CHECK_RETURN_STATUS(x, fail_code, fmt...) if(__builtin_expect(!(x), 0)) {FSP_LOG(fmt); return fail_code;}
#define FSP_CHECK_STATUS_RET(x, fmt...) if(__builtin_expect((x.ok())==FSP_STATUS_OK, 0)) {FSP_LOG(fmt); return x;}
#define FSP_CHECK(x) if(__builtin_expect(!(x), 0)) {FSP_LOGE("Assert Failed %s at %s:%d", #x, __FUNCTION__, __LINE__); assert(x);}
#define FSP_ASSERT_LOG(x, fmt...) if(__builtin_expect(!(x), 0)) {FSP_LOGE(fmt); assert(x);}
#define FSP_ASSERT_OK(x) assert(x == FSP_STATUS_OK);

#define FSP_CHECK_EQ(val1, val2) FSP_CHECK(val1 == val2)
#define FSP_CHECK_NE(val1, val2) FSP_CHECK(val1 != val2)
#define FSP_CHECK_LE(val1, val2) FSP_CHECK(val1 <= val2)
#define FSP_CHECK_LT(val1, val2) FSP_CHECK(val1 < val2)
#define FSP_CHECK_GE(val1, val2) FSP_CHECK(val1 >= val2)
#define FSP_CHECK_GT(val1, val2) FSP_CHECK(val1 >  val2)

#define likely(x) __builtin_expect(!!(x), 1) //x很可能为真
#define unlikely(x) __builtin_expect(!!(x), 0) //x很可能为假
#else
#define FSP_FSP_LOG(...)
#define FSP_TIMER(func)
#define FSP_CHECK_LOG(x, fmt...)
#define FSP_CHECK(x)
#define FSP_ASSERT(x, fmt...)
#endif // DF_SEGMENT_TEST_ENABLED

#endif //COREUTILS_LOGGER_H
