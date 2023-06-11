#include "builtins.hpp"
#include "evaluator.hpp"
#include <valarray>

std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == STRING_OBJ) {
    auto str{std::static_pointer_cast<String>(args[0])};
    return integer(str->value.length());
  } else if (args[0]->type() == ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    return integer(arr->elements.size());
  }
  return error("argument to `len` not supported, got " +
               std::to_string(args[0]->type()));
}

std::shared_ptr<Object> first(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    if (arr->elements.size() > 0) {
      return arr->elements[0];
    } else {
      return null();
    }
  }
  return error("argument to `first` must be " + std::to_string(ARRAY_OBJ) +
               ", got " + std::to_string(args[0]->type()));
}

std::shared_ptr<Object> last(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    auto size{arr->elements.size()};
    if (size > 0) {
      return arr->elements[size - 1];
    } else {
      return null();
    }
  }
  return error("argument to `last` must be " + std::to_string(ARRAY_OBJ) +
               ", got " + std::to_string(args[0]->type()));
}

std::shared_ptr<Object> rest(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    auto size{arr->elements.size()};
    if (size > 0) {
      return array(std::vector(arr->elements.begin() + 1, arr->elements.end()));
    } else {
      return null();
    }
  }
  return error("argument to `rest` must be " + std::to_string(ARRAY_OBJ) +
               ", got " + std::to_string(args[0]->type()));
}

std::shared_ptr<Object> push(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 2) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=2");
  }
  if (args[0]->type() == ARRAY_OBJ) {
    auto arr{std::static_pointer_cast<Array>(args[0])};
    auto copy{std::vector(arr->elements.begin(), arr->elements.end())};
    copy.push_back(args[1]);
    return array(copy);
  }
  return error("argument to `push` must be " + std::to_string(ARRAY_OBJ) +
               ", got " + std::to_string(args[0]->type()));
}

std::unordered_map<std::string, std::shared_ptr<Builtin>> builtins =
    std::unordered_map<std::string, std::shared_ptr<Builtin>>{
        {"len", std::make_shared<Builtin>(len)},
        {"first", std::make_shared<Builtin>(first)},
        {"last", std::make_shared<Builtin>(last)},
        {"rest", std::make_shared<Builtin>(rest)},
        {"push", std::make_shared<Builtin>(push)},
    };
