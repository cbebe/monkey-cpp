#include "parser.hpp"
#include "ast.hpp"
#include <iostream>
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

Statement *Parser::parse_statement() {
  if (cur_token.is_type<Let>()) {
    return parse_let_statement();
  }

  return nullptr;
}

LetStatement *Parser::parse_let_statement() {
  if (!expect_peek<Ident>()) {
    return nullptr;
  }

  Identifier identifier{std::get<Ident>(cur_token.value).literal};

  if (!expect_peek<Assign>()) {
    return nullptr;
  }

  while (cur_token.is_type<Semicolon>()) {
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

Parser::Parser(Lexer l) : lexer(l) {
  next_token();
  next_token();
};
