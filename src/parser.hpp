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
  std::vector<std::string> errors{};

private:
  void next_token();
  template <typename TokenType> bool expect_peek();
  template <typename TokenType> void peek_error(Token t);
  Statement *parse_statement();
  LetStatement *parse_let_statement();
  ReturnStatement *parse_return_statement();
  Token cur_token;
  Token peek_token;
  Lexer lexer;
};
