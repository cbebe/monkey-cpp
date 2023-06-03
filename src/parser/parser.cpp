#include "parser.hpp"
#include <memory>

using namespace token_types;

std::unique_ptr<Program> Parser::parse_program() {
  auto program = std::make_unique<Program>();
  while (!cur_token.is_type<Eof>()) {
    auto statement{parse_statement()};
    if (statement) {
      program->statements.push_back(std::move(statement));
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
