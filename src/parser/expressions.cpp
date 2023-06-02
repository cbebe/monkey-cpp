#include "../parser.hpp"

using namespace token_types;

std::unordered_map<TokenVariant, Precedence> precedences{
    {Eq{}, EQUALS},      {NotEq{}, EQUALS},     {LT{}, LESSGREATER},
    {GT{}, LESSGREATER}, {Plus{}, SUM},         {Minus{}, SUM},
    {Slash{}, PRODUCT},  {Asterisk{}, PRODUCT}, {LParen{}, CALL},
};

Precedence get_precedence(TokenVariant v) {
  auto precedence{precedences[v]};
  return precedence ? precedence : LOWEST;
}

Precedence Parser::peek_predence() { return get_precedence(peek_token.value); }
Precedence Parser::cur_predence() { return get_precedence(cur_token.value); }

std::unique_ptr<Expression> Parser::parse_expression(Precedence precedence) {
  auto prefix{prefix_parse_fns[cur_token.value]};
  if (!prefix) {
    errors.push_back("no prefix parse function for " + cur_token.to_string() +
                     " found");
    return nullptr;
  }
  auto left_exp{(this->*prefix)()};

  while (!peek_token.is_type<Semicolon>() && precedence < peek_predence()) {
    auto infix{infix_parse_fns[peek_token.value]};
    if (!infix) {
      return left_exp;
    }
    next_token();
    left_exp = (this->*infix)(std::move(left_exp));
  }

  return left_exp;
}

std::unique_ptr<Expression> Parser::parse_identifier() {
  return std::unique_ptr<Expression>{std::make_unique<Identifier>(
      Identifier{std::get<Ident>(cur_token.value).literal})};
}

std::unique_ptr<Expression> Parser::parse_integer_literal() {
  return std::unique_ptr<Expression>{std::make_unique<IntegerLiteral>(
      IntegerLiteral{std::get<Int>(cur_token.value).value})};
}

std::unique_ptr<Expression> Parser::parse_boolean_literal() {
  return std::unique_ptr<Expression>{std::make_unique<BooleanLiteral>(
      BooleanLiteral{cur_token.is_type<True>()})};
}

std::unique_ptr<Expression> Parser::parse_function_literal() {
  if (!expect_peek<LParen>()) {
    return nullptr;
  }
  auto params{parse_function_parameters()};
  if (!expect_peek<LSquirly>()) {
    return nullptr;
  }
  auto body{parse_block_statement()};
  return std::unique_ptr<Expression>{std::make_unique<FunctionLiteral>(
      FunctionLiteral{params, std::move(body)})};
}

std::unique_ptr<Expression> Parser::parse_if_expression() {
  if (!expect_peek<LParen>()) {
    return nullptr;
  }
  next_token();
  auto condition{parse_expression(LOWEST)};
  if (!expect_peek<RParen>()) {
    return nullptr;
  }
  if (!expect_peek<LSquirly>()) {
    return nullptr;
  }
  auto consequence{parse_block_statement()};
  std::unique_ptr<BlockStatement> alternative{};
  if (peek_token.is_type<Else>()) {
    next_token();
    if (!expect_peek<LSquirly>()) {
      return nullptr;
    }
    alternative = parse_block_statement();
  }

  return std::unique_ptr<Expression>{std::make_unique<IfExpression>(
      IfExpression{std::move(condition), std::move(consequence),
                   std::move(alternative)})};
}

std::unique_ptr<Expression> Parser::parse_grouped_expression() {
  next_token();
  auto expr{parse_expression(LOWEST)};
  if (!expect_peek<RParen>()) {
    return nullptr;
  }
  return expr;
}

std::unique_ptr<Expression> Parser::parse_prefix_expression() {
  auto oper{cur_token.value};
  next_token();
  auto right{parse_expression(PREFIX)};
  return std::unique_ptr<Expression>{std::make_unique<PrefixExpression>(
      PrefixExpression{oper, std::move(right)})};
}

std::unique_ptr<Expression>
Parser::parse_infix_expression(std::unique_ptr<Expression> left) {
  auto oper{cur_token.value};
  auto precedence{cur_predence()};
  next_token();
  auto right{parse_expression(precedence)};

  return std::unique_ptr<Expression>{std::make_unique<InfixExpression>(
      InfixExpression{std::move(left), oper, std::move(right)})};
}

std::unique_ptr<Expression>
Parser::parse_call_expression(std::unique_ptr<Expression> caller) {
  return std::unique_ptr<Expression>{std::make_unique<CallExpression>(
      CallExpression{std::move(caller), parse_call_arguments()})};
}
