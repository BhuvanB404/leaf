#pragma once
#include <assert.h>
#include <string>
#include <vector>
#include "Tokenizer.h"
#include <optional>
#include <iostream>
#include <valarray>
#include <variant>
#include "arena.h"

//namespace node {

struct NodeExpr;

struct NodeTermIntLit
{
    Token int_literal;
};

struct NodeTermIdent
{
    Token ident;
};

struct NodeTermParen
{
    NodeExpr* expr;
};

struct NodeBinExprAdd
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};
struct NodeBinExprSub
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprMul
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprDiv
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr
{
    std::variant<NodeBinExprAdd*, NodeBinExprSub*, NodeBinExprMul*, NodeBinExprDiv*> var;
};

struct NodeTerm
{
    std::variant<NodeTermIntLit*, NodeTermIdent*,NodeTermParen*> var;
};


struct NodeExpr {
    std::variant<NodeTerm*, NodeBinExpr*> var;
};




struct NodeStmtExit
{
    NodeExpr* expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr* expr{};
};
struct NodeStmt;

struct NodeScope
{
    std::vector<NodeStmt*> stmts;
};

struct NodeStmtIf
{
    NodeExpr* expr;
    NodeScope* scope;
};

struct NodeStmt
{
    std::variant<NodeStmtExit*, NodeStmtLet*, NodeScope*, NodeStmtIf*> var;
};

struct NodeProg
{
    std::vector<NodeStmt> stmts;
};

struct NodeExit {
    NodeExpr* expr;
};


class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_index(0) ,
            m_allocator(1024*1024 * 4) // 4mb
    {
    }

    std::optional<NodeTerm*> parse_term()
    {
        if (auto int_literal = try_consume(TokenType::int_literal))
  {
            // Token int_token = consume();
            auto* term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_literal = int_literal.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        }
        else if (auto ident = try_consume( TokenType::ident)) {
            auto term_ident = m_allocator.alloc<NodeTermIdent>();
            term_ident->ident = ident.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_ident;
            return term;
        }
        else if (auto open_paren = try_consume(TokenType::open_paren))
        {
            auto expr = parse_expr();
            if (!expr.has_value())
            {
                std::cerr << "expected expression" << std::endl;
                exit(1);
            }
            try_consume(TokenType::close_paren, "expected close paren");
            auto term_paren = m_allocator.alloc<NodeTermParen>();
            term_paren->expr = expr.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_paren;
            return term;
        }
        else {
            return {};
        }
    }

    std::optional<NodeExpr*> parse_expr(int min_presidence = 0)
    {
        std::optional<NodeTerm*> term_lhs = parse_term();
        if (!term_lhs)
        {
            return {};
        }
        auto expr_lhs = m_allocator.alloc<NodeExpr>();
        expr_lhs->var = term_lhs.value();
        while (true)
        {

            std::optional<Token> currnt_token = peek();
            std::optional<int> presidence;
            if (currnt_token.has_value())
            {
                presidence = bin_preci(currnt_token->type);
                if (!presidence.has_value() || presidence.value() < min_presidence)
                    break;
            }
            else
            {
                break;
            }
            Token op = consume();
            int next_min_preci = presidence.value() + 1;
            auto expr_rhs = parse_expr(next_min_preci);
            if (!expr_rhs.has_value())
            {
                std::cerr << "unable to parse expression" << std::endl;
                exit(EXIT_FAILURE);
            }


            auto expr = m_allocator.alloc<NodeBinExpr>();

            if (op.type == TokenType::plus)
            {
                auto add = m_allocator.alloc<NodeBinExprAdd>();
                add->lhs = expr_lhs;
                add->rhs = expr_rhs.value();
                expr->var = add;

                expr->var = add;
            }
            else if (op.type == TokenType::minus)
            {
                auto sub = m_allocator.alloc<NodeBinExprSub>();
                sub->lhs = expr_lhs;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            }

            else if (op.type == TokenType::multi)
            {
                auto mul = m_allocator.alloc<NodeBinExprMul>();
                mul->lhs = expr_lhs;
                mul->rhs = expr_rhs.value();
                expr->var = mul;
            }
            else if (op.type == TokenType::div)
            {
                auto div = m_allocator.alloc<NodeBinExprDiv>();
                div->lhs = expr_lhs;
                div->rhs = expr_rhs.value();
                expr->var = div;
            }
            else
            {
                assert(false);
            }

            // expr_lhs->var = expr;

            auto new_expr = m_allocator.alloc<NodeExpr>();
            new_expr->var = expr;
            expr_lhs = new_expr;
        }

        return expr_lhs;

        // if (auto term = parse_term())
        // {
        //     auto expr = m_allocator.alloc<NodeExpr>();
        //     expr->var = term.value();
        //
        //     if (try_consume( TokenType::plus).has_value())
        //     {
        //         auto bin_expr = m_allocator.alloc<NodeBinExpr>();
        //         auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
        //
        //         bin_expr_add->lhs = expr;
        //
        //         if (auto rhs = parse_expr())
        //         {
        //             bin_expr_add->rhs = rhs.value();
        //             bin_expr->var = bin_expr_add;
        //
        //             auto new_expr = m_allocator.alloc<NodeExpr>();
        //             new_expr->var = bin_expr;
        //             return new_expr;
        //         }
        //         else
        //         {
        //             std::cerr << "Expected expression" << std::endl;
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //     else if (try_consume( TokenType::minus).has_value())
        //     {
        //         auto bin_expr = m_allocator.alloc<NodeBinExpr>();
        //         auto bin_expr_sub = m_allocator.alloc<NodeBinExprSub>();
        //
        //         bin_expr_sub->lhs = expr;
        //
        //         if (auto rhs = parse_expr())
        //         {
        //             bin_expr_sub->rhs = rhs.value();
        //             bin_expr->var = bin_expr_sub;
        //
        //             auto new_expr = m_allocator.alloc<NodeExpr>();
        //             new_expr->var = bin_expr;
        //             return new_expr;
        //         }
        //         else
        //         {
        //             std::cerr << "Expected expression" << std::endl;
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //     else if (try_consume( TokenType::multi).has_value())
        //     {
        //         auto bin_expr = m_allocator.alloc<NodeBinExpr>();
        //         auto bin_expr_mul = m_allocator.alloc<NodeBinExprMul>();
        //
        //         bin_expr_mul->lhs = expr;
        //
        //         if (auto rhs = parse_expr())
        //         {
        //             bin_expr_mul->rhs = rhs.value();
        //             bin_expr->var = bin_expr_mul;
        //
        //             auto new_expr = m_allocator.alloc<NodeExpr>();
        //             new_expr->var = bin_expr;
        //             return new_expr;
        //         }
        //         else
        //         {
        //             std::cerr << "Expected expression" << std::endl;
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //
        //     return expr;
        // }
        //
        // return {};
    }

    std::optional<NodeScope*> parse_scope()
    {
        if (!try_consume(TokenType::open_curly).has_value())
        {
            return {};
        }
        auto scope = m_allocator.alloc<NodeScope>();
        while (auto stmt = parse_stmt())
        {
            scope->stmts.push_back(stmt.value());
        }

        try_consume(TokenType::close_curly, "expected close curly");
      return scope;
    }

    std::optional<NodeStmt*> parse_stmt()
    {

        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren )
        {
            consume();
            consume();
            
            if (auto node_expr = parse_expr())
            {
                // NodeStmtExit stmt_exit{.expr = node_expr.value()};
                auto stmt_exit = m_allocator.alloc<NodeStmtExit>();
                stmt_exit->expr = node_expr.value();

                try_consume(TokenType::close_paren, "Expected ')'");
                try_consume(TokenType::semi, "Expected ';'");

                auto stmt = m_allocator.alloc<NodeStmt>();
                stmt->var = stmt_exit;
                return stmt;
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
            // auto stmt_let = NodeStmtLet {.ident = consume()}; // consuymes idetifyer
            auto  stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = consume();

            consume(); //consume '='
            if (const auto expr = parse_expr())
            {
                stmt_let->expr = expr.value();
            }
            else
            {
                std::cerr << "Error parsing expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::semi, "Expected ';'");

            // return NodeStmt{.var = stmt_let};
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        }
        else if (peek().has_value() && peek().value().type == TokenType::open_curly)
        {
       if (auto scope = parse_scope())
       {
           auto stmt = m_allocator.alloc<NodeStmt>();
           stmt->var = scope.value();
           return stmt;
       }
            else
            {
                std::cerr << "Invalid scope" << std::endl;
            }
        }
        else if (auto if_ = try_consume(TokenType::if_))
        {
            try_consume(TokenType::open_paren, "Expected '('");
            auto stmt_if = m_allocator.alloc<NodeStmtIf>();
            if (auto expr = parse_expr())
            {
                stmt_if->expr = expr.value();

            }
            else
            {
                std::cerr << "INVALID expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')'");
            if (auto scope = parse_scope())
            {
                 stmt_if->scope = scope.value();
            }
            else
            {
                std::cerr << "Invalid scope" << std::endl;
                exit(EXIT_FAILURE);
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_if;
            return stmt;
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
                prog.stmts.push_back(*stmt.value());
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


    [[nodiscard]] std::optional<Token> peek(int ahead = 0) const {
        if (m_index + ahead >= m_tokens.size()) {
            return {};
        }
        return m_tokens.at(m_index + ahead);

    }

    Token consume() {
        return m_tokens.at(m_index++);
    }

    inline Token try_consume(TokenType type, const std::string& err_msg)
    {
        if (peek().has_value() && peek().value().type == type)
        {
            return consume();
        }
        else
        {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline std::optional<Token> try_consume(TokenType type)
    {
        if (peek().has_value() && peek().value().type == type)
        {
            return consume();
        }
        else
        {
            return {};
        }
    }

    std::vector<Token> m_tokens;
    size_t m_index;
    ArenaAllocator m_allocator;

}; // Added missing closing brace
//} // Commented out namespace
