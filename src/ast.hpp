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

class Statement : public Node {};
class Expression : public Node {};
// }}}

class Program : virtual Node {
public:
  virtual std::string token_literal() const override;
  std::vector<Statement> statements;
};

class Identifier : virtual Expression {
public:
  virtual std::string token_literal() const override { return "IDENT"; }
  std::string value;
};

class LetStatement : virtual Statement {
public:
  virtual std::string token_literal() const override { return "LET"; }
  Identifier identifier;
  std::unique_ptr<Expression> value;
};

// vim:foldmethod=marker
