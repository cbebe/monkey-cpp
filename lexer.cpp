#pragma once
#include "token.cpp"
#include <string>
class Lexer {

public:
  Lexer(std::string input) : input{input} {}
  Token next_token();

private:
  std::string input;
};

Token Lexer ::next_token() { return Token{token_types::Eof{}}; }
