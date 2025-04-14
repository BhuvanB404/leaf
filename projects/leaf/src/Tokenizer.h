

#pragma once

#include <vector>
#include  <string>

enum class TokenType
{
    exit,
    int_literal,
    semi,
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
        while (peak().has_value())
        {
            if (std::isalpha(peak().value()))
            {
                buf.push_back(consume());

                while (std::isalnum(peak().value()) && peak().has_value())
                {
                    buf.push_back(consume());
                }

                if (buf == "exit")
                {
                    tokens.push_back({.type = TokenType::exit});
                    buf.clear();
                    continue;
                }
                else
                {
                    std::cerr << "YOU FUCKED UP" << std::endl;
                    exit(EXIT_FAILURE);
                }

            }

            else if (std::isdigit(peak().value()))
            {
                buf.push_back(consume());
                while (peak().has_value() && std::isdigit(peak().value()))
                {
                    buf.push_back(consume());
                }

                tokens.push_back({.type = TokenType::int_literal, .value = buf});
                buf.clear();
                continue;

            }

            else if(peak().value() == ';')
            {   consume();
                tokens.push_back({.type = TokenType::semi, .value = buf});
                continue;
            }

            else if (std::isspace(peak().value()))
            {   consume();
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

    [[nodiscard]] inline std::optional<char> peak(int ahead = 1) const
    {
        if (m_index + ahead  > m_src.length())
        {
            return std::nullopt;
        }
        else
        {
            return m_src.at(m_index);
        }
    };
inline char consume()
{
    return m_src.at(m_index++);

}

    const std::string m_src;
    size_t m_index = 0;
};
