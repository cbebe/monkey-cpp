#pragma once
#include "../ast/ast.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

enum class ObjectType : uint8_t {
  INTEGER_OBJ,
  BOOLEAN_OBJ,
  STRING_OBJ,
  NULL_OBJ,
  RETURN_VALUE_OBJ,
  ERROR_OBJ,
  FUNCTION_OBJ,
  BUILTIN_OBJ,
  ARRAY_OBJ,
  HASH_OBJ,
};

namespace std {
std::string to_string(ObjectType);
};

class Object {
public:
  virtual std::string inspect() const = 0;
  virtual ObjectType type() const = 0;
};

struct HashKey {
  ObjectType type;
  uint64_t value;

  bool operator==(const HashKey &) const;
};

namespace std {
template <> struct hash<HashKey> {
  std::size_t operator()(const HashKey &k) const {
    std::hash<uint64_t> hash{};
    return hash((uint8_t)k.type) ^ hash(k.value);
  }
};
} // namespace std

class Hashable {
public:
  virtual HashKey hash_key() const = 0;
};

class Integer : public Object, public Hashable {
public:
  Integer(IntType value);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  virtual HashKey hash_key() const override;
  IntType value;
};

class Boolean : public Object, public Hashable {
public:
  Boolean(bool value);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  virtual HashKey hash_key() const override;
  bool value;
};

class String : public Object, public Hashable {
public:
  String(const std::string &);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  virtual HashKey hash_key() const override;
  std::string value;
};

class Array : public Object {
public:
  Array(std::vector<std::shared_ptr<Object>>);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::vector<std::shared_ptr<Object>> elements;
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

struct HashPair {
  std::shared_ptr<Object> key;
  std::shared_ptr<Object> value;
};

class Hash : public Object {
public:
  Hash(std::unordered_map<HashKey, HashPair>);
  virtual std::string inspect() const override;
  virtual ObjectType type() const override;
  std::unordered_map<HashKey, HashPair> pairs;
};
