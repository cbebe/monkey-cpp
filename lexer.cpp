#pragma once
#include "token.cpp"
#include <string>

class Lexer {
public:
  Lexer(std::string input) : input{input} { read_char(); };
  Token next_token();

private:
  void read_char();
  std::string input;
  int position{0};
  int read_position{0};
  char ch{0};
};

Token new_token(token_types::token_type t) { return Token{t}; }

Token Lexer::next_token() {
  using namespace token_types;
  Token tok{Eof{}};
  switch (ch) {
    // clang-format off
  case '=': tok = new_token(Assign{}); break;
  case ';': tok = new_token(Semicolon{}); break;
  case '(': tok = new_token(LParen{}); break;
  case ')': tok = new_token(RParen{}); break;
  case ',': tok = new_token(Comma{}); break;
  case '+': tok = new_token(Plus{}); break;
  case '{': tok = new_token(LBrace{}); break;
  case '}': tok = new_token(RBrace{}); break;
  case   0: tok = new_token(Eof{}); break;
    // clang-format on
  }
  read_char();
  return tok;
}

void Lexer::read_char() {
  if (read_position >= input.length()) {
    ch = 0;
  } else {
    ch = input[read_position];
  }
  position = read_position++;
}
