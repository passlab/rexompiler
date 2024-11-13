// This file defines various C preprocessor macros that describe what ROSE features are enabled in this configuration. The
// advantage of doing this in a C header instead of calculating it in the configuration system and storing a result is that
// this same logic can then be used across all configuration and build systems.
//
// All ROSE feature macros start with the letters "ROSE_ENABLED_" followed by the name of the feature. These macros are defined
// if the feature is enabled, and not defined (but also not #undef) if the feature is not automatically enabled. By not
// explicitly undefining the macro, we make it possible for developers to enabled features from the C++ compiler command-line
// that would not normally be enabled.

#ifndef ROSE_FeatureTests_H
#define ROSE_FeatureTests_H

// DO NOT INCLUDE LARGE HEADERS HERE! These headers should generally be only C preprocessor directives, not any substantial
// amount of C++ code. This means no sage3basic.h or rose.h, among others. This <featureTests.h> file is meant to be as small
// and fast as possible because its purpose is to be able to quickly compile (by skipping over) source code that's not
// necessary in a particular ROSE configuration.
#include <rosePublicConfig.h>

#if defined(_MSC_VER)
    // Microsoft Visual C++ Compiler erroneously advertises that it's a C++98 compiler. The "fix" for Visual Studio 2017
    // version 15.7 Preview 3 is not really a fix -- the user has to explicitly opt-in on the command-line that __cplusplus
    // should be set to the correct value. Therefore, we just avoid checking this compiler and let the user try to figure out
    // that they're using the wrong language based on the compiler's error messages.
    //   See https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-160
    //   See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#elif __cplusplus < 201402L
    #error "ROSE requires a C++14 or later compiler"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configuration fixes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Source code analysis in general.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Defined if any source-level analysis is enabled.
#if !defined(ROSE_ENABLE_SOURCE_ANALYSIS) && ( \
    defined(ROSE_BUILD_CPP_LANGUAGE_SUPPORT) ||   \
    defined(ROSE_BUILD_CUDA_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_CXX_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_C_LANGAUGE_SUPPORT) || \
    defined(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT))
#define ROSE_ENABLE_SOURCE_ANALYSIS
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C/C++ analysis features
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
