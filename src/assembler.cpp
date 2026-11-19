#include "mips/assembler.h"

#include "mips/decoder.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace mips {
namespace {

struct RTypeOperation {
    unsigned function;
    bool shift;
};

const std::unordered_map<std::string, RTypeOperation> kOperations = {
    {"add",  {0x20, false}}, {"addu", {0x21, false}},
    {"sub",  {0x22, false}}, {"subu", {0x23, false}},
    {"and",  {0x24, false}}, {"or",   {0x25, false}},
    {"xor",  {0x26, false}}, {"nor",  {0x27, false}},
    {"slt",  {0x2a, false}}, {"sltu", {0x2b, false}},
    {"sll",  {0x00, true}},  {"srl",  {0x02, true}},
    {"sra",  {0x03, true}}
};

const std::unordered_map<std::string, unsigned> kRegisterAliases = {
    {"zero", 0}, {"at", 1},
    {"v0", 2}, {"v1", 3},
    {"a0", 4}, {"a1", 5}, {"a2", 6}, {"a3", 7},
    {"t0", 8}, {"t1", 9}, {"t2", 10}, {"t3", 11},
    {"t4", 12}, {"t5", 13}, {"t6", 14}, {"t7", 15},
    {"s0", 16}, {"s1", 17}, {"s2", 18}, {"s3", 19},
    {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
    {"t8", 24}, {"t9", 25}, {"k0", 26}, {"k1", 27},
    {"gp", 28}, {"sp", 29}, {"fp", 30}, {"s8", 30}, {"ra", 31}
};

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string lowercase(std::string value) {
    for (char& ch : value)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return value;
}

[[noreturn]] void invalidLine(unsigned lineNumber, const std::string& reason) {
    throw std::invalid_argument("Assembly line " + std::to_string(lineNumber)
                                + ": " + reason);
}

std::vector<std::string> splitOperands(const std::string& text, unsigned lineNumber) {
    std::vector<std::string> operands;
    if (text.empty()) return operands;

    std::size_t start = 0;
    while (true) {
        const auto comma = text.find(',', start);
        const auto operand = trim(text.substr(start, comma - start));
        if (operand.empty()) invalidLine(lineNumber, "empty operand");
        operands.push_back(operand);
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return operands;
}

unsigned parseRegister(const std::string& operand, unsigned lineNumber) {
    if (operand.size() < 2 || operand[0] != '$')
        invalidLine(lineNumber, "expected a register such as $t0 or $8");

    const std::string name = lowercase(operand.substr(1));
    const auto alias = kRegisterAliases.find(name);
    if (alias != kRegisterAliases.end()) return alias->second;

    if (name.empty()) invalidLine(lineNumber, "missing register name");
    unsigned index = 0;
    for (char digit : name) {
        if (digit < '0' || digit > '9' || index > 3)
            invalidLine(lineNumber, "unknown or out-of-range register '" + operand + "'");
        index = index * 10 + static_cast<unsigned>(digit - '0');
        if (index > 31)
            invalidLine(lineNumber, "unknown or out-of-range register '" + operand + "'");
    }
    return index;
}

unsigned parseShiftAmount(const std::string& operand, unsigned lineNumber) {
    unsigned amount = 0;
    if (operand.empty())
        invalidLine(lineNumber, "shift amount must be a decimal value from 0 to 31");
    for (char digit : operand) {
        if (digit < '0' || digit > '9' || amount > 3)
            invalidLine(lineNumber, "shift amount must be a decimal value from 0 to 31");
        amount = amount * 10 + static_cast<unsigned>(digit - '0');
        if (amount > 31)
            invalidLine(lineNumber, "shift amount must be a decimal value from 0 to 31");
    }
    return amount;
}

} // namespace

std::vector<std::uint32_t> assemble(std::istream& source) {
    std::vector<std::uint32_t> words;
    std::string line;
    unsigned lineNumber = 0;

    while (std::getline(source, line)) {
        ++lineNumber;
        const auto comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        line = trim(line);
        if (line.empty()) continue;

        const auto separator = line.find_first_of(" \t\r\n");
        const std::string mnemonic = lowercase(line.substr(0, separator));
        const std::string operandText = separator == std::string::npos
                                      ? std::string{} : trim(line.substr(separator + 1));
        const auto operands = splitOperands(operandText, lineNumber);

        if (mnemonic == "nop") {
            if (!operands.empty()) invalidLine(lineNumber, "nop takes no operands");
            words.push_back(0);
            continue;
        }

        const auto operation = kOperations.find(mnemonic);
        if (operation == kOperations.end())
            invalidLine(lineNumber, "unsupported instruction '" + mnemonic + "'");
        if (operands.size() != 3)
            invalidLine(lineNumber, mnemonic + " requires three comma-separated operands");

        const unsigned rd = parseRegister(operands[0], lineNumber);
        unsigned rs = 0;
        unsigned rt = 0;
        unsigned shiftAmount = 0;
        if (operation->second.shift) {
            rt = parseRegister(operands[1], lineNumber);
            shiftAmount = parseShiftAmount(operands[2], lineNumber);
        } else {
            rs = parseRegister(operands[1], lineNumber);
            rt = parseRegister(operands[2], lineNumber);
        }
        words.push_back(encodeRType(rs, rt, rd, shiftAmount, operation->second.function));
    }

    if (source.bad()) throw std::runtime_error("Error reading assembly source");
    return words;
}

std::vector<std::uint32_t> assembleFile(const std::string& path) {
    std::ifstream source(path);
    if (!source) throw std::runtime_error("Cannot open assembly file: " + path);
    return assemble(source);
}

} // namespace mips
