#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

class Parser
{
public:
    Parser();
    ~Parser();

    void Load( std::vector<Token> tokens);

        std::shared_ptr<Program>  parse();

    void clear();

private:


    std::vector<Token> tokens;
    int current;
    bool panicMode;
    int countBegins;
    int countEnds ;




    bool match(const std::vector<TokenType> &types);
    bool match(TokenType type);
    Token consume(TokenType type, const std::string &message);
    bool check(TokenType type);
    bool isAtEnd();
    void synchronize();
    Token advance();
    Token peek();
    Token previous();
    Token lookAhead();

    void Error(const Token &token,const std::string &message);
    void Error(const std::string &message);
    void Warning(const Token &token, const std::string &message);


    ExprPtr expression();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr assignment();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();  
    ExprPtr primary(); 
    ExprPtr logical_or();
    ExprPtr logical_and();
    ExprPtr logical_xor();
    ExprPtr self_expr();
    ExprPtr super_expr();

    ExprPtr call();
    ExprPtr function_call(ExprPtr callee,  Token name );

    void freecalls();


    ExprPtr now();

    std::shared_ptr<Program> program();

    StmtPtr expression_statement();
    StmtPtr variable_declaration(bool inIntern=false);
    StmtPtr function_declaration();
    StmtPtr print_statement();
    StmtPtr statement();
    StmtPtr declarations();
    StmtPtr block();

    StmtPtr if_statement();
    StmtPtr while_statement();
    StmtPtr do_statement();
    StmtPtr for_statement();
    StmtPtr from_statement();
    StmtPtr return_statement();
    StmtPtr break_statement();
    StmtPtr continue_statement();
    StmtPtr switch_statement();
    StmtPtr class_declaration();
    StmtPtr struct_declaration();


};