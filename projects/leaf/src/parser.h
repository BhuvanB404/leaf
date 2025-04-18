#pragma once
#include <string>
#include <vector>
#include "Tokenizer.h"
#include <optional>
#include <iostream>
#include <valarray>
#include <variant>

//namespace node {

struct NodeExprIntLit
{
    Token int_literal;
};

struct NodeExprIdent
{
    Token ident;
};
struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdent> var;
};

struct NodeStmtExit
{
    NodeExpr expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr expr;
};

struct NodeStmt
{
    std::variant<NodeStmtExit, NodeStmtLet> var;
};
struct NodeProg
{
    std::vector<NodeStmt> stmts;
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

    std::optional<NodeExpr> parse_expr()
    {
        if (peek().has_value() && peek().value().type == TokenType::int_literal) {
            // Token int_token = consume();

            return NodeExpr{.var = NodeExprIntLit{.int_literal =  consume()}};
        }
        else if (peek().has_value() && peek().value().type == TokenType::ident) {
            return NodeExpr{.var = NodeExprIdent{.ident = consume()}};
        }
        else{
            return {};
        }
    }

    std::optional<NodeStmt> parse_stmt()
    {

        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren )
        {
            consume();
            consume();
            
            if (auto node_expr = parse_expr())
            {
                NodeStmtExit stmt_exit{.expr = node_expr.value()};
                
                if (peek().has_value() && peek().value().type == TokenType::close_paren)
                {
                    consume();
                }
                else
                {
                    std::cerr << "expected ')'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if (peek() && peek().value().type == TokenType::semi) {
                    consume();
                } else {
                    std::cerr << "Expected ';'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return NodeStmt{.var = stmt_exit};
            }
            else
            {
                std::cerr << "Error parsing expression" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value()
            && peek(1).value().type == TokenType::ident
            && peek(2).has_value() && peek(2).value().type == TokenType::equal)
        {
                consume(); //consumes let
            auto stmt_let = NodeStmtLet {.ident = consume()}; // consuymes idetifyer
            consume(); //consume '='
            if (const auto expr = parse_expr())
            {
                stmt_let.expr = expr.value();
            }
            else
            {
                std::cerr << "Error parsing expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() && peek().value().type == TokenType::semi)
            {
                consume();
            }
            else
            {
                std::cerr << "Expected ';'" << std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStmt{.var = stmt_let};
        }
        else
        {
            return {};
        }
    }

   std::optional<NodeProg> parse_prog()
    {
        NodeProg prog;
        while (peek().has_value())
        {
            if (auto stmt = parse_stmt())
            {
                prog.stmts.push_back(stmt.value());
            }
            else
            {
                std::cerr << "Invalid stmts" << std::endl;
                exit(EXIT_FAILURE);
            }

        }
        return prog;
    }

private:
    std::vector<Token> m_tokens;
    size_t m_index;

    [[nodiscard]] std::optional<Token> peek(int ahead = 0) const {
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
