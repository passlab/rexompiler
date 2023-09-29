# Summarizes the _important_ configuration results for the user.

########################################################################################################################
#                    DO NOT SUMMARIZE VARIABLES THAT THE __USER__ DOESN'T CARE ABOUT!
#
# For instance, if the user didn't enable C++ as a analyzable language, then don't tell him what the backend C++
# compiler is.
#
# Also, do not add things here that are just for your own debugging. Debugging output should be close to where the
# detection is happening, and should use the AC_MSG_NOTICE macro, not "echo". This file is for summaries only!
#
########################################################################################################################

AC_DEFUN([ROSE_SUMMARY_HEADING],[
    echo "  $1"
])

AC_DEFUN([ROSE_SUPPORT_SUMMARY],[
# Given the C/C++/Fortran compiler command-line, create output variables such as HOST_CXX_VENDOR, HOST_CXX_VERSION, and HOST_CXX_LANGUAGE
# that contain the vendor (gnu, llvm, or intel), the version (as defined by CPP macros, not the --version output), and
# the language dialect (c++17, gnu++11, etc.).
ROSE_COMPILER_FEATURES([c], [$CC $CFLAGS], [HOST_CC_])
ROSE_COMPILER_FEATURES([c++], [$CXX $CPPFLAGS $CXXFLAGS], [HOST_CXX_])
ROSE_COMPILER_FEATURES([fortran], [$FC $FCLAGS], [HOST_FC_])

    echo "${T_BM}Configuration summary:${T_NM}"
    #--------------------------------------------------------------------------------
    # ROSE conflates the compilers used to compile ROSE with the compiler frontend
    # compiler used for C++ analysis, thus the variables printed for this section
    # of the summary are named "FRONTEND".
    ROSE_SUMMARY_HEADING([Compilers for ROSE source code])
    echo "    C compiler                       ${CC}"
    echo "        CFLAGS                       ${CFLAGS}"
    echo "        LDFLAGS                      ${LDFLAGS}"
    echo "    C vendor                         ${HOST_CC_VENDOR:-unknown}"
    echo "    C version                        ${HOST_CC_VERSION:-unknown}"
    echo "    C language                       ${HOST_CC_LANGUAGE:-unknown}"
    echo "    C++ compiler                     ${CXX}"
    echo "        CXXFLAGS                     ${CXXFLAGS}"
    echo "        LDFLAGS                      ${LDFLAGS}"
    echo "    C++ vendor                       ${HOST_CXX_VENDOR:-unknown}"
    echo "    C++ version                      ${HOST_CXX_VERSION:-unknown}"
    echo "    C++ language                     ${HOST_CXX_LANGUAGE:-unknown}"
    echo "    Fortran compiler                 ${FC}"
    echo "        FCFLAGS                      ${FCFLAGS}"
    echo "        LDFLAGS                      ${LDFLAGS}"
    echo "    Fortran vendor                   ${HOST_FC_VENDOR:-unknown}"
    echo "    Fortran version                  ${HOST_FC_VERSION:-unknown}"
    echo "    Fortran language                 ${HOST_FC_LANGUAGE:-unknown}"
    echo "    Bison version                    ${bison_version:-unknown}"

    #--------------------------------------------------------------------------------
    ROSE_SUMMARY_HEADING([Languages supported for analysis])
    echo "    C                                ${support_c_frontend:-no}"
    echo "    C++                              ${support_cxx_frontend:-no}"
    echo "    C preprocessor                   ${support_cpp_frontend:-no}"
    echo "    Cuda                             ${support_cuda_frontend:-no}"
    echo "    Fortran                          ${support_fortran_frontend:-no}"
    echo "    OpenCL                           ${support_opencl_frontend:-no}"

    #--------------------------------------------------------------------------------
    if test -n "$support_cxx_frontend" -o -n "$verbose"; then
        ROSE_SUMMARY_HEADING([C/C++ analysis support])
	echo "    frontend C++ parser              EDG-${edg_major_version_number}.${edg_minor_version_number}"
	echo "    compiling EDG from source        $has_edg_source"
	echo "    generating EDG binary tarball    $binary_edg_tarball_enabled"
	echo "    backend C++ compiler             ${BACKEND_CXX_COMPILER_COMMAND:-none}"
	echo "    C++ vendor                       ${BACKEND_CXX_COMPILER_VENDOR:-unknown}"
	echo "    C++ version                      ${BACKEND_CXX_VERSION_TRIPLET:-unknown}"
	echo "    backend C compiler               ${BACKEND_C_COMPILER:-none}"
    fi

    #--------------------------------------------------------------------------------
    if test -n "$support_fortran_frontend" -o -n "$verbose"; then
        ROSE_SUMMARY_HEADING([Fortran analysis support])
	echo "    fortran compiler                 ${BACKEND_FORTRAN_COMPILER:-none}"
    fi

    #--------------------------------------------------------------------------------
    if test -n "$DOXYGEN" -o -n "$verbose"; then
        ROSE_SUMMARY_HEADING([Doxygen documentation generator])
	echo "    executable                       ${DOXYGEN:-none}"
	echo "    version                          ${DOXYGEN_VERSION:-unknown}"
        echo "    indexer                          ${DOXYINDEXER:-none}"
        echo "    stubs for undocumented entities  ${DOXYGEN_EXTRACT_ALL:-no}"
        echo "    document private entities        ${DOXYGEN_EXTRACT_PRIVATE:-no}"
        echo "    document static entities         ${DOXYGEN_EXTRACT_STATIC:-no}"
        echo "    document internal entities       ${DOXYGEN_INTERNAL_DOCS:-no}"
        echo "    hide undocumented members        ${DOXYGEN_HIDE_UNDOC_MEMBERS:-no}"
        echo "    hide undocumented classes        ${DOXYGEN_HIDE_UNDOC_CLASSES:-no}"
        echo "    hide friend compounds            ${DOXYGEN_HIDE_FRIEND_COMPOUNDS:-no}"
    fi
])
