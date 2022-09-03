#ifndef ROSE_SAGESUPPORT_H
#define ROSE_SAGESUPPORT_H

/**
 * \file    sage_support.h
 * \author  Justin Too <too1@llnl.gov>
 * \date    April 4, 2012
 */

/*-----------------------------------------------------------------------------
 *  Dependencies
 *
 *  TODO:
 *  1. Move #includes into .cpp file if they are not needed here.
 *---------------------------------------------------------------------------*/
// tps (01/14/2010) : Switching from rose.h to sage3.
// sage3basic.h should not be included by librose header files, only by .C files per policy. [Robb P. Matzke 2015-01-07]
//#include "sage3basic.h"

#include "rose_paths.h"
#include "astPostProcessing.h"
#include <sys/stat.h>

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "omp_lowering.h"
#endif

#include "attachPreprocessingInfo.h"

#include "sageBuilder.h"

#include "CollectionHelper.h"
#include "IncludeDirective.h"
#include "CompilerOutputParser.h"
#include "IncludingPreprocessingInfosCollector.h"

#ifdef _MSC_VER
//#pragma message ("WARNING: wait.h header file not available in MSVC.")
#else
#include <sys/wait.h>
#endif

#ifdef _MSC_VER
//#pragma message ("WARNING: libgen.h header file not available in MSVC.")
#else
#include <libgen.h>
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#ifdef HAVE_DLADDR
   #include <dlfcn.h>
#endif

// Needed for boost::filesystem::exists(...)
#include "boost/filesystem.hpp"
#include <stdio.h>

// Liao 10/8/2010, refactored OpenMP related code to ompAstConstruction.C
#include "ompAstConstruction.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

namespace Rose {
namespace Frontend {
  int Run(SgProject* project);
  int RunSerial(SgProject* project);
}// ::Rose::Frontend
}

#endif // ROSE_SAGESUPPORT_H

