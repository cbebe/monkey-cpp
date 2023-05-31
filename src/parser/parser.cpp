#include "../parser.hpp"

using namespace token_types;

std::unique_ptr<Program> Parser::parse_program() {
  auto program = std::make_unique<Program>();
  while (!cur_token.is_type<Eof>()) {
    auto statement{parse_statement()};
    if (statement) {
      program->statements.push_back(statement);
    }
    next_token();
  }
  return program;
}

void Parser::next_token() {
  cur_token = peek_token;
  peek_token = lexer.next_token();
}

inline void Parser::register_prefix(TokenVariant t, PrefixParseFn fn) {
  prefix_parse_fns.insert(std::make_pair(t, fn));
}

inline void Parser::register_infix(TokenVariant t, InfixParseFn fn) {
  infix_parse_fns.insert(std::make_pair(t, fn));
}

Parser::Parser(Lexer l) : lexer(l) {
  next_token();
  next_token();

  register_prefix(Ident{}, &Parser::parse_identifier);
  register_prefix(Int{}, &Parser::parse_integer_literal);
  register_prefix(Bang{}, &Parser::parse_prefix_expression);
  register_prefix(Minus{}, &Parser::parse_prefix_expression);
};
