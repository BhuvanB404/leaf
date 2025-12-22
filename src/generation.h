#pragma once

#include <assert.h>
#include <map>
#include <string>
#include <sstream>
#include<format>
#include "parser.h"

class Generator
{
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

                auto it = std::find_if(gen->m_vars.cbegin(), gen->m_vars.cend(), [&](const Var& var)
                {
                    return var.name == term_ident->ident.value.value();});

                if ( it == gen->m_vars.cend())
                {
                    std::cerr << "undeclared identifier" << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                // const auto& var = (*it);
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack - (*it).stack_loc - 1) * 8 << "]";
                gen->push(offset.str());
            }

            void operator()(const NodeTermParen*  term_paren)
            {
                gen->generate_expr(term_paren->expr);
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
                gen->generate_expr(add->rhs);
                gen->generate_expr(add->lhs);
                gen->pop("rax");  // rhs
                gen->pop("rbx");  // lhs
                gen->m_output << "    add rax, rbx" << std::endl;
                gen->push("rax");
            }

            void operator()(const NodeBinExprSub* sub) const
            {
                gen->generate_expr(sub->rhs);
                gen->generate_expr(sub->lhs);
                gen->pop("rax");  // rhs
                gen->pop("rbx");  // lhs
                gen->m_output << "    sub rbx, rax" << std::endl;
                gen->push("rbx");
            }

            void operator()(const NodeBinExprMul* mul) const
            {
                gen->generate_expr(mul->rhs);
                gen->generate_expr(mul->lhs);
                gen->pop("rax");  // rhs
                gen->pop("rbx");  // lhs
                gen->m_output << "    imul rax, rbx" << std::endl;
                gen->push("rax");
            }
            void operator()(const NodeBinExprDiv* div) const
            {
                gen->generate_expr(div->lhs);
                gen->generate_expr(div->rhs);
                gen->pop("rcx");  // rhs (divisor)
                gen->pop("rax");  // lhs (dividend)
                gen->m_output << "    xor rdx, rdx" << std::endl;  // Clear rdx for division
                gen->m_output << "    idiv rcx" << std::endl;  // Divide rax by rcx, result in rax
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

    void generate_scope(const NodeScope* scope)
    {
        begin_scope();
        for (const NodeStmt* stmt : scope->stmts)
        {
            generate_stmt(*stmt);
        }
        end_scope();

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
                auto it = std::find_if(gen->m_vars.cbegin(), gen->m_vars.cend(), [&](const Var& var)
                {
                    return var.name == stmt_let->ident.value.value();});
                if(it != gen->m_vars.cend())
                {
                    std::cerr << "identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.push_back({stmt_let->ident.value.value(), gen->m_stack});
                gen->generate_expr(stmt_let->expr);
            }

            void operator()(const NodeScope* scope) const
            {
                gen->generate_scope(scope);

            }

            void operator()(const NodeStmtIf* stmt_if) const
            {
                assert(stmt_if != nullptr);

                gen->m_label_count++;
                

                gen->generate_expr(stmt_if->expr);
                gen->pop("rax");
                std::string label = gen->lable();
                gen->m_output << "    test rax, rax\n";
                gen->m_output << "    jz " << label.substr(0, label.size() - 1) << "\n";
                gen->generate_scope(stmt_if->scope);
                gen->m_output << label << std::endl;


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

    void begin_scope()
    {
        m_scopes.push_back(m_vars.size());
    }

    void end_scope()
    {
        size_t pop_count = m_vars.size() - m_scopes.back();
        m_output << "    add rsp, "<< pop_count * 8 << std::endl;
        m_stack -= pop_count;
        for (int i  = 0; i < pop_count; ++i)
        {
            m_vars.pop_back();
        }
        m_scopes.pop_back();

    }

    std::string lable()
    {
        std::stringstream ss;
        ss << "    label" << m_label_count++;
        return ss.str() + ":";
    }

    struct Var
    {
        std::string name;
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack = 0;

    std::vector<Var> m_vars{};
    std::vector<size_t> m_scopes{};
    // std::map<std::string, Var> m_vars{};
    size_t m_label_count = 0;
    int m_lable_count = 0;
};