#pragma once

#include <string>
#include <optional>
#include <vector>

// Token types (using uppercase to match the flex.l implementation)
enum class TokenType
{
    EXIT,
    INT_LITERAL,
    IDENT,
    LET,
    EQUAL,
    SEMI,
    OPEN_PAREN,
    CLOSE_PAREN,
    PLUS,
    MINUS,
    MULTI,
    DIV,
    OPEN_CURLY,
    CLOSE_CURLY,
    IF,
};

// Function to get binary operator precedence
std::optional<int> bin_preci(TokenType type);

// Define the Token structure
struct Token
{
    TokenType type;
    std::optional<std::string> value{};
};

// Forward declaration of the Flex class
class Flex;

// Tokenizer class (wrapper for Flex to maintain compatibility)
class Tokenizer {
public:
    explicit Tokenizer(const std::string& src);
    
    std::vector<Token> tokenize(); // Note: Changed from tokenizer() to match main.cpp usage
    
private:
    Flex lexer;
}; 