#pragma once
#include <memory>
#include <string>

enum ObjectType {
  INTEGER_OBJ,
  BOOLEAN_OBJ,
  NULL_OBJ,
  RETURN_VALUE_OBJ,
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

class Null : public Object {
public:
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
};

class ReturnValue : public Object {
public:
  ReturnValue(std::unique_ptr<Object>);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::unique_ptr<Object> value;
};
