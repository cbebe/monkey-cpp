#include "builtins.hpp"
#include "evaluator.hpp"
#include <iostream>

std::shared_ptr<Object> _len(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ObjectType::STRING_OBJ) {
    auto str{std::static_pointer_cast<String>(args[0])};
    return integer(str->value.length());
  } else if (args[0]->type() == ObjectType::ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    return integer(arr->elements.size());
  }
  return error("argument to `len` not supported, got " +
               std::to_string(args[0]->type()));
}

std::shared_ptr<Object> _first(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ObjectType::ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    if (arr->elements.size() > 0) {
      return arr->elements[0];
    } else {
      return null();
    }
  }
  return error("argument to `first` must be " +
               std::to_string(ObjectType::ARRAY_OBJ) + ", got " +
               std::to_string(args[0]->type()));
}

std::shared_ptr<Object> _last(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ObjectType::ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    auto size{arr->elements.size()};
    if (size > 0) {
      return arr->elements[size - 1];
    } else {
      return null();
    }
  }
  return error("argument to `last` must be " +
               std::to_string(ObjectType::ARRAY_OBJ) + ", got " +
               std::to_string(args[0]->type()));
}

std::shared_ptr<Object> _rest(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ObjectType::ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    auto size{arr->elements.size()};
    if (size > 0) {
      return array(std::vector(arr->elements.begin() + 1, arr->elements.end()));
    } else {
      return null();
    }
  }
  return error("argument to `rest` must be " +
               std::to_string(ObjectType::ARRAY_OBJ) + ", got " +
               std::to_string(args[0]->type()));
}

std::shared_ptr<Object> _push(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 2) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=2");
  }
  if (args[0]->type() == ObjectType::ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    auto copy{std::vector(arr->elements.begin(), arr->elements.end())};
    copy.push_back(args[1]);
    return array(copy);
  }
  return error("argument to `push` must be " +
               std::to_string(ObjectType::ARRAY_OBJ) + ", got " +
               std::to_string(args[0]->type()));
}

std::shared_ptr<Object> _puts(std::vector<std::shared_ptr<Object>> args) {
  for (const auto &arg : args) {
    std::cout << arg->inspect() << std::endl;
  }
  return null();
}

std::unordered_map<std::string, std::shared_ptr<Builtin>> builtins =
    std::unordered_map<std::string, std::shared_ptr<Builtin>>{
        {"len", std::make_shared<Builtin>(_len)},
        {"first", std::make_shared<Builtin>(_first)},
        {"last", std::make_shared<Builtin>(_last)},
        {"rest", std::make_shared<Builtin>(_rest)},
        {"push", std::make_shared<Builtin>(_push)},
        {"puts", std::make_shared<Builtin>(_puts)},
    };
