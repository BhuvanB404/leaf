#pragma once

#include <vector>
#include  <string>
#include  <optional>


enum class TokenType
{
    exit,
    int_literal,
    ident,
    let,
    equal,
    semi,
    open_paren,
    close_paren,
};

struct Token
{
    TokenType type;
    std::optional<std::string> value{};
};

class Tokenizer
{
    public:
    inline  explicit Tokenizer(const std::string& src)
        :m_src(std::move(src))
    {

    }

    // tokeniser


    inline std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;
        std::string buf;
        while (peek().has_value())
        {
            if (std::isalpha(peek().value()))
            {
                buf.push_back(consume());

                while (peek().has_value() && std::isalnum(peek().value()))
                {
                    buf.push_back(consume());
                }

                if (buf == "exit")
                {
                    tokens.push_back({.type = TokenType::exit});
                    buf.clear();
                    // continue;
                }
                else if (buf == "let")
                {
                    tokens.push_back({.type = TokenType::let});
                    buf.clear();
                    // continue;
                }
                else
                {
                    tokens.push_back({.type = TokenType::ident, .value = buf});
                    buf.clear();
                }

            }

            else if (std::isdigit(peek().value()))
            {
                buf.clear();
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value()))
                {
                    buf.push_back(consume());
                }

                tokens.push_back({.type = TokenType::int_literal, .value = buf});
                buf.clear();
                continue;

            }

            else if (peek().value() == '(')
            {
                consume();
                tokens.push_back({.type = TokenType::open_paren});
            }

            else if (peek().value() == ')')
            {
                consume();
                tokens.push_back({.type = TokenType::close_paren});
            }

            else if(peek().value() == ';')
            {   
                consume();
                tokens.push_back({.type = TokenType::semi});
                continue;
            }
            else if(peek().value() == '=')
            {
                consume();
                tokens.push_back({.type = TokenType::equal});
                continue;
            }

            else if (std::isspace(peek().value()))
            {   
                consume();
                continue;
            }
            else
            {
                std::cerr << "YPU FUCKED UOP"  << std::endl;
                exit(EXIT_FAILURE);
            }

        }
        m_index = 0;
        return tokens;

    }


    private:

    [[nodiscard]] inline std::optional<char> peek(int ahead = 0) const
    {
        if (m_index + ahead >= m_src.length())
        {
            return std::nullopt;
        }
        else
        {
            return m_src.at(m_index + ahead);
        }
    };
inline char consume()
{
    return m_src.at(m_index++);

}

    const std::string m_src;
    size_t m_index = 0;
};
