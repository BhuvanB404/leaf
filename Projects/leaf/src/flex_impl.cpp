#include "tokens.h"
#include <iostream>

// External declarations from Flex
extern "C" int yylex();
extern "C" void yy_scan_string(const char* str);
extern "C" void yylex_destroy();

// Token vector used by Flex
std::vector<Token> tokens;

// Implementation of bin_preci
std::optional<int> bin_preci(TokenType type)
{
    switch (type)
    {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 0;
        case TokenType::MULTI:
        case TokenType::DIV:
            return 1;
        default:
            return {};
    }
}

// Flex class implementation
class Flex
{
public:
    explicit Flex(const std::string& src)
    {
        yy_scan_string(src.c_str());
    }

    ~Flex()
    {
        yylex_destroy();
    }

    std::vector<Token> tokenize()
    {
        tokens.clear();
        
        // Run the lexer
        yylex();
        
        // Return the collected tokens
        return tokens;
    }
};

// Tokenizer implementation
Tokenizer::Tokenizer(const std::string& src)
    : lexer(src)
{
}

std::vector<Token> Tokenizer::tokenize()
{
    return lexer.tokenize();
} 