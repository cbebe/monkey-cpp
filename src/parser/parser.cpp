#include "../parser.hpp"
#include <memory>

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

std::vector<Identifier> Parser::parse_function_parameters() {
  std::vector<Identifier> params{};
  if (peek_token.is_type<RParen>()) {
    next_token();
    return params;
  }
  next_token();
  params.push_back(Identifier{std::get<Ident>(cur_token.value).literal});

  while (peek_token.is_type<Comma>()) {
    next_token();
    next_token();
    params.push_back(Identifier{std::get<Ident>(cur_token.value).literal});
  }

  if (!expect_peek<RParen>()) {
    return std::vector<Identifier>{};
  }

  return params;
}

std::vector<std::unique_ptr<Expression>> Parser::parse_call_arguments() {
  std::vector<std::unique_ptr<Expression>> args{};
  if (peek_token.is_type<RParen>()) {
    next_token();
    return args;
  }
  next_token();
  args.push_back(std::unique_ptr<Expression>{parse_expression(LOWEST)});

  while (peek_token.is_type<Comma>()) {
    next_token();
    next_token();
    args.push_back(std::unique_ptr<Expression>{parse_expression(LOWEST)});
  }

  if (!expect_peek<RParen>()) {
    return std::vector<std::unique_ptr<Expression>>{};
  }

  return args;
}

Parser::Parser(Lexer l) : lexer(l) {
  next_token();
  next_token();

  register_prefix(Ident{}, &Parser::parse_identifier);
  register_prefix(LParen{}, &Parser::parse_grouped_expression);
  register_prefix(Int{}, &Parser::parse_integer_literal);
  register_prefix(True{}, &Parser::parse_boolean_literal);
  register_prefix(False{}, &Parser::parse_boolean_literal);
  register_prefix(Function{}, &Parser::parse_function_literal);
  register_prefix(Bang{}, &Parser::parse_prefix_expression);
  register_prefix(Minus{}, &Parser::parse_prefix_expression);
  register_prefix(If{}, &Parser::parse_if_expression);

  register_infix(Plus{}, &Parser::parse_infix_expression);
  register_infix(Minus{}, &Parser::parse_infix_expression);
  register_infix(Slash{}, &Parser::parse_infix_expression);
  register_infix(Asterisk{}, &Parser::parse_infix_expression);
  register_infix(Eq{}, &Parser::parse_infix_expression);
  register_infix(NotEq{}, &Parser::parse_infix_expression);
  register_infix(LT{}, &Parser::parse_infix_expression);
  register_infix(GT{}, &Parser::parse_infix_expression);
  register_infix(LParen{}, &Parser::parse_call_expression);
};
