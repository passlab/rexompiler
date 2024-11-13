#ifndef ROSE_StringUtility_Replace_H
#define ROSE_StringUtility_Replace_H

#include <string>
#include <rosedll.h>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for replacing a pattern with another one in a string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Replace a pattern with another one in a string
 *
 *  @{ */
ROSE_UTIL_API std::string replaceAllCopy(const std::string& input, const std::string& search, const std::string & replacedWith);

} // namespace
} // namespace

#endif
