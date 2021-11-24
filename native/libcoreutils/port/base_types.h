//
// Created by zhaokai on 2020/6/18.
//

#ifndef COREUTILS_BASE_TYPES_H
#define COREUTILS_BASE_TYPES_H

#include <sys/types.h>
#include <cstdint>

typedef signed char schar;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef u_int8_t uint8;
typedef u_int16_t uint16;
typedef u_int32_t uint32;
typedef u_int64_t uint64;

typedef signed int char32;
typedef unsigned long uword_t;

#define GG_LONGLONG(x) x##LL
#define GG_ULONGLONG(x) x##ULL
#define GG_LL_FORMAT "ll"  // As in "%lld". Note that "q" is poor form also.
#define GG_LL_FORMAT_W L"ll"

const uint8 kuint8max{0xFF};
const uint16 kuint16max{0xFFFF};
const uint32 kuint32max{0xFFFFFFFF};
const uint64 kuint64max{GG_ULONGLONG(0xFFFFFFFFFFFFFFFF)};
const int8 kint8min{~0x7F};
const int8 kint8max{0x7F};
const int16 kint16min{~0x7FFF};
const int16 kint16max{0x7FFF};
const int32 kint32min{~0x7FFFFFFF};
const int32 kint32max{0x7FFFFFFF};
const int64 kint64min{GG_LONGLONG(~0x7FFFFFFFFFFFFFFF)};
const int64 kint64max{GG_LONGLONG(0x7FFFFFFFFFFFFFFF)};

typedef enum {
    CUTILS_STATUS_OK,
    CUTILS_STATUS_INTERNAL_ERROR,
    CUTILS_STATUS_INVALID,
    CUTILS_STATUS_INVALID_OPERATION,
    CUTILS_STATUS_UNKNOWN,
    CUTILS_STATUS_FAILURE,
    CUTILS_STATUS_CANCEL,
    CUTILS_DEADLINE_EXCEEDED,
    CUTILS_STATUS_ALREADY_EXISTS,
    CUTILS_STATUS_PERMISSION_DENNIED,
    CUTILS_STATUS_RESOURCE_EXHAUSTED,
    CUTILS_STATUS_FAIL_PRECONDITION,
    CUTILS_STATUS_ABORT,
    CUTILS_STATUS_OUT_OF_RANGE,
    CUTILS_STATUS_UNIMPLEMENTATION,
    CUTILS_STATUS_UNAVAILABLE,
    CUTILS_STATUS_DATALOSS,
    CUTILS_STATUS_NOT_FOUND,
    CUTILS_STATUS_STOP,
    CUTILS_STATUS_NO_PERSON,
    CUTILS_STATUS_TOO_FAR,
    CUTILS_STATUS_TOO_MANY_PERSON,
    CUTILS_STATUS_LOADING,
    CUTILS_STATUS_GL_COMPILING,
    CUTILS_STATUS_MODEL_LOADING,
} CUTILS_STATUS;

typedef long Timestamp;

#endif //COREUTILS_BASE_TYPES_H
