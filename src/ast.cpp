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
    ss << s->to_string();
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

// {{{ BooleanLiteral
BooleanLiteral::BooleanLiteral(bool v) : value(v) {}
std::string BooleanLiteral::token_literal() const { return "BOOL"; }
std::string BooleanLiteral::to_string() const {
  return value ? "true" : "false";
}
// }}}

// {{{ PrefixExpression
PrefixExpression::PrefixExpression(token_types::TokenVariant prefix,
                                   Expression *e)
    : oper(prefix), right(e) {}
std::string PrefixExpression::token_literal() const { return "PREFIX"; }
std::string PrefixExpression::to_string() const {
  auto value{right ? right->to_string() : "<NIL>"};
  return "(" + Token{oper}.literal() + value + ")";
}
// }}}

// {{{ InfixExpression
InfixExpression::InfixExpression(Expression *left,
                                 token_types::TokenVariant prefix,
                                 Expression *right)
    : left(left), oper(prefix), right(right) {}
std::string InfixExpression::token_literal() const { return "INFIX"; }
std::string InfixExpression::to_string() const {
  auto left_val{left ? left->to_string() : "<NIL>"};
  auto right_val{right ? right->to_string() : "<NIL>"};
  return "(" + left_val + " " + Token{oper}.literal() + " " + right_val + ")";
}
// }}}

// {{{ CallExpression
CallExpression::CallExpression(
    std::unique_ptr<Expression> function,
    std::vector<std::unique_ptr<Expression>> &&arguments)
    : function(std::move(function)), arguments(std::move(arguments)) {}
std::string CallExpression::token_literal() const { return "CALL"; }
std::string CallExpression::to_string() const {
  std::stringstream ss;
  ss << token_literal() << "(";
  for (auto &s : arguments) {
    ss << s->to_string();
  }
  ss << ") ";
  return ss.str();
}
// }}}

// {{{ IfExpression
IfExpression::IfExpression(Expression *condition, BlockStatement *consequence,
                           BlockStatement *alternative)
    : condition(condition), consequence(consequence), alternative(alternative) {
}
std::string IfExpression::token_literal() const { return "if"; }
std::string IfExpression::to_string() const {
  return "if" + condition->to_string() + " " + consequence->to_string() +
         (alternative ? ("else " + alternative->to_string()) : "");
}
// }}}

// {{{ LetStatement
LetStatement::LetStatement(Identifier i, Expression *v)
    : identifier(i), value(v) {}
std::string LetStatement::token_literal() const { return "LET"; }
std::string LetStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return token_literal() + " " + identifier.to_string() + " = " + expr;
}
// }}}

// {{{ ReturnStatement
ReturnStatement::ReturnStatement(Expression *v) : value(v) {}
std::string ReturnStatement::token_literal() const { return "RETURN"; }
std::string ReturnStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return token_literal() + " " + expr;
}
// }}}

// {{{ ExpressionStatement
ExpressionStatement::ExpressionStatement(Expression *v) : value(v) {}
std::string ExpressionStatement::token_literal() const { return "EXPRESSION"; }
std::string ExpressionStatement::to_string() const {
  return value ? value->to_string() : "<NIL>";
}
// }}}

// {{{ BlockStatement
std::string BlockStatement::token_literal() const { return "BLOCK"; }
std::string BlockStatement::to_string() const {
  std::stringstream ss;
  for (auto &s : statements) {
    ss << s->to_string();
  }
  return ss.str();
}
BlockStatement::~BlockStatement() {
  for (auto &s : statements) {
    delete s;
  }
}
// }}}

// {{{ FunctionLiteral
FunctionLiteral::FunctionLiteral(std::vector<Identifier> params,
                                 BlockStatement *body)
    : params(params), body(body) {}
std::string FunctionLiteral::token_literal() const { return "FUNCTION"; }
std::string FunctionLiteral::to_string() const {
  std::stringstream ss;
  ss << token_literal() << "(";
  for (auto &s : params) {
    ss << s.to_string();
  }
  ss << ") " << body->to_string();
  return ss.str();
}
// }}}

// vim:foldmethod=marker
