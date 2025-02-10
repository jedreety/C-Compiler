#include "../Assembly.h"

namespace GC {

    std::string TokenToASM(const std::vector<GC::Token>& tokens) {
        // Begin our assembly output with a basic program header.
        std::string outputASM =
            "section .text\n"
            "global main\n"
            "extern ExitProcess\n\n"
            "main:\n";

        size_t tk_len = tokens.size();

        // A very simple parser example that only handles a return statement with an integer literal.
        for (size_t i = 0; i < tk_len; i++) {
            const GC::Token& token = tokens[i];

            // Check for a 'return' token.
            if (token.getType() == GC::TokenType::ExitProgram) {
                // Ensure the next token is an integer literal
                bool nextParamInt = (i + 1 < tk_len && tokens[i + 1].getType() == GC::TokenType::IntegerLiteral);

                if (nextParamInt) {
					// Add shadow space + 8 bytes alignment.
                    outputASM += "    sub rsp, 40\n";
                    // Generate the assembly to move the integer literal into RCX.
                    // We advance the index to consume the integer literal token.
                    outputASM += "    mov rcx, " + tokens[++i].getLexeme() + "\n";
                }
            }
        }

        outputASM += "    call ExitProcess\n";
        return outputASM;
    }

} // namespace GC
