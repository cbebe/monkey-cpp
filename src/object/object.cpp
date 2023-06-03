#include "object.hpp"

Integer::Integer(long value) : value(value) {}
std::string Integer::inspect() const { return std::to_string(value); }
ObjectType Integer::type() const { return INTEGER_OBJ; }

Boolean::Boolean(bool value) : value(value) {}
std::string Boolean::inspect() const { return value ? "true" : "false"; }
ObjectType Boolean::type() const { return BOOLEAN_OBJ; }

std::string Null::inspect() const { return "null"; }
ObjectType Null::type() const { return NULL_OBJ; }

ReturnValue::ReturnValue(std::unique_ptr<Object> value)
    : value(std::move(value)) {}
std::string ReturnValue::inspect() const { return value->inspect(); }
ObjectType ReturnValue::type() const { return RETURN_VALUE_OBJ; }
