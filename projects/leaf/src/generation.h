#pragma once

#include "parser.h"

class Generator {
  public:

    inline Generator(NodeProg prog)
        :m_prog(std::move(prog)) {}

    std::string generate_expr(NodeExpr& expr)
    {
        struct ExprVisitor
        {
            Generator* gen;

            void operator()(const NodeExprIntLit& expr_int_lit)
            {
                gen->m_output << "mov rax, " << expr_int_lit.int_literal.value.value() << std::endl;
                gen->m_output << "push rax\n";
            }
            void operator()(const NodeExprIdent& expr_ident)
            {

            }
        };

        ExprVisitor visitor{ this};
        std::visit(visitor, expr.var);

    }

    std::string generate_stmt(const NodeStmt& stmt)
    {
        struct StmtVisitor
        {
            void operator()(const NodeStmtExit& stmt_exit)
            {
                output << "    mov rax, 60\n";
                output << "    mov rdi, " ;
                output << "    syscall";
            }
            void operator()(const NodeStmtLet& stmt_let)
            {

            }
        };

        StmtVisitor visitor;
        std::visit(visitor, stmt.var);
    }


     std::string generate_prog()
        {
        // std::stringstream output;
        m_output << "global _start\n_start:\n";
        for (const NodeStmt& stmt : m_prog.stmts)
        {
            m_output << generate_stmt();
        }

        return output.str();
        }

    private:

      const NodeProg m_prog;
    std::stringstream m_output;


};