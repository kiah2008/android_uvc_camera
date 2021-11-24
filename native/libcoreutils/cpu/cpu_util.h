//
// Created by zhaokai on 2020/6/28.
//

#ifndef COREUTILS_CPU_UTIL_H
#define COREUTILS_CPU_UTIL_H

#include <set>

namespace cutils{
    // Returns the number of CPU cores. Compatible with Android.
    int NumCPUCores();
// Returns a set of inferred CPU ids of lower cores.
    std::set<int> InferLowerCoreIds();
// Returns a set of inferred CPU ids of higher cores.
    std::set<int> InferHigherCoreIds();
}


#endif //COREUTILS_CPU_UTIL_H
