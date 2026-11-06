#include "instruction_memory.hpp"
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

void InstructionMemory::load(const std::vector<uint32_t>& words, uint32_t base) {
    if (base % 4 != 0) throw std::invalid_argument("Text base must be word aligned");
    // Make endAddress() representable and ensure PC + 4 cannot overflow.
    if (words.size() > (static_cast<uint64_t>(UINT32_MAX) - base) / 4u)
        throw std::out_of_range("Program exceeds 32-bit address space");
    words_ = words;
    base_ = base;
}

void InstructionMemory::loadHexFile(const std::string& path, uint32_t base) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Cannot open instruction file: " + path);
    std::vector<uint32_t> words;
    std::string line;
    unsigned lineNumber = 0;
    while (std::getline(in, line)) {
        ++lineNumber;
        auto comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        std::istringstream ss(line);
        std::string token, extra;
        if (!(ss >> token)) continue;
        if (ss >> extra) throw std::invalid_argument("More than one word on line " + std::to_string(lineNumber));
        std::size_t consumed = 0;
        unsigned long long parsed = 0;
        try { parsed = std::stoull(token, &consumed, 16); }
        catch (const std::exception&) {
            throw std::invalid_argument("Invalid hexadecimal word on line " + std::to_string(lineNumber));
        }
        if (consumed != token.size() || parsed > UINT32_MAX)
            throw std::invalid_argument("Invalid 32-bit word on line " + std::to_string(lineNumber));
        words.push_back(static_cast<uint32_t>(parsed));
    }
    if (in.bad()) throw std::runtime_error("Error reading instruction file");
    load(words, base);
}

bool InstructionMemory::contains(uint32_t pc) const {
    return pc >= base_ && pc < endAddress() && ((pc - base_) % 4u == 0);
}

uint32_t InstructionMemory::endAddress() const {
    return base_ + static_cast<uint32_t>(words_.size() * 4u);
}

uint32_t InstructionMemory::fetch(uint32_t pc) const {
    if (!contains(pc)) throw std::out_of_range("PC is outside loaded instruction memory or unaligned");
    return words_[(pc - base_) / 4u];
}
