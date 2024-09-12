#include "pch.h"
#include "Lexer.hpp"
#include "Utils.hpp"


Lexer::Lexer()
{
}

bool Lexer::Load(const std::string &input)
{
  if (input.size() == 0)
    return false;
  this->input = input;
  start = 0;
  current = 0;
  line = 1;
  panicMode = false;

  return true;
}

void Lexer::process()
{
  char c = advance();
  switch (c)
  {
  case '(':
  {
    parens++;
    addToken(TokenType::LEFT_PAREN);
    break;
  }
  case ')':
  {
    parens--;
    addToken(TokenType::RIGHT_PAREN);
    break;
  }
  case '{':
  {
    braces++;
    addToken(TokenType::LEFT_BRACE);
    break;
  }
  case '}':
  {
    braces--;
    addToken(TokenType::RIGHT_BRACE);
    break;
  }
  case '[':
  {
    brackets++;
    addToken(TokenType::LEFT_BRACKET);
    break;
  }
  case ']':
  {
    brackets--;
    addToken(TokenType::RIGHT_BRACKET);
    break;
  }

  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
  {
    if (match('-'))
      addToken(TokenType::DEC);
    else if (match('='))
      addToken(TokenType::MINUS_EQUAL);
    else
      addToken(TokenType::MINUS);
    break;
  }
  case '+':
  {
    if (match('+'))
      addToken(TokenType::INC);
    else if (match('='))
      addToken(TokenType::PLUS_EQUAL);
    else
      addToken(TokenType::PLUS);

    break;
  }
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case ':':
    addToken(TokenType::COLON);
    break;

  case '^':
    addToken(TokenType::POWER);
    break;

  case '%':
    addToken(TokenType::MOD);
    break;

  case '*':
  {
    if (match('='))
        addToken(TokenType::STAR_EQUAL);
    else
        addToken(TokenType::STAR);
    break;
  }
  // case '|':
  // {
  //  // if (match('|'))
  //   //     addToken(TokenType::OR);
  //   break;
  // }
  // case '&':
  // {
  //   //if (match('&'))
  //   //  addToken(TokenType::AND);
  //   break;
  // }

  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;

  case '/':
    if (match('='))
    {
      addToken(TokenType::SLASH_EQUAL);
    }
    else if (match('/'))
    {
      while (peek() != '\n' && !isAtEnd())
        advance();
    }
    else if (match('*'))
    {
      advance();
      while (!isAtEnd())
      {
        if (peek() == '*')
          break;
        if (peek() == '\n')
          line++;
        advance();
      }
      if (peek() == '*' && peekNext() == '/')
      {
        advance();
        advance();
        break;
      }
      else
      {
        Error("Unterminated comment");
        return;
      }
    }
    else
    {
      addToken(TokenType::SLASH);
    }
    break;

  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace.
    break;

  case '\n':
    line++;
    break;
  case '"':
    string();
    break;

  default:
    if (isDigit(c))
    {
      number();
    }
    else if (isAlpha(c))
    {
      identifier();
    }
    else
    {
      Error("Unexpected character");
    }

    break;
  }
}

bool Lexer::ready()
{
  if (input.size() == 0)
  {
    Log(2, "Input is empty");
    return false;
  }
  if (blocks > 0)
  {
    Log(2, "Blocks 'begin' 'end' not closed");
    return false;
  }
  if (brackets > 0)
  {
    Log(2, "Brackets not closed");
    return false;
  }
  if (braces > 0)
  {
    Log(2, "Braces not closed");
    return false;
  }
  if (parens > 0)
  {
    Log(2, "Parens not closed");
    return false;
  }
  return true;
}

void Lexer::clear()
{

  input = "";
  start = 0;
  current = 0;
  line = 1;
  tokens.clear();
  blocks = 0;
  brackets = 0;
  braces = 0;
  parens = 0;
  

  panicMode = false;

}

std::vector<Token> Lexer::GetTokens()
{
  if (panicMode)
  {
    return tokens;
  }
  while (!isAtEnd())
  {
    start = current;
    process();
    if (panicMode)
    {
      tokens.clear();
      std::string error = input.substr(start, current - start);
      auto token = Token(TokenType::ERROR, "Error", error, line);
      tokens.push_back(token);
      return tokens;
    }
  }
  auto token = Token(TokenType::END_OF_FILE, "EOF", "", line);
  tokens.push_back(token);
  return tokens;
}

char Lexer::peek()
{
  if (isAtEnd())
    return '\0';
  return input[current];
}

char Lexer::advance()
{
  return input[current++];
}

bool Lexer::match(char expected)
{
  if (isAtEnd())
    return false;
  if (input[current] != expected)
    return false;

  current++;
  return true;
}

char Lexer::peekNext()
{
  if (current + 1 >= (int)input.size())
    return '\0';
  return input[current + 1];
}

char Lexer::peekAhead(int n)
{
  if (current + n >= (int)input.size())
    return '\0';
  return input[current + n];
}

char Lexer::previous()
{
  if (current - 1 < 0)
    return '\0';
  return input[current - 1];
}

bool Lexer::isAtEnd()
{
  return current >= (int)input.size();
}

bool Lexer::isDigit(char c)
{
  return (c >= '0' && c <= '9');
}

bool Lexer::isAlpha(char c)
{
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         c == '_';
}

bool Lexer::isAlphaNumeric(char c)
{
  return isAlpha(c) || isDigit(c);
}

std::string Lexer::extractIdentifier(const std::string &str)
{
  std::string result;
  for (char c : str)
  {
    if (isAlpha(c) || c == '_')
    {
      result += c;
    }
  }
  return result;
}





void Lexer::initialize()
{


  keywords["and"] = TokenType::AND;

  keywords["or"] = TokenType::OR;

  keywords["xor"] = TokenType::XOR;

  keywords["if"] = TokenType::IF;

  keywords["else"] = TokenType::ELSE;

 keywords["elif"] = TokenType::ELIF;


  keywords["switch"] = TokenType::SWITCH;

  keywords["case"] = TokenType::CASE;

  keywords["default"] = TokenType::DEFAULT;




  keywords["for"] = TokenType::FOR;

  keywords["from"] = TokenType::FROM;

  keywords["while"] = TokenType::WHILE;

  keywords["do"] = TokenType::DO;

  keywords["return"] = TokenType::RETURN;

  keywords["break"] = TokenType::BREAK;

  keywords["continue"] = TokenType::CONTINUE;

  keywords["def"] = TokenType::FUNCTION;

  keywords["class"] = TokenType::CLASS;

  keywords["struct"] = TokenType::STRUCT;

  keywords["self"] = TokenType::SELF;

  //keywords["super"] = TokenType::SUPER;

  keywords["nil"] = TokenType::NIL;
  keywords["var"] = TokenType::VAR;
  keywords["false"] = TokenType::FALSE;
  keywords["true"] = TokenType::TRUE;

  keywords["print"] = TokenType::PRINT;
  keywords["now"] = TokenType::NOW;
}

void Lexer::identifier()
{

  while (isAlphaNumeric(peek()))
    advance();

  // std::cout<<" ,"<<peek()<<" ";

  std::string text = input.substr(start, current - start);
  if (keywords.find(text) != keywords.end())
  {
    addToken(keywords[text]);
    return;
  }

  addToken(TokenType::IDENTIFIER, text);
  
}

void Lexer::string()
{
  while (peek() != '"' && !isAtEnd())
  {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd())
  {
    Error("Unterminated string");
    return;
  }

  advance();

   size_t length = (current - start) - 2;
   std::string text = input.substr(start + 1, length);
  addToken(TokenType::STRING, text);
}

void Lexer::number()
{
  std::string text = "";
  bool hasDot = false;

  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext()))
  {
    hasDot = true;
    advance();
    while (isDigit(peek()))
      advance();
  }

  text = input.substr(start, current - start);
  addToken(TokenType::NUMBER, text);
}

void Lexer::addToken(TokenType type, const std::string &literal)
{
  if (type == TokenType::LEFT_BRACE)
  {
    blocks++;
  }
  if (type == TokenType::RIGHT_BRACE)
  {
    blocks--;
  }
  std::string text = input.substr(start, current - start);
  Token token = Token(type, text, literal, line);
  tokens.push_back(token);
}

void Lexer::Error(std::string message)
{
  panicMode = true;
  std::string text = "ERROR: " + message + " at line: " + std::to_string(line);
  throw  FatalException(text);

}

void Lexer::Warning(std::string message)
{

  std::string text = "WARNING: " + message + " at line: " + std::to_string(line);
  throw FatalException(text);
}
