#include "../parser.hpp"

using namespace token_types;

Expression *Parser::parse_expression(Precedence) {
  auto prefix = prefix_parse_fns[cur_token.value];
  if (!prefix) {
    errors.push_back("no prefix parse function for " + cur_token.to_string() +
                     " found");
    return nullptr;
  }
  auto left_exp{(this->*prefix)()};

  return left_exp;
}

Expression *Parser::parse_identifier() {
  return new Identifier{std::get<Ident>(cur_token.value).literal};
}

Expression *Parser::parse_integer_literal() {
  return new IntegerLiteral{std::get<Int>(cur_token.value).value};
}

Expression *Parser::parse_prefix_expression() {
  auto oper{cur_token.value};
  next_token();
  auto right{parse_expression(PREFIX)};
  return new PrefixExpression{oper, right};
}
