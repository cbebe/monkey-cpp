#pragma once
#include <string>

enum ObjectType {
  INTEGER_OBJ,
  BOOLEAN_OBJ,
  NULL_OBJ,
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
