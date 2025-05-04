%{
#include <iostream>
#include <vector>
#include  <string>
#include  <optional>
#include "tokens.h"



//Token types
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

typedef struct
{
        TokenType type;
        std::optional<std::string> value{};
}Token;


//the precedence of binarhy operators
std::optional<int> bin_preci(TokenType type)
{
    switch (type)
    {
        case TokenType::PLUS:
        case TokenType::MINUS:
             0;
        case TokenType::MULTI:
        case TokenType::DIV:
             1;
        default:
             {};
    }
}


std::vector<Token> tokens;


void add_token(TokenType type, const char* value = nullptr) {
    if (value) {
        tokens.push_back({.type = type, .value = std::string(value)});
    } else {
        tokens.push_back({.type = type});
    }
}

%}

%option noyywrap
%option c++
%option yyclass="FlexLexer"

//regular expressions

%%

"exit" {  add_token(TokenType::EXIT);}
"let" { add_token(TokenType::LET);}
"if" { add_token(TokenType::IF);}

[a-zA-z][a-zA-Z0-9]* { add_token(TokenType::IDENT, yytext);}

[0-9]+ { add_token(TokenType::INT_LITERAL,yytext);}

"("  {  add_token(TokenType::OPEN_PAREN);}
")"  {  add_token(TokenType::CLOSE_PAREN);}
";"  {  add_token(TokenType::SEMI);}
"="  {  add_token(TokenType::EQUAL);}
"+"  {  add_token(TokenType::PLUS);}
"-"  {  add_token(TokenType::MINUS);}
"*"  {  add_token(TokenType::MULTI);}
"/"  {  add_token(TokenType::DIV);}
"{"  {  add_token(TokenType::OPEN_CURLY);}
"}"  {  add_token(TokenType::CLOSE_CURLY);}

[ \t\n\r] {   }

.      {
            std::cerr << "invalid character encountered: " << yytext << '/n';
            exit(EXIT_FAILURE);

}

%%

class Flex
{
        public:
        explicit Flex(const std::string& src)
        {
                yy_scan_string(src.c_str());
        }

        std::vector<Token> tokenize()
        {
                tokens.clear();

                //runs lexer
                yylex();

                //return vector of tokens
                return tokens;
        }

};



class Tokenizer{
        public:
        
        explicit Tokenizer(const std::string& src)
                : lexer(src)
                {

                }

        std::vector<Token> tokenizer()
        {
                return lexer.tokenize();
        }

        private:
        
        Flex lexer;
};



