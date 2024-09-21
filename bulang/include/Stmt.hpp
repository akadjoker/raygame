#pragma once 
#include <vector>
#include <memory>
#include "Config.hpp"

#include "Token.hpp"


struct Visitor;
struct Expr;
class Environment;
using ExprPtr = std::shared_ptr<Expr>;

enum StmtType
{
    S_NONE = 0,
    BLOCK,
    EXPRESSION,
    DECLARATION,
    IF,
    WHILE,
    LOOP,
    FOR,
    FROM,
    DO,
    SWITCH,
    RETURN,
    BREAK,
    CONTINUE,
    PRINT,
    FUNCTION,
    STRUCT,
    CLASS,
    ARRAY,
    MAP,
    IMPORT,
    PROGRAM,
    S_COUNT,
};

class Stmt
{
public:
    Stmt()  {}
    virtual ~Stmt() {}
    virtual u8 visit( Visitor &v) = 0;

    StmtType type{StmtType::S_NONE};

    std::string toString();
};

using StmtPtr =  std::shared_ptr<Stmt>;

class BlockStmt : public Stmt
{
public:
    BlockStmt() : Stmt() { type = StmtType::BLOCK; }



    u8 visit( Visitor &v) override;

    std::vector<StmtPtr> statements;
};


class ExpressionStmt : public Stmt
{
public:
    ExpressionStmt() : Stmt() { type = StmtType::EXPRESSION; }

    u8 visit( Visitor &v) override;

    ExprPtr expression;
};


struct ElifStmt
{
    ExprPtr condition;
    StmtPtr then_branch;
};

class IFStmt : public Stmt
{
public:
    IFStmt();

    u8 visit( Visitor &v) override;

    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;

    std::vector<std::shared_ptr<ElifStmt>> elifBranch;

};

struct CaseStmt 
{
    ExprPtr condition;
    StmtPtr body;
};

class SwitchStmt : public Stmt
{
public:
    SwitchStmt() : Stmt() { type = StmtType::SWITCH; }

    u8 visit( Visitor &v) override;

    ExprPtr condition;
    std::vector<std::shared_ptr<CaseStmt>> cases;
    StmtPtr defaultBranch;
};

class WhileStmt : public Stmt
{
public:
    WhileStmt() : Stmt() { type = StmtType::WHILE; }

    u8 visit( Visitor &v) override;

    ExprPtr condition;
    StmtPtr body;

};

class LoopStmt : public Stmt
{
public:
    LoopStmt() : Stmt() { type = StmtType::LOOP; }

    u8 visit( Visitor &v) override;

    StmtPtr body;
};

class DoStmt : public Stmt
{
public:
    DoStmt() : Stmt() { type = StmtType::DO; }

    u8 visit( Visitor &v) override;

    ExprPtr condition;
    StmtPtr body;
};


class ForStmt : public Stmt
{
public:
    ForStmt() : Stmt() { type = StmtType::FOR; }

    u8 visit( Visitor &v) override;

    StmtPtr initializer;
    ExprPtr condition;
    ExprPtr increment;
    StmtPtr body;

};


class FromStmt : public Stmt
{
public:
    FromStmt() : Stmt() { type = StmtType::FROM; }
    u8 visit( Visitor &v) override;
    StmtPtr variable;
    ExprPtr array;
    StmtPtr body;
};


class PrintStmt : public Stmt
{
public:
    PrintStmt() : Stmt() { type = StmtType::PRINT; }
    u8 visit( Visitor &v) override;

    ExprPtr expression;
};

class Declaration : public Stmt
{
public:
    Declaration() : Stmt() { type = StmtType::DECLARATION; }
    u8 visit( Visitor &v) override;
    std::vector<Token> names;
    ExprPtr initializer;
};

class ReturnStmt : public Stmt
{
public:
    ReturnStmt() : Stmt() { type = StmtType::RETURN; }
    u8 visit( Visitor &v) override;

    ExprPtr value;

};


class BreakStmt : public Stmt
{
public:
    BreakStmt() : Stmt() { type = StmtType::BREAK; }
    u8 visit( Visitor &v) override;
};


class ContinueStmt : public Stmt
{
public:
    ContinueStmt() : Stmt() { type = StmtType::CONTINUE; }
    u8 visit( Visitor &v) override;
};


class FunctionStmt : public Stmt
{
public:
    FunctionStmt() : Stmt() { type = StmtType::FUNCTION; }
    u8 visit( Visitor &v) override;
    std::vector<std::string> args;
    Token name;
    StmtPtr body;

};

class StructStmt : public Stmt
{
public:
    StructStmt() : Stmt() { type = StmtType::STRUCT; }
    u8 visit( Visitor &v) override;
    
    
    std::vector<StmtPtr> values;
   
    
    Token name;
};

class ClassStmt : public Stmt
{
public:
    ClassStmt() : Stmt() { type = StmtType::CLASS; }
    u8 visit( Visitor &v) override;
    std::vector<StmtPtr> fields;
    std::vector<StmtPtr> methods;
    ExprPtr superClass;
    Token name;
};


class ArrayStmt : public Stmt
{
public:
    ArrayStmt() : Stmt() { type = StmtType::ARRAY; }
    u8 visit( Visitor &v) override;
    std::vector<ExprPtr> values;
    Token name;
};

class MapStmt : public Stmt
{
public:
    MapStmt() : Stmt() { type = StmtType::MAP; }
    u8 visit( Visitor &v) override;
    std::unordered_map<ExprPtr, ExprPtr> values;
    Token name;
};




class Program : public Stmt
{
public:
    Program() : Stmt() { type = StmtType::PROGRAM; }

    u8 visit( Visitor &v) override;

    std::vector<StmtPtr> statements;
};

class Importer : public Stmt
{
public:
    Importer() : Stmt() { type = StmtType::IMPORT; }
    u8 visit( Visitor &v) override;
    Token name;
  //  std::shared_ptr<Program> program;
};