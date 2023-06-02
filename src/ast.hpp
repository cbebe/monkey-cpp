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

// {{{ Expresssions
class Identifier : public Expression {
public:
  Identifier(std::string);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::string value;
};

class BooleanLiteral : public Expression {
public:
  BooleanLiteral(bool);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  bool value;
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

// }}}

// {{{ Statements
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

class BlockStatement : public Statement {
public:
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  ~BlockStatement();
  std::vector<Statement *> statements{};
};
// }}}

// {{{ IfExpression (needs BlockStatement definition)
class IfExpression : public Expression {
public:
  IfExpression(Expression *, BlockStatement *, BlockStatement *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> condition;
  std::unique_ptr<BlockStatement> consequence;
  std::unique_ptr<BlockStatement> alternative;
};
// }}}

// {{{ FunctionLiteral
class FunctionLiteral : public Expression {
public:
  FunctionLiteral(BlockStatement *);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<Identifier *> params{};
  std::unique_ptr<BlockStatement> body;
};
// }}}

// vim:foldmethod=marker
