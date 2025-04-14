#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include <cctype>  // Add this for isalpha/isalnum/isdigit
#include "Tokenizer.h"
#include "parser.h"
#include "generation.h"
//
// enum class TokenType {
//     exit,
//     int_lit,
//     semi,
// };
//
// struct Token {
//     TokenType type;
//     std::optional<std::string> value {};
// };
//



// std::string tokens_to_asm(const std::vector<Token>& tokens)
// {
//     std::stringstream output;
//     output << "global _start\n_start:\n";
//     for (int i = 0; i < tokens.size(); i++)
//     {
//         const Token& token = tokens.at(i);
//         if (token.type == TokenType::exit)
//         {
//             if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::int_literal)
//             {
//                 if (i + 2 < tokens.size() && tokens[i + 2].type == TokenType::semi)
//                 {
//                     output << "    mov rax, 60\n";
//                     output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
//                     output << "    syscall";
//                 }
//             }
//         }
//
//     }
//     return output.str();
// }


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
    std::optional<NodeExit> tree = parser.parse();

    if (!tree.has_value())
    {
        std::cerr << "failed to parse tree" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());
    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return  EXIT_SUCCESS;

}
