#include "object.hpp"

Integer::Integer(long value) : value(value) {}
std::string Integer::inspect() const { return std::to_string(value); }
ObjectType Integer::type() const { return INTEGER_OBJ; }

Boolean::Boolean(bool value) : value(value) {}
std::string Boolean::inspect() const { return value ? "true" : "false"; }
ObjectType Boolean::type() const { return BOOLEAN_OBJ; }

std::string Null::inspect() const { return "null"; }
ObjectType Null::type() const { return NULL_OBJ; }

ReturnValue::ReturnValue(std::shared_ptr<Object> value) : value(value) {}
std::string ReturnValue::inspect() const { return value->inspect(); }
ObjectType ReturnValue::type() const { return RETURN_VALUE_OBJ; }

Error::Error(std::string message) : value(message) {}
std::string Error::inspect() const { return "ERROR:" + value; }
ObjectType Error::type() const { return ERROR_OBJ; }

namespace std {
std::string to_string(ObjectType t) {
  switch (t) {
  case INTEGER_OBJ:
    return "INTEGER";
  case BOOLEAN_OBJ:
    return "BOOLEAN";
  case NULL_OBJ:
    return "NULL";
  case RETURN_VALUE_OBJ:
    return "RETURN";
  case ERROR_OBJ:
    return "ERROR";
    break;
  default:
    return "ILLEGAL";
  }
}
} // namespace std
