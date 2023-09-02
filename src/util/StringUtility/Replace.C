#include <Replace.h>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                     Replace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string replaceAllCopy(const std::string& input, const std::string& search, const std::string & replacedWith) {
	return boost::replace_all_copy(input, search, replacedWith);
	//TODO: for deBoost, this can be replaced with std::replace_copy when the compiler bumps to support c++17 by default
}

} // namespace
} // namespace
