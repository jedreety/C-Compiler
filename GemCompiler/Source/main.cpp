#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Token/Token.h"
#include "Assembler/Assembly.h"

int main(int argc, char* argv[]) {
    // Check if the user has provided exactly one argument (the file path)
    /*if (argc != 2) {
        std::cerr << "Usage: GemCompiler <path to file>\nPlease provide the path to the file." << std::endl;
        return EXIT_FAILURE;
    }*/

    // Open the file
    //std::ifstream file(argv[1], std::ios::in);
    std::ifstream file("GemFiles/base.gem", std::ios::in);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << argv[1] << "'." << std::endl;
        return EXIT_FAILURE;
    }

    // Read the file content
    std::stringstream contents_stream;
    contents_stream << file.rdbuf();
    std::string content = contents_stream.str();

    // Close the file
    file.close();

    // Output the file content
    std::cout << "File content:\n" << content << "\n\n";

    auto tk_len = GC::Tokenize(content);

    std::cout << "\n\n" << GC::TokenToASM(tk_len);

    {

		std::fstream file("outAsm/out.asm", std::ios::out);
		file << GC::TokenToASM(tk_len);

    }


    return 0;
}