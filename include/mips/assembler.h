#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

namespace mips {

// Assemble the supported R-type subset from a stream or a text file.
// Blank lines and # comments are ignored; labels and directives are not supported.
std::vector<std::uint32_t> assemble(std::istream& source);
std::vector<std::uint32_t> assembleFile(const std::string& path);

} // namespace mips