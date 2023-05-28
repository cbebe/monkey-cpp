#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <optional>
#include <string>

class Parser {
public:
  Parser(Lexer l);
  std::unique_ptr<Program> parse_program();

private:
  void next_token();
  template <typename TokenType> bool expect_peek();
  Statement *parse_statement();
  LetStatement *parse_let_statement();
  Token cur_token;
  Token peek_token;
  Lexer lexer;
};
