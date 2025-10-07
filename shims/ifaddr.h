#ifndef SHIM_IFADDRS_H_INCLUDED
#define SHIM_IFADDRS_H_INCLUDED
#include <android/api-level.h>
#if __ANDROID_API__ >= 24
    #include "ifaddrs_libc.h"
#else
    #include "ifaddrs_compat.h"
#endif
#endif // SHIM_IFADDRS_H_INCLUDED