#ifndef ROSE_SAGESUPPORT_CMDLINE_H
#define ROSE_SAGESUPPORT_CMDLINE_H

/**
 * \file    cmdline.h
 * \author  Justin Too <too1@llnl.gov>
 * \date    April 4, 2012
 */

/*-----------------------------------------------------------------------------
 *  Dependencies
 *---------------------------------------------------------------------------*/
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage_support.h"

namespace Rose {
namespace Cmdline {
  /** Constants to be used with CommandlineProcessing::isOptionWithParameter
   *  to specify the removeOption argument.
   */
  enum {
    KEEP_OPTION_IN_ARGV = 0,      ///< Don't remove the CLI option from the input argv
    REMOVE_OPTION_FROM_ARGV = 1   ///< Remove the CLI option from the input argv
  };

  extern ROSE_DLL_API int verbose;

  void
  makeSysIncludeList(const Rose_STL_Container<string> &dirs, Rose_STL_Container<string> &result, bool using_nostdinc_option = false);

  //! Convert `-I <path>` to `-I<path>`
  //
  // TOO1 (11/21/2013): Current CLI handling assumes that there is no space
  // between the -I option and its <path> option. That is,
  //
  //      +----------+------------+
  //      | Valid    |  -I<path>  |
  //      +----------+------------+
  //      | Invalid  |  -I <path> |
  //      +----------+------------+
  //
  // Note: Path argument is validated for existence.
  //
  std::vector<std::string>
  NormalizeIncludePathOptions (std::vector<std::string>& argv);

  /** Removes "-rose:" options, or transforms them into their associated
   *  compiler options.
   *
   *  For example,
   *
   *      -rose:java:classpath "/some/class/path"
   *
   *      becomes
   *
   *      -classpath "/some/class/path"
   *
   *  Whereas, this ROSE-only option is completely removed:
   *
   *      -rose:verose 3
   */
  void
  StripRoseOptions (std::vector<std::string>& argv);

  void
  ProcessKeepGoing (SgProject* project, std::vector<std::string>& argv);

  namespace Unparser {
    static const std::string option_prefix = "-rose:unparser:";

    /** @returns true if the Unparser option requires a user-specified argument.
     */
    bool
    OptionRequiresArgument (const std::string& option);

    void
    StripRoseOptions (std::vector<std::string>& argv);

    /** Process all Unparser-specific commandline options, i.e. -rose:unparser.
     */
    void
    Process (SgProject* project, std::vector<std::string>& argv);

    void
    ProcessClobberInputFile (SgProject* project, std::vector<std::string>& argv);
  } // namespace ::Rose::Cmdline::Unparser

  namespace Fortran {
    static const std::string option_prefix = "-rose:fortran:";

    /** @returns true if the Java option requires a user-specified argument.
     */
    bool
    OptionRequiresArgument (const std::string& option);

    void
    StripRoseOptions (std::vector<std::string>& argv);

    /** Process all Fortran commandline options.
     */
    void
    Process (SgProject* project, std::vector<std::string>& argv);

    // -rose:fortran
    void
    ProcessFortranOnly (SgProject* project, std::vector<std::string>& argv);

    /** Targeted for src/frontend/OpenFortranParser_SAGE_Connection/jserver.C,
     */
    namespace Ofp {
      extern ROSE_DLL_API std::list<std::string> jvm_options;

      ROSE_DLL_API void
      StripRoseOptions (std::vector<std::string>& argv);

      ROSE_DLL_API std::string
      GetRoseClasspath();

      ROSE_DLL_API void
      Process (SgProject* project, std::vector<std::string>& argv);

      /** -rose:fortran:ofp:jvm_options
       */
      ROSE_DLL_API void
      ProcessJvmOptions (SgProject* project, std::vector<std::string>& argv);

      /** -rose:fortran:ofp:enable_remote_debugging
       *  Enable remote debugging of the Java Virtual Machine (JVM).
       */
      ROSE_DLL_API void
      ProcessEnableRemoteDebugging (SgProject* project, std::vector<std::string>& argv);
    } // namespace Rose::Cmdline::Fortran::OpenFortranParser
  } // namespace Rose::Cmdline::Fortran

  namespace Gnu {
    /** @returns true if the GNU option requires a user-specified argument.
     */
    bool
    OptionRequiresArgument (const std::string& option);

    void
    Process (SgProject* project, std::vector<std::string>& argv);

    // --param
    void
    ProcessParam (SgProject* project, std::vector<std::string>& argv);
  } // namespace Rose::Cmdline::Gnu
} // namespace Rose::Cmdline
} // namespace Rose
#endif // ROSE_SAGESUPPORT_CMDLINE_H

