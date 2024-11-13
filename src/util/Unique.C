#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Rose {
namespace Unique {

std::string
genUniqueID() {
	// Get the current timestamp
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // Use a counter to ensure uniqueness if generated within the same millisecond
    static int counter = 0;

    // Construct a unique id using a hex representation of the timestamp and counter
    std::ostringstream oss;
    oss << std::hex << timestamp << std::setw(4) << std::setfill('0') << counter++;
    return oss.str();
}

} // namespace
} // namespace
