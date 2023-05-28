#include "parser.hpp"
#include "ast.hpp"
#include <memory>

std::unique_ptr<Program> Parser::parse_program() {
  return std::unique_ptr<Program>{};
}

void Parser::next_token() {
  cur_token = peek_token;
  peek_token = lexer.next_token();
}

Parser::Parser(Lexer l) : lexer(l) {
  next_token();
  next_token();
};
