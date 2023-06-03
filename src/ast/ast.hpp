#pragma once
#include "../token/token.hpp"
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
class Expression : public Node {
public:
  virtual ~Expression() = default;
};
// }}}
class Program : public Node {
public:
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<std::unique_ptr<Statement>> statements{};
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
  IntegerLiteral(long);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  long value;
};

class PrefixExpression : public Expression {
public:
  PrefixExpression(token_types::TokenVariant, std::unique_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  token_types::TokenVariant oper;
  std::unique_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
  InfixExpression(std::unique_ptr<Expression>, token_types::TokenVariant,
                  std::unique_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> left;
  token_types::TokenVariant oper;
  std::unique_ptr<Expression> right;
};

class CallExpression : public Expression {
public:
  CallExpression(std::unique_ptr<Expression>,
                 std::vector<std::unique_ptr<Expression>> &&);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> function;
  std::vector<std::unique_ptr<Expression>> arguments;
};
// }}}

// {{{ Statements
class LetStatement : public Statement {
public:
  LetStatement(Identifier, std::unique_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  Identifier identifier;
  std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(std::unique_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> value;
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(std::unique_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> value;
};

class BlockStatement : public Statement {
public:
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<std::unique_ptr<Statement>> statements{};
};
// }}}

// {{{ Expressions that need BlockStatement definition
class IfExpression : public Expression {
public:
  IfExpression(std::unique_ptr<Expression>, std::unique_ptr<BlockStatement>,
               std::unique_ptr<BlockStatement>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> condition;
  std::unique_ptr<BlockStatement> consequence;
  std::unique_ptr<BlockStatement> alternative;
};

class FunctionLiteral : public Expression {
public:
  FunctionLiteral(std::vector<Identifier>, std::unique_ptr<BlockStatement>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<Identifier> params;
  std::unique_ptr<BlockStatement> body;
};
// }}}

// vim:foldmethod=marker