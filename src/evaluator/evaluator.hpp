#pragma once
#include "../ast/ast.hpp"
#include "../object/environment.hpp"
#include "../object/object.hpp"

const Boolean _TRUE = Boolean{true};
const Boolean _FALSE = Boolean{false};
const Null _NULL = Null{};

std::shared_ptr<Object> array(std::vector<std::shared_ptr<Object>> elements);
std::shared_ptr<Object> error(std::string message);
std::shared_ptr<Object> null();
std::shared_ptr<Object> integer(long value);

std::shared_ptr<Object> eval(std::shared_ptr<Node>,
                             std::shared_ptr<Environment>);
