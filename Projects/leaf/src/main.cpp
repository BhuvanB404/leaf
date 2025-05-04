#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include <cctype>  // Add this for isalpha/isalnum/isdigit
// #include "Tokenizer.h"  // Replace the old tokenizer with the new one
#include "tokens.h"  // Use the new Flex-based tokenizer
#include "parser.h"
#include "generation.h"
#include "arena.h"

int main( int  argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "incorrect usage. Correct usage is.. " << std::endl;
        std::cerr << "leaf <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream file_stream;
        std::fstream input(argv[1], std::ios::in);
        file_stream << input.rdbuf();
        contents = file_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));

    std::vector<Token> tokens =  tokenizer.tokenize();
    Parser parser(std::move(tokens));

    std::optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value())
    {
        std::cerr << "INVALID PROGRAM" << std::endl;
        exit(EXIT_FAILURE);
    }

    {
        Generator generator(prog.value());
        std::fstream file("out.asm", std::ios::out);
        file << generator.generate_prog();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return  EXIT_SUCCESS;

}
