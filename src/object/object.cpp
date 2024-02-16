#include "object.hpp"
#include <sstream>

bool HashKey::operator==(const HashKey &other) const {
  return type == other.type && value == other.value;
}

Integer::Integer(IntType value) : value(value) {}
std::string Integer::inspect() const { return std::to_string(value); }
ObjectType Integer::type() const { return ObjectType::INTEGER_OBJ; }
HashKey Integer::hash_key() const { return HashKey{type(), (uint64_t)value}; }

Boolean::Boolean(bool value) : value(value) {}
std::string Boolean::inspect() const { return value ? "true" : "false"; }
ObjectType Boolean::type() const { return ObjectType::BOOLEAN_OBJ; }
HashKey Boolean::hash_key() const {
  return HashKey{type(), (uint64_t)(value ? 1 : 0)};
}

uint64_t fnv64(const std::string &str) {
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;

  uint64_t hash = FNV_OFFSET_BASIS;
  for (char c : str) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
  }

  return hash;
}

String::String(const std::string &value) : value(value) {}
std::string String::inspect() const { return value; }
ObjectType String::type() const { return ObjectType::STRING_OBJ; }
HashKey String::hash_key() const { return HashKey{type(), fnv64(value)}; }

Array::Array(std::vector<std::shared_ptr<Object>> elements)
    : elements(elements) {}
std::string Array::inspect() const {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < elements.size(); i++) {
    ss << elements[i]->inspect();
    if (i < elements.size() - 1) {
      ss << ", ";
    }
  }
  ss << "]";
  return ss.str();
}
ObjectType Array::type() const { return ObjectType::ARRAY_OBJ; }

std::string Null::inspect() const { return "null"; }
ObjectType Null::type() const { return ObjectType::NULL_OBJ; }

ReturnValue::ReturnValue(std::shared_ptr<Object> value) : value(value) {}
std::string ReturnValue::inspect() const { return value->inspect(); }
ObjectType ReturnValue::type() const { return ObjectType::RETURN_VALUE_OBJ; }

Error::Error(std::string message) : value(message) {}
std::string Error::inspect() const { return "ERROR: " + value; }
ObjectType Error::type() const { return ObjectType::ERROR_OBJ; }

namespace std {
std::string to_string(ObjectType t) {
  switch (t) {
  case ObjectType::INTEGER_OBJ:
    return "INTEGER";
  case ObjectType::BOOLEAN_OBJ:
    return "BOOLEAN";
  case ObjectType::STRING_OBJ:
    return "STRING";
  case ObjectType::NULL_OBJ:
    return "NULL";
  case ObjectType::RETURN_VALUE_OBJ:
    return "RETURN";
  case ObjectType::ERROR_OBJ:
    return "ERROR";
  case ObjectType::FUNCTION_OBJ:
    return "FUNCTION";
  case ObjectType::BUILTIN_OBJ:
    return "BUILTIN";
  case ObjectType::ARRAY_OBJ:
    return "ARRAY";
  case ObjectType::HASH_OBJ:
    return "HASH";
  }
}
} // namespace std

Function::Function(std::vector<Identifier> params,
                   std::shared_ptr<BlockStatement> body,
                   std::shared_ptr<Environment> env)
    : params(std::move(params)), body(std::move(body)), env(env) {}
std::string Function::inspect() const {
  std::stringstream ss;
  ss << "fn(";
  for (size_t i = 0; i < params.size(); i++) {
    ss << params[i].to_string();
    if (i < params.size() - 1) {
      ss << ", ";
    }
  }
  ss << ") {\n" << body->to_string() << "\n}";
  return ss.str();
}
ObjectType Function::type() const { return ObjectType::FUNCTION_OBJ; }

Builtin::Builtin(BuiltinFunction fn) : fn(fn) {}
std::string Builtin::inspect() const { return "builtin function"; }
ObjectType Builtin::type() const { return ObjectType::BUILTIN_OBJ; }

Hash::Hash(std::unordered_map<HashKey, HashPair> pairs) : pairs(pairs) {}
std::string Hash::inspect() const {
  std::stringstream ss;
  auto size{pairs.size()};
  size_t i = 0;
  ss << "{";
  for (const auto &k : pairs) {
    ss << k.second.key->inspect() << ": " << k.second.value->inspect();
    if (i < size - 1) {
      ss << ", ";
    }
  }
  ss << "}";
  return ss.str();
}
ObjectType Hash::type() const { return ObjectType::HASH_OBJ; }
