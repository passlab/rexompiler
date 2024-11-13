#ifndef ROSE_isnan_H
#define ROSE_isnan_H

#include <cmath>

// In case anyone included a C header file, since isnan is a macro in C
#undef isnan

template<typename T>
bool rose_isnan(T x) {
    return std::isnan(x);
}

#endif
