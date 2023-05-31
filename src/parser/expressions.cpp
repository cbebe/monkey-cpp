#include "../parser.hpp"

using namespace token_types;

std::unordered_map<TokenVariant, Precedence> precedences{
    {Eq{}, EQUALS},      {NotEq{}, EQUALS},     {LT{}, LESSGREATER},
    {GT{}, LESSGREATER}, {Plus{}, SUM},         {Minus{}, SUM},
    {Slash{}, PRODUCT},  {Asterisk{}, PRODUCT},
};

Precedence get_precedence(TokenVariant v) {
  auto precedence{precedences[v]};
  return precedence ? precedence : LOWEST;
}

Precedence Parser::peek_predence() { return get_precedence(peek_token.value); }
Precedence Parser::cur_predence() { return get_precedence(cur_token.value); }

Expression *Parser::parse_expression(Precedence precedence) {
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
    left_exp = (this->*infix)(left_exp);
  }

  return left_exp;
}

Expression *Parser::parse_identifier() {
  return new Identifier{std::get<Ident>(cur_token.value).literal};
}

Expression *Parser::parse_integer_literal() {
  return new IntegerLiteral{std::get<Int>(cur_token.value).value};
}

Expression *Parser::parse_boolean_literal() {
  return new BooleanLiteral{cur_token.is_type<True>()};
}

Expression *Parser::parse_prefix_expression() {
  auto oper{cur_token.value};
  next_token();
  auto right{parse_expression(PREFIX)};
  return new PrefixExpression{oper, right};
}

Expression *Parser::parse_infix_expression(Expression *left) {
  auto oper{cur_token.value};
  auto precedence{cur_predence()};
  next_token();
  auto right{parse_expression(precedence)};
  return new InfixExpression{left, oper, right};
}
