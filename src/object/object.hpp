#pragma once
#include "../ast/ast.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

enum ObjectType {
  INTEGER_OBJ,
  BOOLEAN_OBJ,
  STRING_OBJ,
  NULL_OBJ,
  RETURN_VALUE_OBJ,
  ERROR_OBJ,
  FUNCTION_OBJ,
  BUILTIN_OBJ,
};

namespace std {
std::string to_string(ObjectType);
};

class Object {
public:
  virtual std::string inspect() const = 0;
  virtual ObjectType type() const = 0;
};

class Integer : public Object {
public:
  Integer(long value);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  long value;
};

class Boolean : public Object {
public:
  Boolean(bool value);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  bool value;
};

class String : public Object {
public:
  String(const std::string &);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::string value;
};

class Null : public Object {
public:
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
};

class ReturnValue : public Object {
public:
  ReturnValue(std::shared_ptr<Object>);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::shared_ptr<Object> value;
};

class Error : public Object {
public:
  Error(std::string);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::string value;
};

class Environment;

class Function : public Object {
public:
  Function(std::vector<Identifier>, std::shared_ptr<BlockStatement>,
           std::shared_ptr<Environment>);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::vector<Identifier> params;
  std::shared_ptr<BlockStatement> body;
  std::shared_ptr<Environment> env;
};

using BuiltinFunction = std::function<std::shared_ptr<Object>(
    std::vector<std::shared_ptr<Object>>)>;

class Builtin : public Object {
public:
  Builtin(BuiltinFunction);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  BuiltinFunction fn;
};
