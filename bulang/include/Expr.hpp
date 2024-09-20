#pragma once 
#include "Token.hpp"
#include <memory>
#include "Utils.hpp"

struct Visitor;

enum ExprType
{
    E_NONE,
    EMPTY_EXPR,
    BINARY,
    UNARY,
    GROUPING,
    LITERAL,
    L_NUMBER,
    L_STRING,
    L_FUNCTION,
    L_NATIVE,
    L_CLASS,
    L_STRUCT,
    L_ARRAY,
    L_MAP,
    GET,
    GET_DEF,
    SET,
    SELF,
    SUPER,
    VARIABLE,
    ASSIGN,
    LOGICAL,
    CALL,
    NOW,
    E_COUNT
};

class Expr;
using ExprPtr =  std::shared_ptr<Expr>;

class Expr
{

public:
    Expr() {  }
    virtual ~Expr() {}

    virtual ExprPtr accept( Visitor &v) = 0;

    ExprType type{ExprType::E_NONE};

    std::string toString();

    virtual std::size_t hash()  const { return 0; }

    virtual void print() {};
    virtual ExprPtr clone() { return nullptr; }

};





class EmptyExpr : public Expr
{
public:
    EmptyExpr() : Expr() { type = ExprType::EMPTY_EXPR; }

    ExprPtr accept( Visitor &v) override;
    ExprPtr clone() override;
};


class BinaryExpr : public Expr
{
public:
    BinaryExpr() : Expr() { type = ExprType::BINARY; }
   // ~BinaryExpr() { INFO("delete BinaryExpr()"); }

    ExprPtr accept( Visitor &v) override;

    ExprPtr left;
    ExprPtr right;
    Token op;
};


class UnaryExpr : public Expr
{
public:
    UnaryExpr() : Expr() { type = ExprType::UNARY; }
  //  ~UnaryExpr() { INFO("delete UnaryExpr()"); }

    ExprPtr accept( Visitor &v) override;

    ExprPtr right;
    Token op;
    bool isPrefix;
};

class GroupingExpr : public Expr
{
public:
    GroupingExpr() : Expr() { type = ExprType::GROUPING; }
 //   ~GroupingExpr() { INFO("delete GroupingExpr()"); }

    ExprPtr accept( Visitor &v) override;

    ExprPtr expr;
};

class LogicalExpr : public Expr
{
public:
    LogicalExpr() : Expr() { type = ExprType::LOGICAL; }
   // ~LogicalExpr() { INFO("delete LogicalExpr()"); }

    ExprPtr accept( Visitor &v) override;

    ExprPtr left;
    ExprPtr right;
    Token op;
};



class Literal : public Expr
{


public:
    Literal() : Expr() { type = ExprType::LITERAL;  }

    ExprPtr accept( Visitor &v) override;
 //   virtual ~Literal() { INFO("delete Literal()"); }
    virtual void print() override;

    ExprPtr clone() override;
 
};

class NumberLiteral : public Literal
{
public:
    NumberLiteral() : Literal() { type = ExprType::L_NUMBER; }
   // virtual ~NumberLiteral() { INFO("delete NumberLiteral(%f) ",value); }



    bool operator==(const NumberLiteral& outra) const 
    {
        return value == outra.value;
    }

    std::size_t hash() const override 
    {
        return std::hash<double>()(value);
    }

    ExprPtr accept( Visitor &v) override;

    void print() override;

    ExprPtr clone() override;

    double value;
};




class StringLiteral : public Literal
{
public:
    StringLiteral() : Literal() { type = ExprType::L_STRING; }

   // virtual ~StringLiteral() { INFO("delete StringLiteral(%s) ",value.c_str()); }
 
    ExprPtr accept( Visitor &v) override;

    void print() override;

    ExprPtr clone() override;

    bool operator==(const StringLiteral& outra) const 
    {
        return value == outra.value;
    }
    std::size_t hash() const override 
    {
        return std::hash<std::string>()(value);
    }

    std::string value;
};

class NowExpr : public Expr
{
public:
    NowExpr() : Expr() { type = ExprType::NOW; }
    ExprPtr accept( Visitor &v) override;

};




class Variable : public Expr
{
public:
    Variable() : Expr() { type = ExprType::VARIABLE; }
//    ~Variable() { INFO("delete Variable()"); }
    
    ExprPtr accept( Visitor &v) override;


    Token name;
};

class Assign : public Expr
{
public:
    Assign() : Expr() { type = ExprType::ASSIGN; }
    ExprPtr accept( Visitor &v) override;
  //  ~Assign() { INFO("delete Assign()"); }

    Token name;   
    ExprPtr value;
};

class CallExpr : public Expr
{
public:
    CallExpr() : Expr() { type = ExprType::CALL; }
 //   ~CallExpr() { INFO("delete CallExpr()"); }
    ExprPtr accept( Visitor &v) override;
    Token name;
    ExprPtr callee;
    std::vector<ExprPtr> args;

};


class GetExpr : public Expr
{
public:
    GetExpr() : Expr() { type = ExprType::GET; }
 //   ~GetExpr() { INFO("delete GetExpr()"); }
    ExprPtr accept( Visitor &v) override;
    Token name;
    ExprPtr object;

};

class GetDefinitionExpr : public Expr
{
public:
    GetDefinitionExpr() : Expr() { type = ExprType::GET_DEF; }
   // ~GetDefinitionExpr() { INFO("delete GetDefinitionExpr()"); }
    ExprPtr accept( Visitor &v) override;
    Token name;
    ExprPtr variable;
    std::vector<ExprPtr> values;
};


class SetExpr : public Expr
{
public:
    SetExpr() : Expr() { type = ExprType::SET; }
   // ~SetExpr() { INFO("delete SetExpr()"); }
    ExprPtr accept( Visitor &v) override;
    Token name;
    ExprPtr object;
    ExprPtr value;
};

class SelfExpr : public Expr
{
public:
    SelfExpr() : Expr() { type = ExprType::SELF; }
    ExprPtr accept( Visitor &v) override;
};

class SuperExpr : public Expr
{
public:
    SuperExpr() : Expr() { type = ExprType::SUPER; }
    ExprPtr accept( Visitor &v) override;
};