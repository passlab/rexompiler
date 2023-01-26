#include <rose.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <map>
#include <pair>
#include <unistd.h>                                     // execvp

using namespace Rose;

int
main(int argc, char *argv[]) {
    typedef std::vector<std::string> Strings;

    // Usage:
    //    translator --command=CMD NAMES [-- SWITCHES...]         -- runs: CMD SWITCHES NAMES
    //    translator NAMES -- SWITCHES                        -- runs ROSE on SWITCHES NAMES
    // NAMES are always de-escaped and created

    std::string cmd;
    //argv[1] has to start either --command= (10 char) or NAMES
    std::vector<std::string> args(argv+1, argv+argc-1);
    if (args[0].compare(0, 10, "--command=") == 0) { //10 characters for --command=
    	cmd = args[0].substr(11);
    	args.erase(args.begin());
    }

    //NOTE: command line processing was changed and Sawyer-based cmd processing is removed
    //However, this fix has not yet tested.

    // Expand the "+SOMETHING+" escapes in all arguments.
    typedef std::map<std::string, std::string> Translations;
    Translations map;
    map.insert(std::pair<std::string, std::string>("+PLUS+",        "+"));
    map.insert(std::pair<std::string, std::string>("+SPACE+",       " "));
    map.insert(std::pair<std::string, std::string>("+DOLLAR+",      "$"));
    map.insert(std::pair<std::string, std::string>("+STAR+",        "*"));
    map.insert(std::pair<std::string, std::string>("+HASH+",        "#"));
    map.insert(std::pair<std::string, std::string>("+SQUOTE+",      "'"));
    map.insert(std::pair<std::string, std::string>("+DQUOTE+",      "\""));
    map.insert(std::pair<std::string, std::string>("+DOT+",         "."));
    map.insert(std::pair<std::string, std::string>("+SLASH+",       "/"));
    map.insert(std::pair<std::string, std::string>("+BSLASH+",      "\\"));
    for (Strings::iterator arg = args.begin(); arg != args.end(); ++arg) {
        std::string translated;
        for (size_t i=0; i<arg->size(); ++i) {
            size_t endPlus;
            if ((*arg)[i] == '+' && (endPlus=arg->find('+', i+1)) != std::string::npos) {
                std::string token = arg->substr(i, endPlus+1-i);
                translated += map[token];
                i = endPlus;
            } else {
                translated += (*arg)[i];
            }
        }
        *arg = translated;
    }

    // If there's a '--' argument, then create a stub C source file for each argument before the '--', move them all to the
    // end, and remove the '--'. I.e., if the arguments are "a b -- c d" then create files "a" and "b" and then rearrange the
    // arguments to read "c d a b".  If there's no "--", then create source files for all the arguments but don't rearrange
    // anything.
    Strings::iterator doubleHyphen = std::find(args.begin(), args.end(), "--");
    for (Strings::iterator arg = args.begin(); arg != doubleHyphen; ++arg) {
        std::ofstream of(arg->c_str());
        ASSERT_require(!of.bad());
        of <<"int main() {}\n";
    }
    if (doubleHyphen != args.end()) {
        Strings tmp(args.begin(), doubleHyphen);
        tmp.insert(tmp.begin(), ++doubleHyphen, args.end());
        args = tmp;
    }

    // Either the ROSE translator or some other command
    if (cmd.empty()) {
        std::cout <<"translator args are:";
        for (const std::string &arg : args)
            std::cout <<" \"" <<StringUtility::cEscape(arg) <<"\"";
        std::cout <<"\n";
        
        args.insert(args.begin(), argv[0]);
        SgProject *project = frontend(args);
        ASSERT_not_null(project);
        exit(backend(project));
    }
    char const** newArgv = new char const*[args.size()+2];
    newArgv[0] = cmd.c_str();
    for (size_t i=0; i<args.size(); ++i)
        newArgv[i+1] = args[i].c_str();
    newArgv[args.size()+1] = NULL;
    execvp(newArgv[0], (char*const*)newArgv);
    ASSERT_not_reachable("exec failed");
}
