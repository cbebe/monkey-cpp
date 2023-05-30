#include "parser.hpp"
#include "ast.hpp"
#include <memory>
#include <string>
#include <unordered_map>

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

Statement *Parser::parse_statement() {
  if (cur_token.is_type<Let>()) {
    return parse_let_statement();
  }
  if (cur_token.is_type<Return>()) {
    return parse_return_statement();
  }

  return parse_expression_statement();
}

Expression *Parser::parse_expression(Precedence p) {
  auto prefix = prefix_parse_fns[cur_token.value];
  if (!prefix) {
    return nullptr;
  }
  auto left_exp{(this->*prefix)()};

  return left_exp;
}

ExpressionStatement *Parser::parse_expression_statement() {
  Expression *expr{parse_expression(LOWEST)};
  if (peek_token.is_type<Semicolon>()) {
    next_token();
  }

  return new ExpressionStatement{expr};
}

ReturnStatement *Parser::parse_return_statement() {
  next_token();

  // TODO: parse expression
  while (!cur_token.is_type<Semicolon>()) {
    next_token();
  }

  return new ReturnStatement{nullptr};
}

LetStatement *Parser::parse_let_statement() {
  if (!expect_peek<Ident>()) {
    return nullptr;
  }

  Identifier identifier{std::get<Ident>(cur_token.value).literal};

  if (!expect_peek<Assign>()) {
    return nullptr;
  }

  while (!cur_token.is_type<Semicolon>()) {
    next_token();
  }

  // TODO: Parse the expression
  return new LetStatement{identifier, nullptr};
}

template <typename TokenType> void Parser::peek_error(Token t) {
  auto message = "expected next token to be " + type_string<TokenType>() +
                 ". got " + t.to_string();
  errors.push_back(message);
}

template <typename TokenType> bool Parser::expect_peek() {
  if (peek_token.is_type<TokenType>()) {
    next_token();
    return true;
  } else {
    peek_error<TokenType>(peek_token);
    return false;
  }
}

void Parser::next_token() {
  cur_token = peek_token;
  peek_token = lexer.next_token();
}

void Parser::register_prefix(TokenVariant t, PrefixParseFn fn) {
  prefix_parse_fns.insert(std::make_pair(t, fn));
}

void Parser::register_infix(TokenVariant t, InfixParseFn fn) {
  infix_parse_fns.insert(std::make_pair(t, fn));
}

Expression *Parser::parse_identifier() {
  return new Identifier{std::get<Ident>(cur_token.value).literal};
}

Expression *Parser::parse_integer_literal() {
  return new IntegerLiteral{std::get<Int>(cur_token.value).value};
}

Parser::Parser(Lexer l) : lexer(l) {
  next_token();
  next_token();

  register_prefix(Ident{}, &Parser::parse_identifier);
  register_prefix(Int{}, &Parser::parse_integer_literal);
};
