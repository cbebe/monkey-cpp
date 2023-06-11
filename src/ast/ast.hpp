#pragma once
#include "../token/token.hpp"
#include <memory>
#include <unordered_map>
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
  std::vector<std::shared_ptr<Statement>> statements{};
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
  IntegerLiteral(IntType);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  IntType value;
};

class StringLiteral : public Expression {
public:
  StringLiteral(const std::string &);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::string value;
};

class ArrayLiteral : public Expression {
public:
  ArrayLiteral(std::vector<std::shared_ptr<Expression>>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<std::shared_ptr<Expression>> elements;
};

class HashLiteral : public Expression {
public:
  HashLiteral(std::unordered_map<std::shared_ptr<Expression>,
                                 std::shared_ptr<Expression>>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unordered_map<std::shared_ptr<Expression>, std::shared_ptr<Expression>>
      pairs;
};

class PrefixExpression : public Expression {
public:
  PrefixExpression(token_types::TokenVariant, std::shared_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  token_types::TokenVariant oper;
  std::shared_ptr<Expression> right;
};

class IndexExpression : public Expression {
public:
  IndexExpression(std::shared_ptr<Expression> left,
                  std::shared_ptr<Expression> index);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> index;
};

class InfixExpression : public Expression {
public:
  InfixExpression(std::shared_ptr<Expression>, token_types::TokenVariant,
                  std::shared_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::shared_ptr<Expression> left;
  token_types::TokenVariant oper;
  std::shared_ptr<Expression> right;
};

class CallExpression : public Expression {
public:
  CallExpression(std::shared_ptr<Expression>,
                 std::vector<std::shared_ptr<Expression>> &&);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::shared_ptr<Expression> function;
  std::vector<std::shared_ptr<Expression>> arguments;
};

// Forward decl
class BlockStatement;

class IfExpression : public Expression {
public:
  IfExpression(std::shared_ptr<Expression>, std::shared_ptr<BlockStatement>,
               std::shared_ptr<BlockStatement>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::shared_ptr<Expression> condition;
  std::shared_ptr<BlockStatement> consequence;
  std::shared_ptr<BlockStatement> alternative;
};

class FunctionLiteral : public Expression {
public:
  FunctionLiteral(std::vector<Identifier>, std::shared_ptr<BlockStatement>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<Identifier> params;
  std::shared_ptr<BlockStatement> body;
};
// }}}

// {{{ Statements
class LetStatement : public Statement {
public:
  LetStatement(Identifier, std::shared_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  Identifier identifier;
  std::shared_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(std::shared_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::shared_ptr<Expression> value;
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(std::shared_ptr<Expression>);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::shared_ptr<Expression> value;
};

class BlockStatement : public Statement {
public:
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::vector<std::shared_ptr<Statement>> statements{};
};
// }}}

// vim:foldmethod=marker
