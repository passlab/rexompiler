#ifndef ROSE_StringUtility_Replace_H
#define ROSE_StringUtility_Replace_H

#include <string>
#include <rosedll.h>
#include "boost/algorithm/string/replace.hpp"


namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for replacing a pattern with another one in a string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Replace a pattern with another one in a string
 *
 *  These functions are wrappers around <code>boost::replace_all_copy</code>.
 *
 *  @{ */
ROSE_UTIL_API std::string replaceAllCopy(const std::string& input, const std::string& search, const std::string & replacedWith);

} // namespace
} // namespace

#endif
