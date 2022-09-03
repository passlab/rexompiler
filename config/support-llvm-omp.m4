AC_DEFUN([ROSE_WITH_LLVM_OPENMP_LIBRARY],
[
# Check if LLVM OpenMP runtime library is available
# Begin macro ROSE_WITH_LLVM_OPENMP_LIBRARY.
# Inclusion of test for LLVM OpenMP Runtime system and its location.

AC_MSG_CHECKING(for OpenMP using LLVM OpenMP runtime library)
AC_ARG_WITH(llvm_omp_runtime_library,
[  --with-llvm_omp_runtime_library=PATH	Specify the prefix where LLVM OpenMP Runtime System is installed],
,
if test ! "$with_llvm_omp_runtime_library" ; then
   with_llvm_omp_runtime_library=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_llvm_omp_runtime_library = "$with_llvm_omp_runtime_library"])

if test "$with_llvm_omp_runtime_library" = no; then
   # If llvm_omp_runtime_library is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of LLVM OpenMP Runtime Library.])
else
   llvm_omp_runtime_library_path=$with_llvm_omp_runtime_library
   AC_MSG_NOTICE([setup LLVM OpenMP library in ROSE. path = "$llvm_omp_runtime_library_path"])
   AC_DEFINE([USE_ROSE_LLVM_OPENMP_LIBRARY],1,[Controls use of ROSE support for OpenMP Translator targeting LLVM OpenMP RTL.])
   AC_DEFINE_UNQUOTED([LLVM_OPENMP_LIB_PATH],"$llvm_omp_runtime_library_path",[Location (unquoted) of the LLVM OpenMP runtime library.])
fi

AC_DEFINE_UNQUOTED([ROSE_INSTALLATION_PATH],"$prefix",[Location (unquoted) of the top directory path to which ROSE is installed.])

AC_SUBST(llvm_omp_runtime_library_path)

# End macro ROSE_WITH_LLVM_OPENMP_LIBRARY.
AM_CONDITIONAL(WITH_LLVM_OPENMP_LIB,test ! "$with_llvm_omp_runtime_library" = no)

]
)
