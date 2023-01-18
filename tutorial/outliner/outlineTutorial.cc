// The "purpose" as it appears in the man page, uncapitalized and a single, short, line.
static const char *purpose = "This tool outlines code segments to functions";

static const char *description =
    "Outlining is the process of replacing a block of consecutive "
    "statements with a function call to a new function containing those statements. "
    "Conceptually, outlining is the inverse of inlining.";

#include <rose.h>                                       // must be first ROSE include
#include <Outliner.hh>                                  // from ROSE

#include <iostream>
#include <vector>
#include <string>

using namespace Rose;                                   // the ROSE team is migrating everything to this namespace

// Switches for this tool. Tools with lots of switches will probably want these to be in some Settings struct mirroring the
// approach used by some analyses that have lots of settings. So we'll do that here too even though it looks funny.
struct Settings {
    bool showOutlinerSettings;                          // should we show the outliner settings instead of running it?

    Settings()
        : showOutlinerSettings(true) {}
} settings;

int
main (int argc, char* argv[])
{
  std::vector<std::string> args(argv, argv+argc);
  Outliner::commandLineProcessing(args);  // this is the old way
  SgProject *proj = frontend(args);

  // Binary analyses know to validate their user-defined settings before they do any analysis, but that's not true (yet) for
  // Outliner, so I've created a function we can call explicitly.
  Outliner::validateSettings();

  if (settings.showOutlinerSettings) {
      std::cout <<"Outliner settings:\n";
      std::cout <<"  enable_debug        = " <<Outliner::enable_debug <<"\n";
      std::cout <<"  preproc_only_       = " <<Outliner::preproc_only_ <<"\n";
      std::cout <<"  useParameterWrapper = " <<Outliner::useParameterWrapper <<"\n";
      std::cout <<"  useStructureWrapper = " <<Outliner::useStructureWrapper <<"\n";
      std::cout <<"  useNewFile          = " <<Outliner::useNewFile <<"\n";
      std::cout <<"  exclude_headers     = " <<Outliner::exclude_headers <<"\n";
      std::cout <<"  enable_classic      = " <<Outliner::enable_classic <<"\n";
      std::cout <<"  temp_variable       = " <<Outliner::temp_variable <<"\n";
      std::cout <<"  use_dlopen          = " <<Outliner::use_dlopen <<"\n";
      std::cout <<"  line                = ";
      BOOST_FOREACH (int &line, Outliner::lines)
          std::cout <<line<<" ";
      std::cout <<"\n";
      std::cout <<"  output_path         = " <<Outliner::output_path <<"\n";
      std::cout <<"  enable_liveness     = " <<Outliner::enable_liveness <<"\n";
  }
  // Ideally, this logging would be part of the Outliner, not every tool that uses it. And the outliner would have its own
  // diagnostic facility that could be turned on and off from the command-line (see --log).
  MLOG_INFO_CXX("tutorial") << "outlining...\n";
  size_t count = Outliner::outlineAll(proj);
  MLOG_INFO_CXX("tutorial") << "outlining processed " << count << " outline directive(s)" <<"\n";
  return backend(proj);
}
