#include "pch.h"
#include "Parser.hpp"
#include "Utils.hpp"






Parser::Parser()
{

}

    
void Parser::Load( std::vector<Token> tokens)
{
this->tokens = tokens;
current = 0;
panicMode = false;
countBegins = 0;
countEnds = 0;
}

Parser::~Parser()
{
   
}

void Parser::clear()
{
    freecalls();

    tokens.clear();
    current = 0;
    panicMode = false;
    countBegins = 0;
    countEnds = 0;
}

bool Parser::match(const std::vector<TokenType> &types)
{
    for (auto type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

Token Parser::consume(TokenType type,const std::string &message)
{
    if (check(type))
    {
      return   advance();
    }
    else
    {
        Error(tokens[current],message+" have '"+tokens[current].lexeme+"'");
        return peek();
    }
}

bool Parser::check(TokenType type)
{
    if (isAtEnd()) return false;
    return tokens[current].type == type;
}

bool Parser::isAtEnd()
{
    return tokens[current].type == TokenType::END_OF_FILE;
}

void Parser::synchronize()
{

    advance();
    while(!isAtEnd())
    {
        if (tokens[current].type == TokenType::SEMICOLON) return;

        switch (tokens[current].type)
        {
            case TokenType::CLASS:
            case TokenType::FUNCTION:
            case TokenType::STRUCT:
            case TokenType::VAR:
            case TokenType::IDENTIFIER:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::NOW:
            case TokenType::SWITCH:
            case TokenType::CASE:
                break;

            default:
                advance();
            }

        advance();
    }
}

Token Parser::advance()
{
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

Token Parser::lookAhead()
{
    if (current + 1 >= (int)tokens.size()) return previous();
    return tokens[current + 1];
}



void Parser::Error(const Token &token,const std::string &message)
{
    panicMode = true;
    int line = token.line;
    std::string text =message+ " at line: " +std::to_string(line);
    Log(2, text.c_str());
   // synchronize();

    throw FatalException(text);
}
void Parser::Error(const std::string &message)
{
    panicMode = true;
    Log(2, message.c_str());
  //  synchronize();

    throw FatalException(message);
}
void Parser::Warning(const Token &token,const std::string &message)
{
    int line = token.line;
    std::string text =message+ " at line: " +std::to_string(line);
    Log(1, text.c_str());
}

//******************************************************************************************************************* */

ExprPtr Parser::expression()
{
    return assignment();
}

ExprPtr Parser::assignment()
{
    ExprPtr expr = logical_or();
    Token token = previous();

   


    if (match(TokenType::EQUAL) )
    {
        Token op = previous();
        ExprPtr value = assignment();

        if (expr->type == ExprType::VARIABLE)
        {
            Variable *var = (Variable *)expr.get();
           
            std::shared_ptr<Assign> assign = std::make_shared<Assign>();
           assign->name = var->name;
           assign->value = value;
           expr = assign;
           return assign;
        } else if (expr->type == ExprType::GET)
        {
            GetExpr *get = (GetExpr *)expr.get();
            std::shared_ptr<SetExpr> set = std::make_shared<SetExpr>();
            set->name  = get->name;
            set->object = get->object;
            set->value = value;
           
           return set;
        }
        else
        {
            Error("Invalid assignment target.");
        }
    }  else     if (match(TokenType::PLUS_EQUAL))
    {
       
        std::shared_ptr<Expr> value = assignment();
        if (expr->type == ExprType::VARIABLE)
        {
            Variable *var = (Variable *)expr.get();
            std::shared_ptr<Assign> assign = std::make_shared<Assign>();
            assign->name = var->name;


            std::shared_ptr<BinaryExpr> addition =  std::make_shared<BinaryExpr>();
            addition->left  = expr;
            addition->right = value;
            addition->op = Token(TokenType::PLUS_EQUAL, token.lexeme,token.literal, token.line);

            assign->value = std::move(addition);         


           return assign;
         } else if (expr->type == ExprType::GET)
        {
            
             GetExpr *get = (GetExpr *)expr.get(); //get value
             std::shared_ptr<SetExpr> set = std::make_shared<SetExpr>(); //value to set
             set->name   = get->name;
             set->object = get->object;
             std::shared_ptr<BinaryExpr> addition =  std::make_shared<BinaryExpr>();// expresion to add
             addition->left  = expr;
             addition->right = value;
             addition->op = Token(TokenType::PLUS_EQUAL, token.lexeme, token.literal, token.line);
             set->value  = addition;
             return set;
      } 
        else 
        {
            Error("Invalid binary '+=' target.");
        }
    } else     if (match(TokenType::MINUS_EQUAL))
    {
        std::shared_ptr<Expr> value = assignment();
        if (expr->type == ExprType::VARIABLE)
        {
            Variable *var = (Variable *)expr.get();
            std::shared_ptr<Assign> assign = std::make_shared<Assign>();
            assign->name = var->name;
            std::shared_ptr<BinaryExpr> addition =  std::make_shared<BinaryExpr>();
            addition->left  = expr;
            addition->right = value;
            addition->op = Token(TokenType::MINUS_EQUAL, token.lexeme,token.literal, token.line);
            assign->value = addition;         
            return assign;
        }  else if (expr->type == ExprType::GET)
        {

             GetExpr *get = (GetExpr *)expr.get(); 
             std::shared_ptr<SetExpr> set = std::make_shared<SetExpr>(); 
             set->name   = get->name;
             set->object = get->object;
             std::shared_ptr<BinaryExpr> subtract =  std::make_shared<BinaryExpr>();
             subtract->left  = expr;
             subtract->right = value;
             subtract->op = Token(TokenType::MINUS_EQUAL, token.lexeme, token.literal, token.line);
             set->value  = subtract;
             return set;
             
        } else  
        {
            Error("Invalid binary '-=' target.");
        }
    } else     if (match(TokenType::STAR_EQUAL))
    {
        std::shared_ptr<Expr> value = assignment();
        if (expr->type == ExprType::VARIABLE)
        {
            Variable *var = (Variable *)expr.get();
            std::shared_ptr<Assign> assign =  std::make_shared<Assign>();
            assign->name = var->name;
            std::shared_ptr<BinaryExpr> addition =  std::make_shared<BinaryExpr>();
            addition->left  = expr;
            addition->right = value;
            addition->op = Token(TokenType::STAR_EQUAL, token.lexeme,token.literal, token.line);
            assign->value = addition;         
           return assign;
        }   else if (expr->type == ExprType::GET)
        {
             GetExpr *get = (GetExpr *)expr.get(); 
             std::shared_ptr<SetExpr> set = std::make_shared<SetExpr>(); 
             set->name   = get->name;
             set->object = get->object;
             std::shared_ptr<BinaryExpr> subtract =  std::make_shared<BinaryExpr>();
             subtract->left  = expr;
             subtract->right = value;
             subtract->op = Token(TokenType::STAR_EQUAL, token.lexeme, token.literal, token.line);
             set->value  = subtract;
             return set;
            
        } else 
        {
            Error("Invalid binary '*='  target. "+ expr->toString());
        }
    } else     if (match(TokenType::SLASH_EQUAL))
    {
       std::shared_ptr<Expr> value = assignment();
        if (expr->type == ExprType::VARIABLE)
        {
            Variable *var = (Variable *)expr.get();
            std::shared_ptr<Assign> assign =  std::make_shared<Assign>();
            assign->name = var->name;
            std::shared_ptr<BinaryExpr> addition =  std::make_shared<BinaryExpr>();
            addition->left  = expr;
            addition->right = value;
            addition->op = Token(TokenType::SLASH_EQUAL, token.lexeme,token.literal, token.line);
            assign->value = addition;         
           return assign;
         } else if (expr->type == ExprType::GET)
        {

             GetExpr *get = (GetExpr *)expr.get();
             std::shared_ptr<SetExpr> set = std::make_shared<SetExpr>();
             set->name   = get->name;
             set->object = get->object;
             std::shared_ptr<BinaryExpr> div =  std::make_shared<BinaryExpr>();
             div->left  = expr;
             div->right = value;
             div->op = Token(TokenType::SLASH_EQUAL, token.lexeme, token.literal, token.line);
             set->value  = div;
             return set;
           
        } else 
        {
            Error("Invalid binary '/=' target.");
        }
    }

    return expr;
}
std::shared_ptr<Expr> Parser::logical_or()
{
    std::shared_ptr<Expr> expr = logical_and();
    while (match({TokenType::OR}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = logical_and();
        std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<LogicalExpr>();
        
        ((LogicalExpr *)expr.get())->left  = left;
        ((LogicalExpr *)expr.get())->right = right;
        ((LogicalExpr *)expr.get())->op = op;
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_and()
{
    std::shared_ptr<Expr> expr = logical_xor();
    while (match({TokenType::AND}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = logical_xor();
        std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<LogicalExpr>();
        ((LogicalExpr *)expr.get())->left  = left;
        ((LogicalExpr *)expr.get())->right = right;
        ((LogicalExpr *)expr.get())->op = op;
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_xor()
{
    std::shared_ptr<Expr> expr = equality();
    while (match({TokenType::XOR}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = equality();
        std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<LogicalExpr>();
        ((LogicalExpr *)expr.get())->left  = left;
        ((LogicalExpr *)expr.get())->right = right;
        ((LogicalExpr *)expr.get())->op = op;
    }
    return expr;

}

ExprPtr Parser::self_expr()
{
    return std::make_shared<SelfExpr>();
}

ExprPtr Parser::super_expr()
{
    return std::make_shared<SuperExpr>();
}

std::shared_ptr<Expr> Parser::equality()
{
    std::shared_ptr<Expr> expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<BinaryExpr>();
        ((BinaryExpr *)expr.get())->left  = left;
        ((BinaryExpr *)expr.get())->right = right;
        ((BinaryExpr *)expr.get())->op = op;
    
    }
    return expr;
}

    /// comparison     term ((GREATER | LESS | GREATER_EQUAL | LESS_EQUAL) term)*
std::shared_ptr<Expr> Parser::comparison()
{
    std::shared_ptr<Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::LESS, TokenType::GREATER_EQUAL, TokenType::LESS_EQUAL}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<BinaryExpr>();
        ((BinaryExpr *)expr.get())->left  = left;
        ((BinaryExpr *)expr.get())->right = right;
        ((BinaryExpr *)expr.get())->op = op;
    }
    return expr;
}



std::shared_ptr<Expr> Parser::term()
{
    std::shared_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS}))
    {
         Token op = previous();
         std::shared_ptr<Expr> right = factor();
         std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<BinaryExpr>();
        ((BinaryExpr *)expr.get())->left  = left;
        ((BinaryExpr *)expr.get())->right = right;
        ((BinaryExpr *)expr.get())->op = op;
    }
    return expr;
}

std::shared_ptr<Expr> Parser::factor()
{
    std::shared_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR, TokenType::MOD}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        std::shared_ptr<Expr> left = expr;
         expr =  std::make_shared<BinaryExpr>();
        ((BinaryExpr *)expr.get())->left  = left;
        ((BinaryExpr *)expr.get())->right = right;
        ((BinaryExpr *)expr.get())->op = op;
    }
    return expr;

}

std::shared_ptr<Expr> Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS, TokenType::INC, TokenType::DEC}))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        std::shared_ptr<UnaryExpr> u_expr =   std::make_shared<UnaryExpr>();
        u_expr->right = right;
        u_expr->op = op;
        u_expr->isPrefix = (op.type == TokenType::INC || op.type == TokenType::DEC);
        return u_expr;
    }

    return call();
}


std::shared_ptr<Expr> Parser::call()
{
    std::shared_ptr<Expr> expr = primary();



    while (true)
    {
        Token name = previous();
        if (match(TokenType::LEFT_PAREN))
        {
            expr = function_call(std::move(expr), name);
        } else if (match(TokenType::DOT))
        {
                Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
             //    INFO("Get property: %s %s", name.lexeme.c_str(),expr->toString().c_str());

                if (match(TokenType::LEFT_PAREN))
                {
                    std::shared_ptr<GetDefinitionExpr> get =  std::make_shared<GetDefinitionExpr>();
                    if (!check(TokenType::RIGHT_PAREN))
                    {
                        do
                        {
                            std::shared_ptr<Expr> value  =  expression();
                            get->values.push_back(std::move(value));
                            
                        } while (match(TokenType::COMMA));
                    }
                    consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

                    get->name = std::move(name);
                    get->variable = std::move(expr);
                    return get;
                } else if (match(TokenType::INC))
                {
                        Token op = previous();

                        std::shared_ptr<GetExpr> get =  std::make_shared<GetExpr>();
                        get->name = std::move(name);
                        get->object = std::move(expr);
                    
                       std::shared_ptr<UnaryExpr> u_expr =   std::make_shared<UnaryExpr>();
                       u_expr->right = get;
                       u_expr->op = op;
                       u_expr->isPrefix = false;

                       return u_expr;
                } else if (match(TokenType::DEC))
                {
                        Token op = previous();
                        std::shared_ptr<GetExpr> get =  std::make_shared<GetExpr>();
                        get->name = std::move(name);
                        get->object = std::move(expr);
                        std::shared_ptr<UnaryExpr> u_expr =   std::make_shared<UnaryExpr>();
                        u_expr->right = get;
                        u_expr->op = op;
                        u_expr->isPrefix = false;
                        return u_expr;
                }
            



            std::shared_ptr<GetExpr> get =  std::make_shared<GetExpr>();
            get->name = std::move(name);
            get->object = std::move(expr);
            return get;
        } 
        else
        {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::function_call(std::shared_ptr<Expr> expr,  Token name )
{
    std::shared_ptr<CallExpr> f =  std::make_shared<CallExpr>();
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            std::shared_ptr<Expr> arg = expression();
            f->args.push_back(std::move(arg));
            
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

  //  INFO("Function call: %s", name.lexeme.c_str());


   f->callee = std::move(expr);
   f->name = std::move(name);

    return f;
}

void Parser::freecalls()
{
   
}


std::shared_ptr<Expr> Parser::primary()
{
     if (match(TokenType::FALSE))
    {
        
          std::shared_ptr<NumberLiteral> b =  std::make_shared<NumberLiteral>();
          b->value = 0;
          return b;
    }
    if (match(TokenType::TRUE))
    {
          std::shared_ptr<NumberLiteral> b =    std::make_shared<NumberLiteral>();
          b->value = 1;
          return b;
    }
    
    if (match(TokenType::NIL))
    {
          return  std::make_shared<Literal>();
    }


    if (match(TokenType::STRING))
    {
        std::shared_ptr<StringLiteral> s =    std::make_shared<StringLiteral>();
        s->value = previous().literal;
        return s;
    }
    if (match(TokenType::NUMBER))
    {
        std::shared_ptr<NumberLiteral> f =    std::make_shared<NumberLiteral>();

        f->value = std::stof(previous().literal);
        return f;
    }
    if (match(TokenType::NOW))
    {
        return    std::make_shared<NowExpr>();
    }
    if (match(TokenType::SELF))
    {
        return self_expr();
    }
    // if (match(TokenType::SUPER))
    // {
    //     return super_expr();
    // }  
    if (match(TokenType::IDENTIFIER))
    {
        Token name = previous();
        std::shared_ptr<Variable> expr =    std::make_shared<Variable>();
        expr->name = name;

          
        


        if (match(TokenType::INC))
        {
  
            Token op = previous();
            std::shared_ptr<UnaryExpr> u_expr =    std::make_shared<UnaryExpr>();
            u_expr->right = expr;
            u_expr->op = op;
            u_expr->isPrefix = false;
            return u_expr;
        }
        if (match(TokenType::DEC))
        {
            Token op = previous();
            std::shared_ptr<UnaryExpr> u_expr =    std::make_shared<UnaryExpr>();
            u_expr->right = expr;
            u_expr->op = op;
            u_expr->isPrefix = false;
            return u_expr;
        }
    

        return expr;
    }


    if (match(TokenType::LEFT_PAREN))
    {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN,"Expect ')' after expression.");
        return expr;
    }
   

    return std::make_shared<Literal>();
}

std::shared_ptr<Expr> Parser::now()
{
    return  std::make_shared<NowExpr>();
}

std::shared_ptr<Program> Parser::program()
{
    try 
    {
        
    std::shared_ptr<Program> p =  std::make_shared<Program>();
    while (!isAtEnd())
    {
        p->statements.push_back(std::move(declarations()));
    }
    return p;
    }
    catch (const FatalException &e)
    {
        synchronize();
        return   nullptr;
    }
    return nullptr;
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    std::shared_ptr<ExpressionStmt> stmt =    std::make_shared<ExpressionStmt>();
    stmt->expression = std::move(expr);
    return stmt;
}
StmtPtr Parser::variable_declaration(bool inIntern)
{
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::vector<Token> names;
    names.push_back(name);

   ExprPtr initializer = nullptr;
   bool is_initialized = false;
   if (match(TokenType::LEFT_BRACKET))//array
   {
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after array declaration.");
        std::vector<ExprPtr> values;
    
        if (match(TokenType::EQUAL))
        {
            consume(TokenType::LEFT_BRACKET, "Expect '[' array initializer.");
            ExprPtr exp = expression();
            values.push_back(std::move(exp));
            while (match(TokenType::COMMA)  && !isAtEnd())
            {
                ExprPtr exp = expression();
                values.push_back(std::move(exp));
            }
            consume(TokenType::RIGHT_BRACKET, "Expect ']' after array initializer.");
        }
        consume(TokenType::SEMICOLON, "Expect ';' after array declaration.");

        std::shared_ptr<ArrayStmt> stmt =    std::make_shared<ArrayStmt>();
        stmt->name = std::move(name);
        stmt->values = std::move(values);
        return stmt;
   } else if  (match(TokenType::LEFT_BRACE)) 
   {
        consume(TokenType::RIGHT_BRACE, "Expect '}' after dictionary declaration.");
        std::unordered_map<ExprPtr, ExprPtr> values;
    
         if (match(TokenType::EQUAL))
         {
              consume(TokenType::LEFT_BRACE, "Expect '{' dictionary initializer.");
              ExprPtr key = expression();
              consume(TokenType::COLON, "Expect ':' after dictionary key.");
              ExprPtr value = expression();
              values[key] = value;  
             while (match(TokenType::COMMA)  && !isAtEnd())
             {
                 ExprPtr key = expression();
                 consume(TokenType::COLON, "Expect ':' after dictionary key.");
                 ExprPtr value = expression();
                 values[key] = value;
             }
             consume(TokenType::RIGHT_BRACE, "Expect '}' after dictionary initializer.");
        }
        consume(TokenType::SEMICOLON, "Expect ';' after dictionary declaration.");

        std::shared_ptr<MapStmt> stmt =    std::make_shared<MapStmt>();
        stmt->name = std::move(name);
        stmt->values = std::move(values);       
        return stmt;

   } if (match(TokenType::EQUAL))
   {
       initializer = expression();
       is_initialized = true;
   } else 
   { 

        while (match(TokenType::COMMA) && !isAtEnd())
        {
           Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
           names.push_back(name);
        }
         if (match(TokenType::EQUAL))
        {
            initializer = expression();
            is_initialized = true;
        } 
   }
   if (!inIntern)
   {
         consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
   } 
   std::shared_ptr<Declaration> stmt =    std::make_shared<Declaration>();
   stmt->names = std::move(names);
   if (!is_initialized)
   {
       WARNING("Variable '%s' is not initialized !", name.lexeme.c_str());
       initializer = std::make_shared<Literal>();
   }
   stmt->initializer = initializer;
   return stmt;
}
std::shared_ptr<Stmt> Parser::function_declaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    std::vector<std::string> names;

    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
           Token name =  consume(TokenType::IDENTIFIER, "Expect parameter name.");
           names.push_back(std::move(name.lexeme));
        } while (match(TokenType::COMMA));
    }
    

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");


    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");

    std::shared_ptr<FunctionStmt> stmt =    std::make_shared<FunctionStmt>();
    stmt->name = std::move(name);
    stmt->args = std::move(names);
    stmt->body = std::move(block());
    return stmt;
}

std::shared_ptr<Stmt> Parser::print_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'print'.");
    std::shared_ptr<Expr> expr = std::move(expression());
    consume(TokenType::RIGHT_PAREN, "Expect ')' after value.");
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    std::shared_ptr<PrintStmt> stmt =    std::make_shared<PrintStmt>();
    stmt->expression = std::move(expr);
    return stmt;
}

//******************************************************************************************************************* */
StmtPtr Parser::statement()
{
    if (match(TokenType::FUNCTION))
    {
        return function_declaration();
    }
    if (match(TokenType::IF))
    {
        return if_statement();
    }
    if (match(TokenType::SWITCH))
    {
        return switch_statement();
    }
    if (match(TokenType::RETURN))
    {
        return return_statement();
    }
    if (match(TokenType::BREAK))
    {
        return break_statement();
    }
    if (match(TokenType::CONTINUE))
    {
        return continue_statement();
    }
    if (match(TokenType::WHILE))
    {
        return while_statement();
    }
    if (match(TokenType::DO))
    {
        return do_statement();
    }
    if (match(TokenType::FOR))
    {
        return for_statement();
    }
    if (match(TokenType::FROM))
    {
        return from_statement();
    }
    if (match(TokenType::PRINT))
    {
        return print_statement();
    }

    if (match(TokenType::LEFT_BRACE))
    {
        return block();
    }

    return expression_statement();
}

StmtPtr Parser::declarations()
{
   
        if (match(TokenType::VAR))
        {
            return variable_declaration();
        }
        if (match(TokenType::STRUCT))
        {
            return struct_declaration();
        }

        if (match(TokenType::CLASS))
        {
            return class_declaration();
        }
        return statement();
 
}

StmtPtr Parser::block()
{
     std::shared_ptr<BlockStmt> stmt =   std::make_shared<BlockStmt>();
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        stmt->statements.push_back(std::move(declarations()));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");


    return stmt;
}

StmtPtr Parser::if_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    std::shared_ptr<Stmt> thenBranch = statement();


    std::vector<std::shared_ptr<ElifStmt>> elifBranch;
    while (match(TokenType::ELIF))
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'elif'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        std::shared_ptr<Stmt> thenBranch = statement();
        std::shared_ptr<ElifStmt> elif =   std::make_shared<ElifStmt>();
        elif->condition = std::move(condition);
        elif->then_branch = std::move(thenBranch);
        elifBranch.push_back(std::move(elif));
    }
    


    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::ELSE))
    {
        elseBranch = statement();
    }
    std::shared_ptr<IFStmt> stmt =    std::make_shared<IFStmt>();
    stmt->condition = std::move(condition);
    stmt->then_branch = std::move(thenBranch);
    stmt->else_branch = std::move(elseBranch);
    stmt->elifBranch = std::move(elifBranch);
    return stmt;
}

StmtPtr Parser::while_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    std::shared_ptr<Stmt> body = statement();
    std::shared_ptr<WhileStmt> stmt =    std::make_shared<WhileStmt>();
    stmt->condition = std::move(condition);
    stmt->body = std::move(body);
    return stmt;
}

StmtPtr Parser::do_statement()
{
     std::shared_ptr<Stmt> body = statement();
    consume(TokenType::WHILE, "Expect 'while' after 'do'.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after 'while' condition.");
    consume(TokenType::SEMICOLON, "Expect ';' after 'while' body.");
    std::shared_ptr<DoStmt> stmt =   std::make_shared<DoStmt>();
    stmt->condition = std::move(condition);
    stmt->body = std::move(body);
    return stmt;    
}


StmtPtr Parser::from_statement()
{

    consume(TokenType::LEFT_PAREN, "Expect '(' after 'from'.");
    consume(TokenType::VAR, "Expect variable declaration  .");
    StmtPtr  var =  variable_declaration(true);
    
    consume(TokenType::COLON, "Expect ':' after variable.");
    ExprPtr array =  expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after 'from' condition.");
    StmtPtr body = statement();
    std::shared_ptr<FromStmt> stmt =    std::make_shared<FromStmt>();
    stmt->variable = std::move(var);
    stmt->array = std::move(array);
    stmt->body = std::move(body);
    return stmt;
}

StmtPtr Parser::for_statement()
{
   consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
   std::shared_ptr<Stmt> initializer;
   if (match({TokenType::SEMICOLON}))
   {
       Error(peek(), "Missing 'for' initializer.");
    } else if (match({TokenType::VAR}))
    {
        initializer=std::move(variable_declaration());
    } else
    {
        initializer=std::move(expression_statement());
    }


    if (match({TokenType::SEMICOLON}))
    {
       Error("Missing 'for' condition.");
    }
    std::shared_ptr<Expr> condition  = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after for condition.");




    if (match(TokenType::SEMICOLON))
    {
        Error(peek(), "Missing 'for' step.");
    }

    std::shared_ptr<Expr> increment =   expression();
    
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    
    std::shared_ptr<Stmt> body = statement();


    std::shared_ptr<ForStmt> stmt =    std::make_shared<ForStmt>();

    
    stmt->initializer = std::move(initializer);
    stmt->condition = std::move(condition);
    stmt->increment = std::move(increment);
    stmt->body = std::move(body);
    return stmt;
}

StmtPtr Parser::return_statement()
{
 Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON))
    {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    std::shared_ptr<ReturnStmt> stmt =   std::make_shared<ReturnStmt>();
    stmt->value = std::move(value);
    return stmt;
}

StmtPtr Parser::break_statement()
{
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    return   std::make_shared<BreakStmt>();

}

StmtPtr Parser::continue_statement()
{
    consume(TokenType::SEMICOLON, "Expect ';' after 'continue'.");
    return   std::make_shared<ContinueStmt>();

}

StmtPtr Parser::switch_statement()
{
    consume(TokenType::LEFT_PAREN,"Expect '(' after 'switch'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN,"Expect ')' after condition.");
    consume(TokenType::LEFT_BRACE,"Expect '{' before switch block.");
    
    
    std::vector<std::shared_ptr<CaseStmt>> cases;
    while (match(TokenType::CASE))
    {
        std::shared_ptr<Expr> exp = expression();
        consume(TokenType::COLON,"Expect ':' after case expression.");
        std::shared_ptr<Stmt> body = statement();
        std::shared_ptr<CaseStmt> case_stmt =    std::make_shared<CaseStmt>();
        case_stmt->condition = std::move(exp);
        case_stmt->body = std::move(body);
        cases.push_back(std::move(case_stmt));
    }


    std::shared_ptr<Stmt> default_case= nullptr;
    if (match(TokenType::DEFAULT))
    {
        consume(TokenType::COLON,"Expect ':' after default case.");
        default_case = statement();
       
    }

    consume(TokenType::RIGHT_BRACE,"Expect '}' after switch block.");

    if (default_case == nullptr && cases.size() == 0)
    {
        Error(peek(), "Switch statement must have at least one case or default case.");
    }
   

    std::shared_ptr<SwitchStmt> stmt =    std::make_shared<SwitchStmt>();
    stmt->condition = std::move(condition);
    stmt->cases = std::move(cases);
    stmt->defaultBranch = std::move(default_case);
    return stmt;
}

StmtPtr Parser::class_declaration()
{

    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
     std::shared_ptr<Variable> superClass = nullptr;
    if (match(TokenType::COLON))
    {
        consume(TokenType::IDENTIFIER, "Expect super class name.");
        superClass = std::make_shared<Variable>();
        superClass->name = previous();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    std::shared_ptr<ClassStmt> stmt =  std::make_shared<ClassStmt>();
    stmt->name = std::move(name);
    stmt->superClass = superClass;

     while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        if (match(TokenType::VAR))
        {
            
            StmtPtr var = variable_declaration();
            stmt->fields.push_back(std::move(var));
            
        } else 
        if (match(TokenType::FUNCTION))
        {
            StmtPtr func = function_declaration();
            stmt->methods.push_back(std::move(func));
        }
          
    }


    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
    consume(TokenType::SEMICOLON, "Expect ';' after class declaration.");

    return stmt;
}

StmtPtr Parser::struct_declaration()
{

    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");

    std::shared_ptr<StructStmt> stmt =  std::make_shared<StructStmt>();
    stmt->name = std::move(name);

    

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        if (match(TokenType::VAR))
        {
            StmtPtr var = variable_declaration();
            stmt->values.push_back(std::move(var));
        }
          
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    consume(TokenType::SEMICOLON, "Expect ';' after struct declaration.");

    return stmt;
}

//*******************************************************************************************************************


std::shared_ptr<Program> Parser::parse()
{
   
        return   program();
    
  
}
