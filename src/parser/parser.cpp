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
  register_prefix(LParen{}, &Parser::parse_grouped_expression);
  register_prefix(Int{}, &Parser::parse_integer_literal);
  register_prefix(True{}, &Parser::parse_boolean_literal);
  register_prefix(False{}, &Parser::parse_boolean_literal);
  register_prefix(Bang{}, &Parser::parse_prefix_expression);
  register_prefix(Minus{}, &Parser::parse_prefix_expression);

  register_infix(Plus{}, &Parser::parse_infix_expression);
  register_infix(Minus{}, &Parser::parse_infix_expression);
  register_infix(Slash{}, &Parser::parse_infix_expression);
  register_infix(Asterisk{}, &Parser::parse_infix_expression);
  register_infix(Eq{}, &Parser::parse_infix_expression);
  register_infix(NotEq{}, &Parser::parse_infix_expression);
  register_infix(LT{}, &Parser::parse_infix_expression);
  register_infix(GT{}, &Parser::parse_infix_expression);
};
