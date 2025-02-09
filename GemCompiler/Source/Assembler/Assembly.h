#pragma once
#include <string>
#include <vector>
#include "../Token/Token.h"

namespace GC {

    // Converts a vector of tokens into NASM-style assembly code.
    std::string TokenToASM(const std::vector<GC::Token>& tokens);

} // namespace GC
