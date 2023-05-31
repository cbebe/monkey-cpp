#include "ast.hpp"
#include <sstream>

// {{{ Program

std::string Program::token_literal() const {
  std::stringstream ss;
  for (const auto &s : statements) {
    ss << s->token_literal() << std::endl;
  }
  return ss.str();
}

std::string Program::to_string() const {
  std::stringstream ss;
  for (const auto &s : statements) {
    ss << s->to_string() << std::endl;
  }
  return ss.str();
}

Program::~Program() {
  for (auto &s : statements) {
    delete s;
  }
}

// }}}

// {{{ Identifier
Identifier::Identifier(std::string v) : value(v) {}
std::string Identifier::token_literal() const { return "IDENT"; }
std::string Identifier::to_string() const { return value; }
// }}}

// {{{ IntegerLiteral
IntegerLiteral::IntegerLiteral(int v) : value(v) {}
std::string IntegerLiteral::token_literal() const { return "INT"; }
std::string IntegerLiteral::to_string() const { return std::to_string(value); }
// }}}

// {{{ PrefixExpression
PrefixExpression::PrefixExpression(token_types::TokenVariant prefix,
                                   Expression *e)
    : oper(prefix), right(e) {}
std::string PrefixExpression::token_literal() const { return "PREFIX"; }
std::string PrefixExpression::to_string() const {
  auto value{right ? right->to_string() : "<NIL>"};
  return "(" + Token{oper}.to_string() + value + ")";
}
// }}}

// {{{ LetStatement
LetStatement::LetStatement(Identifier i, Expression *v)
    : identifier(i), value(v) {}
std::string LetStatement::token_literal() const { return "LET"; }
std::string LetStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return token_literal() + " " + identifier.to_string() + " = " + expr + ";";
}
// }}}

// {{{ Return Statement
ReturnStatement::ReturnStatement(Expression *v) : value(v) {}
std::string ReturnStatement::token_literal() const { return "RETURN"; }
std::string ReturnStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return token_literal() + " " + expr + ";";
}
// }}}

// {{{ Expression Statement
ExpressionStatement::ExpressionStatement(Expression *v) : value(v) {}
std::string ExpressionStatement::token_literal() const { return "EXPRESSION"; }
std::string ExpressionStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return expr + ";";
}
// }}}

// vim:foldmethod=marker
