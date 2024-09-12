#pragma once
#include "Config.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Arena.hpp"

class Interpreter;
class Context;

typedef ExprPtr (*NativeFunction)(Context *ctx, int argc);
typedef struct
{
    const char *name;
    NativeFunction func;
} NativeFuncDef;


struct Function : public Literal
{
    std::string args[32];
    u32 arity;
    Token name;
    StmtPtr body;
    Function();
    ExprPtr clone() override;

 

};

struct Native : public Literal
{
    Token name;
    Native();
};

struct ClassLiteral : public Literal
{
    std::string name;
    std::string parentName;
    
    bool isChild;
    Environment *environment;

    
    ClassLiteral();
    virtual ~ClassLiteral();
    void print();
    ExprPtr clone() override;

};



struct StructLiteral : public Literal
{
    std::string name;
    std::unordered_map<std::string, ExprPtr> members;

    StructLiteral();
    virtual ~StructLiteral();
    void print();
    ExprPtr clone() override;
};

struct ArrayLiteral : public Literal
{
    std::vector<ExprPtr> values;
    std::string name;
    ArrayLiteral();
    void print() override;

    ExprPtr clone() override;
};

struct MapLiteral : public Literal
{
    std::map<ExprPtr, ExprPtr> values;
    std::string name;
    MapLiteral();
    void print() override;
    ExprPtr clone() override;
};


struct Visitor
{
    virtual ~Visitor() {}
    virtual ExprPtr visit(ExprPtr node) = 0;

    virtual ExprPtr visit_empty_expression(EmptyExpr *node) = 0;
    virtual ExprPtr visit_binary(BinaryExpr *node) = 0;
    virtual ExprPtr visit_unary(UnaryExpr *node) = 0;
    virtual ExprPtr visit_logical(LogicalExpr *node) = 0;
    virtual ExprPtr visit_grouping(GroupingExpr *node) = 0;
    virtual ExprPtr visit_literal(Literal *node) = 0;
    virtual ExprPtr visit_number_literal(NumberLiteral *node) = 0;
    virtual ExprPtr visit_string_literal(StringLiteral *node) = 0;
    virtual ExprPtr visit_now_expression(NowExpr *node) = 0;
    virtual ExprPtr visit_read_variable(Variable *node) = 0;
    virtual ExprPtr visit_assign(Assign *node) = 0;
    virtual ExprPtr visit_call(CallExpr *node) = 0;
    virtual ExprPtr visit_get(GetExpr *node) = 0;
    virtual ExprPtr visit_get_definition(GetDefinitionExpr *node) = 0;
    virtual ExprPtr visit_set(SetExpr *node) = 0;
    virtual ExprPtr visit_self(SelfExpr *node) = 0;
    virtual ExprPtr visit_super(SuperExpr *node) = 0;



    virtual u8 execute(Stmt *stmt) = 0;
    virtual u8 visit_block_smt(BlockStmt *node) = 0;
    virtual u8 visit_expression_smt(ExpressionStmt *node) = 0;
    virtual u8 visit_print_smt(PrintStmt *node) = 0;
    virtual u8 visit_declaration(Declaration *node) = 0;
    virtual u8 visit_if(IFStmt *node) = 0;
    virtual u8 visit_while(WhileStmt *node) = 0;
    virtual u8 visit_do(DoStmt *node) = 0;
    virtual u8 visit_program(Program *node) = 0;
    virtual u8 visit_function(FunctionStmt *node) = 0;
    virtual u8 visit_for(ForStmt *node) = 0;
    virtual u8 visit_from(FromStmt *node) = 0;
    virtual u8 visit_return(ReturnStmt *node) = 0;
    virtual u8 visit_break(BreakStmt *node) = 0;
    virtual u8 visit_switch(SwitchStmt *node) = 0;
    virtual u8 visit_continue(ContinueStmt *node) = 0;
    virtual u8 visit_struct(StructStmt *node) = 0;
    virtual u8 visit_class(ClassStmt *node) = 0;
    virtual u8 visit_array(ArrayStmt *node) = 0;
    virtual u8 visit_map(MapStmt *node) = 0;
};

class Environment
{

private:
    friend class Factory;
    friend class Interpreter;

    Environment *parent;
    u32 depth;
    std::unordered_map<std::string, ExprPtr> m_values;

public:
    Environment();
    Environment(Environment *parent);
    virtual ~Environment();



    void print();

    bool define(const std::string &name, ExprPtr value);
    ExprPtr get(const std::string &name);
    bool set(const std::string &name, ExprPtr value);

    bool empty() { return m_values.empty(); }
    bool size() { return m_values.size(); }

    bool contains(const std::string &name);
    void remove(const std::string &name); 

    bool assign(const std::string &name, ExprPtr value);
    bool replace(const std::string &name, ExprPtr value);

    bool addInteger(const std::string &name, int value);
    bool addDouble(const std::string &name, double value);
    bool addString(const std::string &name, std::string value);
    bool addBoolean(const std::string &name, bool value);


    bool copy(std::unordered_map<std::string, ExprPtr> values);
    bool copy(Environment *environment); 

    std::shared_ptr<Environment> clone();

    std::unordered_map<std::string, ExprPtr> values() { return m_values; }
    std::unordered_map<std::string, ExprPtr> values() const { return m_values; }

    void setParent(Environment *p) { parent = p; }
    Environment* getParent() { return parent; }

    u32 getDepth() const { return depth; }
};

class Context
{
public:

    Context(Interpreter *interpreter);
    ~Context();
    
    long getLong(u8 index);
    int getInt(u8 index);
    double getDouble(u8 index);
    float getFloat(u8 index);
    std::string getString(u8 index);
    bool getBoolean(u8 index);

    ExprPtr asFloat(float value);
    ExprPtr asDouble(double value);
    ExprPtr asInt(int value);
    ExprPtr asLong(long value);
    ExprPtr asString(std::string value);
    ExprPtr asBoolean(bool value);
    ExprPtr asNil();

    bool isNumber(u8 index);
    bool isString(u8 index);

private:
    friend class Interpreter;
    friend class Compiler;


    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

    void add(ExprPtr value, Literal *literal);

    std::vector<ExprPtr> values;
    std::vector<Literal *> literals;
    std::vector<ExprPtr>   expressions;
    Interpreter *interpreter;
    ExprPtr NIL;
    

    void clear();
};







struct Compiler : public Visitor
{

    ExprPtr visit(ExprPtr node) override;
    ExprPtr visit_empty_expression(EmptyExpr *node) override;
    ExprPtr visit_binary(BinaryExpr *node) override;
    ExprPtr visit_unary(UnaryExpr *node) override;
    ExprPtr visit_logical(LogicalExpr *node) override;
    ExprPtr visit_grouping(GroupingExpr *node) override;
    ExprPtr visit_literal(Literal *node) override;
    ExprPtr visit_number_literal(NumberLiteral *node) override;
    ExprPtr visit_string_literal(StringLiteral *node) override;
    ExprPtr visit_now_expression(NowExpr *node) override;
    ExprPtr visit_read_variable(Variable *node) override;//read
    ExprPtr visit_assign(Assign *node) override;
    ExprPtr evaluate(ExprPtr node);
    ExprPtr visit_call(CallExpr *node) override;
    ExprPtr visit_get(GetExpr *node) override;
    ExprPtr visit_self(SelfExpr *node) override;
    ExprPtr visit_super(SuperExpr *node) override;
    


    ExprPtr visit_get_definition(GetDefinitionExpr *node) override;
    ExprPtr visit_set(SetExpr *node) override;
    ExprPtr visit_call_native(CallExpr *node);
    ExprPtr visit_call_struct(const ExprPtr &var,CallExpr *node, Expr *expr);
    ExprPtr visit_call_function(CallExpr *node, Expr *expr);
    ExprPtr visit_call_class(const ExprPtr &var,CallExpr *node, Expr *expr);

    u8 execute(Stmt *stmt);

    u8 visit_block_smt(BlockStmt *node) override;
    u8 visit_expression_smt(ExpressionStmt *node) override;
    u8 visit_print_smt(PrintStmt *node) override;
    u8 visit_declaration(Declaration *node) override;
    u8 visit_if(IFStmt *node) override;
    u8 visit_while(WhileStmt *node) override;
    u8 visit_for(ForStmt *node) override;
    u8 visit_from(FromStmt *node) override;
    u8 visit_do(DoStmt *node);
    u8 visit_return(ReturnStmt *node) override;
    u8 visit_break(BreakStmt *node) override;
    u8 visit_continue(ContinueStmt *node) override;
    u8 visit_switch(SwitchStmt *node) override;
    u8 visit_program(Program *node) override;
    u8 visit_function(FunctionStmt *node) override;

    u8 visit_struct(StructStmt *node) override;
    u8 visit_class(ClassStmt *node) override;

    u8 visit_array(ArrayStmt *node) override;
    u8 visit_map(MapStmt *node) override;

    ExprPtr ProcessString(Expr *var, GetDefinitionExpr *node);
    ExprPtr ProcessArray(Expr *var, GetDefinitionExpr *node);
    ExprPtr ProcessMap(Expr *var, GetDefinitionExpr *node);
    ExprPtr ProcessClass(Expr *var, GetDefinitionExpr *node);
    ExprPtr visit_call_function_member(CallExpr *node, Expr *callee, ClassLiteral *main);

    u8 execte_block(BlockStmt *node, Environment *env);

    Compiler(Interpreter *i, Compiler *parent = nullptr);
    ~Compiler();

private:
    friend class Interpreter;
    Interpreter *interpreter;
    Environment *environment;
    std::shared_ptr<Environment> global;
    Environment *prefEnv;
    Compiler *parent;
    u32 loop_count = 0;
    std::stack<Environment *> locals;

    std::shared_ptr<ClassLiteral> instance;


    void pop_local();
    void push_local(Environment *local);


    void clear();
    void init();
};

class Interpreter
{

public:
    ~Interpreter();
    Interpreter();

    bool compile(const std::string &source);

    void clear();

    void collect();

    void registerFunction(const std::string &name, NativeFunction function);
    bool registerInteger(const std::string &name, int value);
    bool registerBoolean(const std::string &name, bool value);
    bool registerDouble(const std::string &name, double value);
    bool registerString(const std::string &name, std::string value);
    

    bool isnative(const std::string &name);

private:
    friend class Compiler;
    friend class Context;

    Compiler *compiler;
    Context *context;

    std::shared_ptr<Compiler> currentCompiler;
    std::shared_ptr<Context> currentContext;

   std::unordered_map<std::string, NativeFunction> nativeFunctions;


    ExprPtr CallNativeFunction(const std::string &name, int argc);
    bool registerGlobal(const std::string &name, ExprPtr value);
    

    void Error(const Token &token, const std::string &message);
    void Error(const std::string &message);
    void Warning(const Token &token, const std::string &message);
    void Warning(const std::string &message);
    void Info(const std::string &message);
};
