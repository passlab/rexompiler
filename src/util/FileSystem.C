#include <FileSystem.h>
#include <set>
#include <fstream>
#include <filesystem>
#include <regex>
#include <chrono>
#include <random>
#include <system_error>

namespace Rose {
namespace FileSystem {

const char *tempNamePattern = "rose-%%%%%%%-%%%%%%%";

bool
baseNameMatches::operator()(const Path &path) {
    return std::regex_match(path.filename().string(), re_);
}

bool
isExisting(const Path &path) {
    return std::filesystem::exists(path);
}

bool
isFile(const Path &path) {
    return std::filesystem::is_regular_file(path);
}

bool
isDirectory(const Path &path) {
    return std::filesystem::is_directory(path);
}

bool
isSymbolicLink(const Path &path) {
    return std::filesystem::is_symlink(path);
}

bool
isNotSymbolicLink(const Path &path) {
    return !std::filesystem::is_symlink(path);
}

Path
createTemporaryDirectory() {
    // Generate a unique name based on timestamp and random number
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert time to a string
    std::string timeStr = std::to_string(now_c);

    // Generate a random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 10000);
    std::string randomStr = std::to_string(dis(gen));

    // Combine time and random number into the directory name
    std::string dirNameStr = "temp_" + timeStr + "_" + randomStr;

    std::filesystem::path dirName = std::filesystem::temp_directory_path() / dirNameStr;
    std::filesystem::create_directory(dirName);

    return dirName;
}

Path
makeNormal(const Path &path) {
    std::vector<Path> components;
    for (std::filesystem::path::const_iterator i=path.begin(); i!=path.end(); ++i) {
        if (0 == i->string().compare("..") && !components.empty()) {
            components.pop_back();
        } else if (0 != i->string().compare(".")) {
            components.push_back(*i);
        }
    }
    Path result;
    for (const Path &component: components)
        result /= component;
    return result;
}

Path
makeAbsolute(const Path &path, const Path &root) {
    return makeNormal(path.is_absolute() ? path : absolute(root / path));
}

Path
makeRelative(const Path &path_, const Path &root_) {
    Path path = makeAbsolute(path_);
    Path root = makeAbsolute(root_);

    std::filesystem::path::const_iterator rootIter = root.begin();
    std::filesystem::path::const_iterator pathIter = path.begin();

    // Skip past common prefix
    while (rootIter!=root.end() && pathIter!=path.end() && *rootIter==*pathIter) {
        ++rootIter;
        ++pathIter;
    }

    // Return value must back out of remaining A components
    Path retval;
    while (rootIter!=root.end()) {
        if (*rootIter++ != ".")
            retval /= "..";
    }

    // Append path components
    while (pathIter!=path.end())
        retval /= *pathIter++;
    return retval;
}

std::vector<Path>
findNames(const Path &root) {
    return findNames(root, isExisting);
}

std::vector<Path>
findNamesRecursively(const Path &root) {
    return findNamesRecursively(root, isExisting, isDirectory);
}

// This doesn't make any sense! First, BOOST_COMPILED_WITH_CXX11 is never defined in any version of boost. Second, even if it
// were defined, it would come from boost header files which are always compiled with the same compile as that which is
// compiling this source file. [Robb Matzke 2016-02-17]
//#if (__cplusplus >= 201103L) 
//#if !defined(BOOST_COMPILED_WITH_CXX11)
//   #warning "Compiling ROSE with C++11 mode: BOOST NOT compiled with C++11 support."
//#else
//   #warning "Compiling ROSE with C++11 mode: BOOST WAS compiled with C++11 support."
//#endif
//#endif

void
copyFile(const Path &src, const Path &dst) {
    // Do not use std::filesystem::copy_file in boost 1.56 and earlier because it is not possible to cross link c++11 rose
    // with c++89 boost when using this symbol.  Boost issue #6124 fixed in boost 1.57 and later. Our solution is to use C++
    // stream I/O instead, which should still work on non-POSIX systems (Microsoft) although the exception situations might not
    // be exactly precise as POSIX. Use path::string rather than path::native in order to support Filesystem version 2.
    std::ifstream in(src.string().c_str(), std::ios::binary);
    std::ofstream out(dst.string().c_str(), std::ios::binary);
    out <<in.rdbuf();
    if (in.fail()) {
        //throw std::filesystem::filesystem_error("read failed", src,
        //                                          std::system::error_code(errno, std::system::generic_category()));
    }
    if (out.fail()) {
        //throw std::filesystem::filesystem_error("write failed", dst,
        //                                          std::system::error_code(errno, std::system::generic_category()));
    }
}

// Copies files to dstDir so that their name relative to dstDir is the same as their name relative to root
void
copyFiles(const std::vector<Path> &fileNames, const Path &root, const Path &dstDir) {
    std::set<Path> dirs;
    for (const Path &fileName: fileNames) {
        Path dirName = dstDir / makeRelative(fileName.parent_path(), root);
        if (dirs.insert(dirName).second)
            std::filesystem::create_directories(dirName);
        Path outputName = dirName / fileName.filename();
        copyFile(fileName, outputName);
    }
}

std::vector<Path>
findRoseFilesRecursively(const Path &root) {
    return findNamesRecursively(root, baseNameMatches(std::regex("rose_.*")), isDirectory);
}

// Don't use this if you can help it!
std::string
toString(const Path &path) {
    return path.generic_string();
}

} // namespace
} // namespace
