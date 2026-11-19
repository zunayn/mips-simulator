#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Read-only, word-addressable MIPS instruction memory (text segment).
// This milestone supports sequential R-type programs; no branches/jumps yet.
class InstructionMemory {
public:
    static constexpr uint32_t kTextBase = 0x00400000u;
    void load(const std::vector<uint32_t>& words, uint32_t base = kTextBase);
    void loadHexFile(const std::string& path, uint32_t base = kTextBase);
    uint32_t fetch(uint32_t pc) const;
    bool contains(uint32_t pc) const;
    uint32_t endAddress() const; // first address past the loaded program
    uint32_t base() const { return base_; }
    std::size_t size() const { return words_.size(); }
private:
    uint32_t base_ = kTextBase;
    std::vector<uint32_t> words_;
};
