#pragma once

#include <unordered_map>

#include "parser.h"

class Generator {
  public:

    inline Generator(NodeProg prog)
        :m_prog(std::move(prog)) {}

    void generate_expr(const NodeExpr& expr)
    {
        struct ExprVisitor
        {
            Generator* gen;

            void operator()(const NodeExprIntLit& expr_int_lit) const
            {
                gen->m_output << "    mov rax, " << expr_int_lit.int_literal.value.value() << std::endl;
                gen->push("rax");
                // gen->m_output << "    push rax\n";
            }
            void operator()(const NodeExprIdent& expr_ident) const
            {
                if (!gen->m_vars.contains(expr_ident.ident.value.value()))
                {
                    std::cerr << "undeclared identifier" << expr_ident.ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(expr_ident.ident.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str());
            }
        };

        ExprVisitor visitor{ this};
        std::visit(visitor, expr.var);
    }

     void generate_stmt(const NodeStmt& stmt)
    {
        struct StmtVisitor
        {
            Generator* gen;

            void operator()(const NodeStmtExit& stmt_exit) const
            {
                gen->generate_expr(stmt_exit.expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");// syscall number for exit

                gen->m_output << "    syscall\n";      // invoke syscall
            }
            void operator()(const NodeStmtLet& stmt_let) const
            {
                if(gen->m_vars.contains(stmt_let.ident.value.value()))
                {
                    std::cerr << "idetntifier already usefd" << stmt_let.ident.value.value()<< std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let.ident.value.value(), Var{gen->m_stack}});
                gen->generate_expr(stmt_let.expr);




            }
        };

        StmtVisitor visitor {this};
        std::visit(visitor, stmt.var);
    }


     std::string generate_prog()
        {
        // std::stringstream output;
        m_output << "global _start\n_start:\n";
        for (const NodeStmt& stmt : m_prog.stmts)
        {
             generate_stmt(stmt);
        }
        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";
        return m_output.str();
        // return m_output.str();

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

    struct  Var
    {
        size_t stack_loc;

    };



      const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack = 0;
    std::pmr::unordered_map<std::string, Var> m_vars{};


};