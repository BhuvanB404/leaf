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
    plus,
    minus,
    multi,
    div,
    open_curly,
    close_curly,
    if_,
};


std::optional<int> bin_preci(TokenType type)
{
    switch (type)
    {
        case TokenType::plus:
        case TokenType::minus:
            return 0;break;
        case TokenType::multi:
        case TokenType::div:
            return 1;break;
        default:
            return {};
    }
}
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
            char current = peek().value();
            
            if (std::isalpha(current))
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
                else if (buf == "if")
                {
                    tokens.push_back({.type = TokenType::if_});
                    buf.clear();
                }
                else
                {
                    tokens.push_back({.type = TokenType::ident, .value = buf});
                    buf.clear();
                }
            }
            else
            {
                switch (current)
                {
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        buf.clear();
                        buf.push_back(consume());
                        while (peek().has_value() && std::isdigit(peek().value()))
                        {
                            buf.push_back(consume());
                        }
                        tokens.push_back({.type = TokenType::int_literal, .value = buf});
                        buf.clear();
                        break;

                    case '(':
                        consume();
                        tokens.push_back({.type = TokenType::open_paren});
                        break;

                    case ')':
                        consume();
                        tokens.push_back({.type = TokenType::close_paren});
                        break;

                    case ';':
                        consume();
                        tokens.push_back({.type = TokenType::semi});
                        break;

                    case '=':
                        consume();
                        tokens.push_back({.type = TokenType::equal});
                        break;

                    case '+':
                        consume();
                        tokens.push_back({.type = TokenType::plus});
                        break;

                    case '-':
                        consume();
                        tokens.push_back({.type = TokenType::minus});
                        break;

                    case '*':
                        consume();
                        tokens.push_back({.type = TokenType::multi});
                        break;

                    case '/':
                        consume();
                        tokens.push_back({.type = TokenType::div});
                        break;
                    case '{':
                        consume();
                    tokens.push_back({.type = TokenType::open_curly});
                        break;
                    case '}':
                        consume();
                    tokens.push_back({.type = TokenType::close_curly});
                    break;

                    default:
                        if (std::isspace(current))
                        {
                            consume();
                        }
                        else
                        {
                            std::cerr << "Invalid character encountered" << std::endl;
                            exit(EXIT_FAILURE);
                        }
                        break;
                }
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
