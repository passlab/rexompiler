#define CONFIG_NAME "rose-config.cfg"

static const char *purpose = "show ROSE configuration arguments";
static const char *description =
  "This command displays various configuration settings that are useful in user makefiles when compiling or linking a "
  "program that uses ROSE.  It does this by reading a configuration file named " CONFIG_NAME " from the directory that "
  "holdes the ROSE libraries, or from the file specified with the @s{config} switch.  This file contains blank lines, "
  "comments starting with '#' as the first non-white-space character of a line, or lines of the form \"KEY = VALUE\" "
  "where white-space around either end of the VALUE is stripped.\n\n"

  "The tool should be invoked with one positional argument: the name of the KEY whose value is to be reported. The "
  "following keys must be present in the " CONFIG_NAME " file:"

  // If you modify this list, be sure to update requiredKeys()
  "@named{cc}{Displays the name of the C compiler.}"

  "@named{cxx}{Displays the name of the C++ compiler.}"

  "@named{cppflags}{Shows the switches that should be passed to the C preprocessor as part of compile commands.}"

  "@named{cflags}{Shows the C compiler switches, excluding preprocessor switches, that should be used when compiling a "
  "program that uses ROSE.}"

  "@named{cxxflags}{Shows the C++ compiler switches, excluding preprocessor switches, that should be used when compiling a "
  "program that uses ROSE.}"

  "@named{ldflags}{Shows the compiler switches that should be used when linking a program that uses the ROSE library.}"

  "@named{libdirs}{Shows a colon-separated list of library directories. These are the directories that might contain shared "
  "libraries.}"

  "@named{prefix}{ROSE installation prefix. This is the name of the directory that includes \"lib\" and \"include\" "
  "subdirectories (among others) where the ROSE library and its headers are installed.}";

#include <rose.h>                                       // POLICY_OK -- this is not a ROSE library source file
#include <rose_getline.h>

#include <boost/foreach.hpp>
#include <regex>
#include <map>
#include <string>
#include <vector>

#include <util/StringUtility/Trim.h>

using namespace Rose;

typedef std::map<std::string, std::string> Configuration;

struct Settings {
  std::string searchDirs;
  std::filesystem::path configFile;

  // When compiling this program, LIBDIR C preprocessor symbol should be the name of the installation path for libraries.
  Settings()
    : searchDirs(LIBDIR) {}
};

static const std::vector<std::string>&
requiredKeys() {
  static std::vector<std::string> required;
  if (required.empty()) {
    // These should all be documented in the --help output
    required.push_back("ROSE_CC");
    required.push_back("ROSE_CXX");
    required.push_back("ROSE_CPPFLAGS");
    required.push_back("ROSE_CFLAGS");
    required.push_back("ROSE_CXXFLAGS");
    required.push_back("ROSE_LDFLAGS");
    required.push_back("ROSE_LIBDIRS");
    required.push_back("ROSE_PREFIX");
  }
  return required;
}

/** 
 * toOldKeyFormat
 *
 * /brief Converts a Key to the old format
 *
 * JL (03/24/2018) I changed the default keys to be ROSE_CC,
 * ROSE_PREFIX, etc.  They used to be cc, prefix, etc.  This was so
 * the rose-config.cfg would not conflict with similar keys when
 * included in a Makefile.
 * To keep the rose-config binary compatible with old code, we now
 * support both on the command line.  This function converts the 
 * new format to the old format.   
 *
 **/
std::string toOldKeyFormat(std::string& key) 
{
    std::string oldFormat;
    std::transform(key.begin()+5, key.end(), std::back_inserter(oldFormat), ::tolower);
    return oldFormat;
    
}

// Read a specific configuration file
static Configuration
readConfigFile(const std::filesystem::path &configName) {
  struct Resources {
    FILE *file;
    char *line;
    size_t linesz;

    Resources()
      : file(NULL), line(NULL), linesz(0) {}

    ~Resources() {
      if (file)
        fclose(file);
      if (line)
        free(line);
    }
  } r;

  Configuration retval;

  if (NULL == (r.file = fopen(configName.string().c_str(), "r"))) {
    MLOG_FATAL_CXX("rose-config") <<strerror(errno) <<": \"" <<configName <<"\"\n";
    exit(1);
  }

  std::regex keyRe("[a-zA-Z][a-zA-Z_0-9]*");
  size_t lineNumber = 0;
  while (ssize_t nchars = rose_getline(&r.line, &r.linesz, r.file)) {
    ++lineNumber;
    if (nchars < 0) {
      if (errno) {
    	MLOG_FATAL_CXX("rose-config") <<configName <<":" <<lineNumber <<": " <<strerror(errno) <<"\n";
        exit(1);
      }
      break;                                      // EOF
    }

    std::string s = r.line;
    s = trim(s);
    if (s.empty() || '#' == s[0])
      continue;

    // Parse the "key=value" line
    size_t equal = s.find('=');
    std::string key = s.substr(0, equal);
    key = trim(key);
    std::string value = equal == std::string::npos ? std::string() : s.substr(equal+1);
    value = trim(value);
    if (equal == std::string::npos || !std::regex_match(key, keyRe)) {
      MLOG_FATAL_CXX("rose-config")  <<configName <<":" <<lineNumber <<": syntax error: expected KEY = VALUE\n";
      exit(1);
    }

    // Save the key and value for returning later
    if (!retval.insert(std::make_pair(key, value)).second) {
      MLOG_FATAL_CXX("rose-config")  <<configName <<":" <<lineNumber <<": duplicate key \"" <<StringUtility::cEscape(key) <<"\"\n";
      exit(1);
    }
    
    //Add the old format key to the database as well
    if (!retval.insert(std::make_pair(toOldKeyFormat(key), value)).second) {
      MLOG_FATAL_CXX("rose-config") <<configName <<":" <<lineNumber <<": duplicate key \"" <<StringUtility::cEscape(key) <<"\"\n";
      exit(1);
    }


  }

  bool hadError = false;
  BOOST_FOREACH (const std::string &key, requiredKeys()) {
    if (retval.find(key) == retval.end()) {
      MLOG_FATAL_CXX("rose-config") <<configName <<":" <<lineNumber <<": required key \"" <<key <<"\" is not defined\n";
      hadError = true;
    }
  }
  if (hadError)
    exit(1);
            
  return retval;
}

// Read the first configuration file we can find.
static Configuration
readConfigFile(const Settings &settings) {
  if (!settings.configFile.empty())
    return readConfigFile(settings.configFile);

  std::vector<std::string> dirs;
  //boost::split(dirs, settings.searchDirs, boost::is_any_of(":;"));
  dirs = Rose::StringUtility::split(settings.searchDirs, ":;");
  BOOST_FOREACH (const std::string &dir, dirs) {
    std::filesystem::path configFile = std::filesystem::path(dir) / CONFIG_NAME;
    if (std::filesystem::exists(configFile))
      return readConfigFile(configFile);
  }

  MLOG_FATAL_CXX("rose-config") <<"cannot find file \"" <<StringUtility::cEscape(CONFIG_NAME) <<"\"\n";
  MLOG_FATAL_CXX("rose-config")  <<"searched in these directories:\n";
  BOOST_FOREACH (const std::string &dir, dirs)
    MLOG_FATAL_CXX("rose-config") <<"  \"" <<StringUtility::cEscape(dir) <<"\"\n";
  exit(1);
}

/**
 * makeLibrarySearchPaths takes a space seperated list of directories, and returns
 * a colon seperated list of directories.
 * As part of this, quoted strings are unquoted.
 **/
static std::string
makeLibrarySearchPaths(const std::string &str) {
  // Split string into space-separated arguments in a rather simplistic way.
  std::vector<std::string> args; //A list of split out strings (from str) 
  {
    char quoted = '\0';
    std::string arg;        //One paricular string being split out
    const char *s = str.c_str();  //pointer into the current string being split out
    for (/*void*/; s && *s; ++s) { //While we haven't hit the end of str
      if ('\\'==*s && s[1]) {    //If we see an escaped backslash, just skip past it.
        arg += *++s;
      } else if ('\''==*s || '"'==*s) { //If we see a quote  
        if (!quoted) {                  //If we aren't in a quoted string, save the quote, but it doesn't go in arg
          quoted = *s;
        } else if (quoted == *s) {      //If we we're in a quoted string, and the quotes match, that's the end of the quoted section.  Clear the quote
          quoted = '\0';
        } else {                        //We found a different quote than the string we're in, so just save it arg like any other char
          arg += *s;
        }
      } else if (isspace(*s)) {  //we find a space 
        if (quoted) {            //in a quoted string, save as a normal characted
          arg += *s;
        } else if (!arg.empty()) {  //NOT in a quote, this must be an argument seperator, so save the old arg, and start a new one 
          args.push_back(arg);
          arg = "";
        }
      } else {                   //Normal case, just copy the character to arg.  
        arg += *s;
      }
    }

    if(arg != "") {  //Get that last argument
      args.push_back(arg);
      arg = "";
    }
    ASSERT_require2(quoted=='\0', "mismatched quotes");
  }

  //Turn them all into a colon-seperated string
  std::string retval;
  for (size_t i=0; i<args.size(); ++i) {
    retval += (retval.empty()?"":":") + args[i];
  }
  return retval;
}

// Escape special characters for including in a makefile string.
// FIXME[Robb P. Matzke 2015-04-14]: for now, just use C-style escaping, which might be good enough.
static std::string
makefileEscape(const std::string &s) {
  return StringUtility::cEscape(s);
}

int
main(int argc, char *argv[]) {
  Settings settings;
  std::string key;
  if (argc == 2) { //--help or <key>
	  std::string helpflag ("--help");
	  std::string versionflag ("--version");
	  if (helpflag.compare(argv[1]) == 0) {
		  std::cout << purpose << "\n" << description << "\n";
		  exit(0);
	  } else if (versionflag.compare(argv[1]) == 0){
		  std::cout << version_message() << "\n";
		  exit(0);
	  } else {
	    MLOG_FATAL_C("rose-config", "incorrect usage; see --help\n");
	    exit(1);
	  }
  } else if (argc == 4) { //--config <file> <key>
	  std::string configflag ("--config");
	  if (configflag.compare(argv[1]) == 0) {
		  settings.configFile = std::filesystem::path(argv[2]);
		  key = std::string(argv[3]);
	  } else {
	    MLOG_FATAL_C("rose-config", "incorrect usage; see --help\n");
	    exit(1);
	  }
  } else { //argc == 1 || argc > 4
    MLOG_FATAL_C("rose-config", "incorrect usage; see --help\n");
    exit(1);
  }
  Configuration config = readConfigFile(settings);

  // Print the value
  Configuration::const_iterator found = config.find(key);
  if (found != config.end()) {
    if (key == "libdirs") { //Special case for libdirs, which is stored space seperated in the file, but returns as colon seperated
      std::cout <<makefileEscape(makeLibrarySearchPaths(found->second)) <<"\n";
    } else {
      std::cout <<makefileEscape(found->second) <<"\n";
    }
    exit(0);
  }

  // Errors
  MLOG_FATAL_CXX("rose-config") <<"unknown key \"" <<StringUtility::cEscape(key) <<"\"\n";
  exit(1);
}
