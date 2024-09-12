
#include "pch.h" 
#include "Expr.hpp" 
#include "Interpreter.hpp"
#include "Utils.hpp"


ExprPtr EmptyExpr::accept(Visitor &v)
{
    return v.visit_empty_expression(this);
}

ExprPtr EmptyExpr::clone()
{
    return std::make_shared<EmptyExpr>();
}

ExprPtr BinaryExpr::accept(Visitor &v)
{
    return v.visit_binary(this);
}



ExprPtr UnaryExpr::accept(Visitor &v)
{
    return v.visit_unary(this);
}



ExprPtr GroupingExpr::accept(Visitor &v)
{
    return v.visit_grouping(this);
}



ExprPtr LogicalExpr::accept(Visitor &v)
{
    return v.visit_logical(this);
}



ExprPtr NumberLiteral::accept(Visitor &v)
{
    return v.visit_number_literal(this);
}

void NumberLiteral::print()
{
    PRINT("%f", value);
}

ExprPtr NumberLiteral::clone()
{
    std::shared_ptr<NumberLiteral> expr = std::make_shared<NumberLiteral>();
    expr->value = value;
    return expr;
}

ExprPtr StringLiteral::accept(Visitor &v)
{
    return  v.visit_string_literal(this);
}

void StringLiteral::print()
{
    PRINT("%s", value.c_str());
}

ExprPtr StringLiteral::clone()
{
    std::shared_ptr<StringLiteral> expr = std::make_shared<StringLiteral>();
    expr->value = value;
    return expr;
}

ExprPtr NowExpr::accept(Visitor &v)
{
    return v.visit_now_expression(this);
}

ExprPtr Variable::accept(Visitor &v)
{
    return  v.visit_read_variable(this);
}

ExprPtr Assign::accept(Visitor &v)
{
    return  v.visit_assign(this);
}

ExprPtr Literal::accept(Visitor &v)
{
    return  v.visit_literal(this);
}

void Literal::print()
{
    PRINT("nil");
}

ExprPtr Literal::clone()
{
    return std::make_shared<Literal>();
}

std::string Expr::toString()
{
   switch (type)
   {
       case ExprType::E_NONE: return "NONE";
       case ExprType::L_NUMBER: return "NUMBER";
       case ExprType::L_STRING: return "STRING";
       case ExprType::L_FUNCTION: return "FUNCTION";
       case ExprType::L_CLASS: return "CLASS";
       case ExprType::L_STRUCT: return "STRUCT";
       case ExprType::L_ARRAY: return "ARRAY";
       case ExprType::L_MAP: return "MAP";
       case ExprType::L_NATIVE: return "NATIVE";
       case ExprType::LITERAL: return "LITERAL";
       case ExprType::BINARY: return "BINARY";
       case ExprType::UNARY: return "UNARY";
       case ExprType::GROUPING: return "GROUPING";
       case ExprType::LOGICAL: return "LOGICAL";
       case ExprType::NOW: return "NOW";
       case ExprType::VARIABLE: return "VARIABLE";
       case ExprType::EMPTY_EXPR: return "EMPTY_EXPR";
       case ExprType::ASSIGN: return "ASSIGN";
       case ExprType::CALL: return "CALL";
       case ExprType::GET: return "GET";
       case ExprType::SET: return "SET";
       case ExprType::GET_DEF: return "GET_DEF";
       
       default: return "UNKNOWN";
       
   }

   return "UNKNOWN";
}

ExprPtr CallExpr::accept(Visitor &v)
{
    return v.visit_call(this);
}

ExprPtr GetExpr::accept(Visitor &v)
{
    return v.visit_get(this);
}

ExprPtr SetExpr::accept(Visitor &v)
{
    return v.visit_set(this);
}

ExprPtr GetDefinitionExpr::accept(Visitor &v)
{
    return v.visit_get_definition(this);
}

ExprPtr SelfExpr::accept(Visitor &v)
{
    return  v.visit_self(this);
}

ExprPtr SuperExpr::accept(Visitor &v)
{
    return  v.visit_super(this);
}
