#include "environment.hpp"
#include <sstream>

Environment::Environment() : outer(nullptr) {}
Environment::Environment(std::shared_ptr<Environment> env)
    : outer(std::move(env)) {}

#include <iostream>
std::shared_ptr<Object> Environment::get(const std::string &name) {
  auto obj{store[name]};
  if (!obj && outer) {
    if (auto outer_val{outer->get(name)}) {
      return outer_val;
    } else {
      std::cout << "WTF" << std::endl;
      return std::make_shared<Null>();
    }
  }
  return obj;
}
std::shared_ptr<Object> Environment::set(const std::string &name,
                                         std::shared_ptr<Object> object) {
  store[name] = object;
  return object;
}

std::string Environment::inspect() {
  std::stringstream ss;
  for (const auto &k : store) {
    ss << k.first << " = " << k.second->inspect() << std::endl;
  }
  if (outer) {
    for (const auto &k : outer->store) {
      ss << k.first << " = " << k.second->inspect() << std::endl;
    }
  }
  return ss.str();
}
