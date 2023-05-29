#pragma once
#include <memory>
#include <string>
#include <variant>
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
  Identifier(std::string v);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::string value;
};

class LetStatement : public Statement {
public:
  LetStatement(Identifier i, Expression *v);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  Identifier identifier;
  std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(Expression *v);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> value;
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(Expression *v);
  virtual std::string token_literal() const override;
  virtual std::string to_string() const override;
  std::unique_ptr<Expression> value;
};

// vim:foldmethod=marker
