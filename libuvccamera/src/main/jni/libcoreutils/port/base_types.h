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
    FSP_STATUS_OK,
    FSP_STATUS_INTERNAL_ERROR,
    FSP_STATUS_INVALID,
    FSP_STATUS_INVALID_OPERATION,
    FSP_STATUS_UNKNOWN,
    FSP_STATUS_FAILURE,
    FSP_STATUS_CANCEL,
    FSP_DEADLINE_EXCEEDED,
    FSP_STATUS_ALREADY_EXISTS,
    FSP_STATUS_PERMISSION_DENNIED,
    FSP_STATUS_RESOURCE_EXHAUSTED,
    FSP_STATUS_FAIL_PRECONDITION,
    FSP_STATUS_ABORT,
    FSP_STATUS_OUT_OF_RANGE,
    FSP_STATUS_UNIMPLEMENTATION,
    FSP_STATUS_UNAVAILABLE,
    FSP_STATUS_DATALOSS,
    FSP_STATUS_NOT_FOUND,
    FSP_STATUS_STOP,
    FSP_STATUS_NO_PERSON,
    FSP_STATUS_TOO_FAR,
    FSP_STATUS_TOO_MANY_PERSON,
    FSP_STATUS_LOADING,
    FSP_STATUS_GL_COMPILING,
    FSP_STATUS_MODEL_LOADING,
} FSP_STATUS;

#endif //COREUTILS_BASE_TYPES_H
