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
                                   std::unique_ptr<Expression> e)
    : oper(prefix), right(std::move(e)) {}
std::string PrefixExpression::token_literal() const { return "PREFIX"; }
std::string PrefixExpression::to_string() const {
  auto value{right ? right->to_string() : "<NIL>"};
  return "(" + Token{oper}.literal() + value + ")";
}
// }}}

// {{{ InfixExpression
InfixExpression::InfixExpression(std::unique_ptr<Expression> left,
                                 token_types::TokenVariant prefix,
                                 std::unique_ptr<Expression> right)
    : left(std::move(left)), oper(prefix), right(std::move(right)) {}
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
  ss << function->to_string() << "(";
  for (size_t i = 0; i < arguments.size(); ++i) {
    ss << arguments[i]->to_string();
    if (i != arguments.size() - 1) {
      ss << ", ";
    }
  }
  ss << ")";
  return ss.str();
}
// }}}

// {{{ IfExpression
IfExpression::IfExpression(std::unique_ptr<Expression> condition,
                           std::unique_ptr<BlockStatement> consequence,
                           std::unique_ptr<BlockStatement> alternative)
    : condition(std::move(condition)), consequence(std::move(consequence)),
      alternative(std::move(alternative)) {}
std::string IfExpression::token_literal() const { return "if"; }
std::string IfExpression::to_string() const {
  return "if" + condition->to_string() + " " + consequence->to_string() +
         (alternative ? ("else " + alternative->to_string()) : "");
}
// }}}

// {{{ LetStatement
LetStatement::LetStatement(Identifier i, std::unique_ptr<Expression> v)
    : identifier(i), value(std::move(v)) {}
std::string LetStatement::token_literal() const { return "LET"; }
std::string LetStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return token_literal() + " " + identifier.to_string() + " = " + expr;
}
// }}}

// {{{ ReturnStatement
ReturnStatement::ReturnStatement(std::unique_ptr<Expression> v)
    : value(std::move(v)) {}
std::string ReturnStatement::token_literal() const { return "RETURN"; }
std::string ReturnStatement::to_string() const {
  auto expr = value ? value->to_string() : "<NIL>";
  return token_literal() + " " + expr;
}
// }}}

// {{{ ExpressionStatement
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> v)
    : value(std::move(v)) {}
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
// }}}

// {{{ FunctionLiteral
FunctionLiteral::FunctionLiteral(std::vector<Identifier> params,
                                 std::unique_ptr<BlockStatement> body)
    : params(params), body(std::move(body)) {}
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
