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
IntegerLiteral::IntegerLiteral(long v) : value(v) {}
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

// {{{ StringLiteral
StringLiteral::StringLiteral(const std::string &v) : value(v) {}
std::string StringLiteral::token_literal() const { return value; }
std::string StringLiteral::to_string() const { return value; }
// }}}

// {{{ ArrayLiteral
ArrayLiteral::ArrayLiteral(std::vector<std::shared_ptr<Expression>> v)
    : elements(v) {}
std::string ArrayLiteral::token_literal() const { return "ARRAY"; }
std::string ArrayLiteral::to_string() const {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < elements.size(); ++i) {
    ss << elements[i]->to_string();
    if (i != elements.size() - 1) {
      ss << ", ";
    }
  }
  ss << "]";
  return ss.str();
}
// }}}

// {{{ PrefixExpression
PrefixExpression::PrefixExpression(token_types::TokenVariant prefix,
                                   std::shared_ptr<Expression> e)
    : oper(prefix), right(std::move(e)) {}
std::string PrefixExpression::token_literal() const { return "PREFIX"; }
std::string PrefixExpression::to_string() const {
  return "(" + Token{oper}.literal() + right->to_string() + ")";
}
// }}}

// {{{ IndexExpression
IndexExpression::IndexExpression(std::shared_ptr<Expression> left,
                                 std::shared_ptr<Expression> index)
    : left(std::move(left)), index(std::move(index)) {}
std::string IndexExpression::token_literal() const { return "INDEX"; }
std::string IndexExpression::to_string() const {
  return "(" + left->to_string() + "[" + index->to_string() + "])";
}
// }}}

// {{{ InfixExpression
InfixExpression::InfixExpression(std::shared_ptr<Expression> left,
                                 token_types::TokenVariant prefix,
                                 std::shared_ptr<Expression> right)
    : left(std::move(left)), oper(prefix), right(std::move(right)) {}
std::string InfixExpression::token_literal() const { return "INFIX"; }
std::string InfixExpression::to_string() const {
  return "(" + left->to_string() + " " + Token{oper}.literal() + " " +
         right->to_string() + ")";
}
// }}}

// {{{ CallExpression
CallExpression::CallExpression(
    std::shared_ptr<Expression> function,
    std::vector<std::shared_ptr<Expression>> &&arguments)
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
IfExpression::IfExpression(std::shared_ptr<Expression> condition,
                           std::shared_ptr<BlockStatement> consequence,
                           std::shared_ptr<BlockStatement> alternative)
    : condition(std::move(condition)), consequence(std::move(consequence)),
      alternative(std::move(alternative)) {}
std::string IfExpression::token_literal() const { return "if"; }
std::string IfExpression::to_string() const {
  return "if (" + condition->to_string() + ") {\n" + consequence->to_string() +
         "\n}" +
         (alternative ? (" else {\n" + alternative->to_string() + "\n}") : "");
}
// }}}

// {{{ LetStatement
LetStatement::LetStatement(Identifier i, std::shared_ptr<Expression> v)
    : identifier(i), value(std::move(v)) {}
std::string LetStatement::token_literal() const { return "let"; }
std::string LetStatement::to_string() const {
  return token_literal() + " " + identifier.to_string() + " = " +
         value->to_string() + ";";
}
// }}}

// {{{ ReturnStatement
ReturnStatement::ReturnStatement(std::shared_ptr<Expression> v)
    : value(std::move(v)) {}
std::string ReturnStatement::token_literal() const { return "RETURN"; }
std::string ReturnStatement::to_string() const {
  return token_literal() + " " + value->to_string() + ";";
}
// }}}

// {{{ ExpressionStatement
ExpressionStatement::ExpressionStatement(std::shared_ptr<Expression> v)
    : value(std::move(v)) {}
std::string ExpressionStatement::token_literal() const { return "EXPRESSION"; }
std::string ExpressionStatement::to_string() const {
  return value->to_string();
}
// }}}

// {{{ BlockStatement
std::string BlockStatement::token_literal() const { return "BLOCK"; }
std::string BlockStatement::to_string() const {
  std::stringstream ss;
  for (auto &s : statements) {
    ss << s->to_string() << std::endl;
  }
  return ss.str();
}
// }}}

// {{{ FunctionLiteral
FunctionLiteral::FunctionLiteral(std::vector<Identifier> params,
                                 std::shared_ptr<BlockStatement> body)
    : params(params), body(std::move(body)) {}
std::string FunctionLiteral::token_literal() const { return "FUNCTION"; }
std::string FunctionLiteral::to_string() const {
  std::stringstream ss;
  ss << "fn(";
  for (size_t i = 0; i < params.size(); i++) {
    ss << params[i].to_string();
    if (i < params.size() - 1) {
      ss << ", ";
    }
  }
  ss << ") {\n" << body->to_string() << "}";
  return ss.str();
}
// }}}

// vim:foldmethod=marker
