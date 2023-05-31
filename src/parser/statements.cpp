#include "../parser.hpp"

using namespace token_types;

Statement *Parser::parse_statement() {
  if (cur_token.is_type<Let>()) {
    return parse_let_statement();
  }
  if (cur_token.is_type<Return>()) {
    return parse_return_statement();
  }

  return parse_expression_statement();
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

BlockStatement *Parser::parse_block_statement() {
  auto block{new BlockStatement{}};
  next_token();
  while (!cur_token.is_type<RSquirly>() && !cur_token.is_type<Eof>()) {
    auto statement{parse_statement()};
    if (statement) {
      block->statements.push_back(statement);
    }
    next_token();
  }
  return block;
}
