#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <memory>

class Parser {
public:
  Parser(Lexer l);
  std::unique_ptr<Program> parse_program();

private:
  void next_token();
  Token cur_token;
  Token peek_token;
  Lexer lexer;
};
