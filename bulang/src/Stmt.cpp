
#include "pch.h" 
#include "Stmt.hpp" 
#include "Expr.hpp"
#include "Interpreter.hpp"
#include "Utils.hpp"




u8 BlockStmt::visit(Visitor &v)
{
   return  v.visit_block_smt(this);
}

u8 ExpressionStmt::visit(Visitor &v)
{
  return  v.visit_expression_smt(this);
}

u8 PrintStmt::visit(Visitor &v)
{
   return v.visit_print_smt(this);
}

u8 Program::visit(Visitor &v)
{
   return v.visit_program(this);
}

u8 Declaration::visit(Visitor &v)
{
   return v.visit_declaration(this);
}

IFStmt::IFStmt():Stmt(), condition(nullptr), then_branch(nullptr), else_branch(nullptr)
{
    type = StmtType::IF;
}

u8 IFStmt::visit(Visitor &v)
{
  return  v.visit_if(this);
}

u8 WhileStmt::visit(Visitor &v)
{
  return  v.visit_while(this);
}

u8 ForStmt::visit(Visitor &v)
{
    return v.visit_for(this);
}

u8 ContinueStmt::visit(Visitor &v)
{
    return v.visit_continue(this);
}

u8 BreakStmt::visit(Visitor &v)
{
    return v.visit_break(this);
}

u8 ReturnStmt::visit(Visitor &v)
{
    return v.visit_return(this);
}

u8 SwitchStmt::visit(Visitor &v)
{
    return v.visit_switch(this);
}

u8 DoStmt::visit(Visitor &v)
{
    return v.visit_do(this);
}

u8 FunctionStmt::visit(Visitor &v)
{
    return v.visit_function(this);
}

std::string Stmt::toString()
{
      switch (type)
      {
    
        case StmtType::BLOCK: return "block";
        case StmtType::EXPRESSION: return "expression";
        case StmtType::DECLARATION: return "declaration";
        case StmtType::IF: return "if";
        case StmtType::WHILE: return "while";
        case StmtType::FOR: return "for";
        case StmtType::DO: return "do";
        case StmtType::SWITCH: return "switch";
        case StmtType::RETURN: return "return";
        case StmtType::BREAK: return "break";
        case StmtType::CONTINUE: return "continue";
        case StmtType::PRINT: return "print";
        case StmtType::FUNCTION: return "def";
        case StmtType::PROGRAM: return "program";
        case StmtType::S_NONE: return "none";

        default: return "Unknown";
      }

      return "Unknown";

}

u8 StructStmt::visit(Visitor &v)
{
    return v.visit_struct(this);
}

u8 ClassStmt::visit(Visitor &v)
{
    return v.visit_class(this);
}

u8 ArrayStmt::visit(Visitor &v)
{ 
    return v.visit_array(this);
}

u8 MapStmt::visit(Visitor &v)
{
    return v.visit_map(this);
}

u8 FromStmt::visit(Visitor &v)
{
    return v.visit_from(this);
}
