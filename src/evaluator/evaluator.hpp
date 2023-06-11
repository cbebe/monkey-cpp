#pragma once
#include "../ast/ast.hpp"
#include "../object/environment.hpp"
#include "../object/object.hpp"

const Boolean _TRUE = Boolean{true};
const Boolean _FALSE = Boolean{false};
const Null _NULL = Null{};

std::shared_ptr<Array> array(std::vector<std::shared_ptr<Object>> elements);
std::shared_ptr<Hash> array(
    std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>> pairs);
std::shared_ptr<Error> error(std::string message);
std::shared_ptr<Null> null();
std::shared_ptr<Integer> integer(IntType value);
std::shared_ptr<String> string(const std::string &value);
std::shared_ptr<Boolean> boolean(bool value);

std::shared_ptr<Object> eval(std::shared_ptr<Node>,
                             std::shared_ptr<Environment>);
