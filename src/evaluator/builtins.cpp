#include "builtins.hpp"
#include "evaluator.hpp"

std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> args) {
  if (args.size() != 1) {
    return error("wrong number of arguments. got=" +
                 std::to_string(args.size()) + ", want=1");
  }
  if (args[0]->type() == STRING_OBJ) {
    auto str{std::static_pointer_cast<String>(args[0])};
    return integer(str->value.length());
  }
  return error("argument to `len` not supported, got " +
               std::to_string(args[0]->type()));
}

std::unordered_map<std::string, std::shared_ptr<Builtin>> builtins =
    std::unordered_map<std::string, std::shared_ptr<Builtin>>{
        {"len", std::make_shared<Builtin>(len)}};
