#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

// {{{ Interfaces
class Node {
public:
  virtual std::string token_literal() const = 0;
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
  std::vector<Statement *> statements{};
  ~Program() {
    for (auto &s : statements) {
      delete s;
    }
  }
};

class Identifier : public Expression {
public:
  Identifier(std::string v) : value(v) {}
  virtual std::string token_literal() const override { return "IDENT"; }
  std::string value;
};

class LetStatement : public Statement {
public:
  LetStatement(Identifier i, Expression *v) : identifier(i), value(v) {}
  virtual std::string token_literal() const override { return "LET"; }
  Identifier identifier;
  std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(Expression *v) : value(v) {}
  virtual std::string token_literal() const override { return "RETURN"; }
  std::unique_ptr<Expression> value;
};

// vim:foldmethod=marker
