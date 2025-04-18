#pragma once

#include <assert.h>
#include <unordered_map>

#include "parser.h"

class Generator {
  public:

    inline explicit Generator(NodeProg prog)
        :m_prog(std::move(prog)) {}

    void generate_term(const NodeTerm* term)
    {
        struct TermVisitor
        {
            Generator* gen;

            void operator()(const NodeTermIntLit* term_int_lit) const
            {
                gen->m_output << "    mov rax, " << term_int_lit->int_literal.value.value() << std::endl;
                gen->push("rax");
            }
            void operator()(const NodeTermIdent* term_ident) const
            {
                if (!gen->m_vars.contains(term_ident->ident.value.value()))
                {
                    std::cerr << "undeclared identifier" << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str());
            }
        };

        TermVisitor visitor{ this };
        std::visit(visitor, term->var);
    }

    void generate_bin_expr(const NodeBinExpr* bin_expr)
    {
        struct BinExprVisitor
        {
            Generator* gen;

            void operator()(const NodeBinExprAdd* add) const
            {
                gen->generate_expr(add->lhs);
                gen->generate_expr(add->rhs);
                gen->pop("rax");  // rhs
                gen->pop("rbx");  // lhs
                gen->m_output << "    add rax, rbx" << std::endl;
                gen->push("rax");
            }

            void operator()(const NodeBinExprSub* sub) const
            {
                gen->generate_expr(sub->lhs);
                gen->generate_expr(sub->rhs);
                gen->pop("rax");  // rhs
                gen->pop("rbx");  // lhs
                gen->m_output << "    sub rbx, rax" << std::endl;
                gen->push("rbx");
            }

            void operator()(const NodeBinExprMul* mul) const
            {
                gen->generate_expr(mul->lhs);
                gen->generate_expr(mul->rhs);
                gen->pop("rax");  // rhs
                gen->pop("rbx");  // lhs
                gen->m_output << "    imul rax, rbx" << std::endl;
                gen->push("rax");
            }
        };

        BinExprVisitor visitor{ this };
        std::visit(visitor, bin_expr->var);
    }

    void generate_expr(const NodeExpr* expr)
    {
        struct ExprVisitor
        {
            Generator* gen;

            void operator()(const NodeTerm* term) const
            {
                gen->generate_term(term);
            }
            
            void operator()(const NodeBinExpr* bin_expr) const
            {
                gen->generate_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor{ this };
        std::visit(visitor, expr->var);
    }

    void generate_stmt(const NodeStmt& stmt)
    {
        struct StmtVisitor
        {
            Generator* gen;

            void operator()(const NodeStmtExit* stmt_exit) const
            {
                gen->generate_expr(stmt_exit->expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");// syscall number for exit
                gen->m_output << "    syscall\n";      // invoke syscall
            }
            void operator()(const NodeStmtLet* stmt_let) const
            {
                if(gen->m_vars.contains(stmt_let->ident.value.value()))
                {
                    std::cerr << "identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let->ident.value.value(), Var{gen->m_stack}});
                gen->generate_expr(stmt_let->expr);
            }
        };

        StmtVisitor visitor {this};
        std::visit(visitor, stmt.var);
    }


     std::string generate_prog()
        {
        m_output << "global _start\n_start:\n";
        for (const NodeStmt& stmt : m_prog.stmts)
        {
             generate_stmt(stmt);
        }
        return m_output.str();
        }

    private:

    void push(const std::string& reg)
    {
        m_output << "    push " << reg << std::endl;
        m_stack++;
    }

    void pop(const std::string& reg)
    {
        m_output << "    pop " << reg << std::endl;
        m_stack--;
    }

    struct Var
    {
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack = 0;
    std::pmr::unordered_map<std::string, Var> m_vars{};
};