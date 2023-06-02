#include "../parser.hpp"

using namespace token_types;

std::unique_ptr<Statement> Parser::parse_statement() {
  if (cur_token.is_type<Let>()) {
    return parse_let_statement();
  }
  if (cur_token.is_type<Return>()) {
    return parse_return_statement();
  }

  return parse_expression_statement();
}

std::unique_ptr<Statement> Parser::parse_expression_statement() {
  auto expr{parse_expression(LOWEST)};
  if (peek_token.is_type<Semicolon>()) {
    next_token();
  }

  return std::unique_ptr<Statement>{std::make_unique<ExpressionStatement>(
      ExpressionStatement{std::move(expr)})};
}

std::unique_ptr<Statement> Parser::parse_return_statement() {
  next_token();

  auto ret_val{parse_expression(LOWEST)};
  if (peek_token.is_type<Semicolon>()) {
    next_token();
  }

  return std::unique_ptr<Statement>{
      std::make_unique<ReturnStatement>(ReturnStatement{std::move(ret_val)})};
}

std::unique_ptr<Statement> Parser::parse_let_statement() {
  if (!expect_peek<Ident>()) {
    return nullptr;
  }

  Identifier identifier{std::get<Ident>(cur_token.value).literal};

  if (!expect_peek<Assign>()) {
    return nullptr;
  }
  next_token();

  auto value{parse_expression(LOWEST)};

  if (peek_token.is_type<Semicolon>()) {
    next_token();
  }

  return std::make_unique<LetStatement>(
      LetStatement{identifier, std::move(value)});
}

std::unique_ptr<BlockStatement> Parser::parse_block_statement() {
  auto block = std::make_unique<BlockStatement>();
  next_token();
  while (!cur_token.is_type<RSquirly>() && !cur_token.is_type<Eof>()) {
    auto statement{parse_statement()};
    if (statement) {
      block->statements.push_back(std::move(statement));
    }
    next_token();
  }
  return block;
}
