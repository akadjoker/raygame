#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>

#include "Token.hpp"



class Lexer
{
private:
    std::string input;
    int start;
    int current;
    int line;
    bool panicMode;

    std::vector<Token> tokens;
    std::unordered_map<std::string, TokenType> keywords;

    int blocks;   // begin end
    int brackets; // []
    int braces;   // {}
    int parens;   // ()


    char peek();
    char advance();
    bool match(char expected);
    char peekNext();
    char peekAhead(int n);
    char previous();

    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    void identifier();
    void number();
    void string();

    void addToken(TokenType type, const std::string &literal = "");

    void Error(std::string message);
    void Warning(std::string message);

    std::string extractIdentifier(const std::string &str);

public:
    Lexer();
    ~Lexer() = default;
    void initialize();
    bool Load(const std::string &input);
    void process();
    bool ready();
    void clear();
    std::vector<Token> GetTokens();
 
};