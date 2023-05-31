#pragma once
#include "token.hpp"
#include <memory>
#include <vector>

// {{{ Interfaces
class Node {
public:
  virtual std::string token_literal() const = 0;
  virtual std::string to_string() const = 0;
};

class Statement : public Node {
public:
  virtual ~Statement() = default;
};
class Expression : public Node {};
// }}}

class Program : virtual Node {
public:
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<Statement *> statements{};
  ~Program();
};

class Identifier : public Expression {
public:
  Identifier(std::string);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::string value;
};

class IntegerLiteral : public Expression {
public:
  IntegerLiteral(int);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  int value;
};

class PrefixExpression : public Expression {
public:
  PrefixExpression(token_types::TokenVariant, Expression *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  token_types::TokenVariant oper;
  std::unique_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
  InfixExpression(Expression *, token_types::TokenVariant, Expression *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> left;
  token_types::TokenVariant oper;
  std::unique_ptr<Expression> right;
};

class LetStatement : public Statement {
public:
  LetStatement(Identifier, Expression *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  Identifier identifier;
  std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(Expression *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> value;
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(Expression *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> value;
};

// vim:foldmethod=marker
