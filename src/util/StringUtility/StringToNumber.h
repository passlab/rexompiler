#ifndef ROSE_StringUtility_StringToNumber_H
#define ROSE_StringUtility_StringToNumber_H

#include <rosedll.h>
namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Number parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert an ASCII hexadecimal character to an integer.
 *
 *  Converts the characters 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f, A, B, C, D, E, and F into their hexadecimal integer
 *  equivalents. Returns zero if the input character is not in this set. */
ROSE_UTIL_API unsigned hexadecimalToInt(char);
} // namespace
} // namespace

#endif
