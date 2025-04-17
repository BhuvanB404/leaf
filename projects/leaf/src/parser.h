#pragma once
#include <string>
#include <vector>
#include "Tokenizer.h"
#include <optional>
#include <iostream>

//namespace node {
struct NodeExpr {
    Token int_literal;
};

struct NodeExit {
    NodeExpr expr;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_index(0) 
    {
    }

    std::optional<NodeExpr> parse_expr() {
        if (peek() && peek().value().type == TokenType::int_literal) {
            return NodeExpr{.int_literal = consume()};
        }
        return {};
    }

    std::optional<NodeExit> parse() {
        std::optional<NodeExit> exit_node;

        while (peek()) {
            if (peek().value().type == TokenType::exit) {
                consume(); // consume 'exit' token
                if (auto node_expr = parse_expr()) {
                    exit_node = NodeExit{.expr = node_expr.value()};
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (peek() && peek().value().type == TokenType::semi) {
                    consume();
                } else {
                    std::cerr << "Expected semicolon" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }

        return exit_node;
    }

private:
    std::vector<Token> m_tokens;
    size_t m_index;

    std::optional<Token> peek(int ahead = 0) const {
        if (m_index + ahead >= m_tokens.size()) {
            return {};
        }
        return m_tokens.at(m_index + ahead);
    }

    Token consume() {
        return m_tokens.at(m_index++);
    }
}; // Added missing closing brace
//} // Commented out namespace
