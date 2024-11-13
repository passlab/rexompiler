#include <Replace.h>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                     Replace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string replaceAllCopy(const std::string& input, const std::string& search, const std::string & replacedWith) {
	if (search.empty()) return input;  // Avoid infinite loop if 'search' is empty

	std::string result;
	result.reserve(input.size());  // Reserve space to avoid multiple allocations

	std::size_t start_pos = 0;
	std::size_t found_pos;

	// Iterate through the input string
	while ((found_pos = input.find(search, start_pos)) != std::string::npos) {
	    // Append the part before the found substring
	    result.append(input, start_pos, found_pos - start_pos);
	    // Append the replacement substring
	    result.append(replacedWith);
	    // Update start position to continue after the found substring
	    start_pos = found_pos + search.size();
	}

	// Append the rest of the string after the last found position
	result.append(input, start_pos, input.size() - start_pos);

	return result;
}

} // namespace
} // namespace
