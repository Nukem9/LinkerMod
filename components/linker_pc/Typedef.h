#pragma once

typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;

typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef void *voidptr_t;

#ifdef _WIN64
typedef unsigned long long sizeptr_t;
#else
typedef __w64 unsigned long sizeptr_t;
#endif

typedef sizeptr_t addrptr_t;