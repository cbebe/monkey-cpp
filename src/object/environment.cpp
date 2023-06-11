#include "environment.hpp"
#include <sstream>

Environment::Environment() : outer(nullptr) {}
Environment::Environment(std::shared_ptr<Environment> env) : outer(env) {}

#include <iostream>
std::shared_ptr<Object> Environment::get(const std::string &name) {
  if (store.contains(name)) {
    return store[name];
  } else if (outer) {
    return outer->get(name);
  } else {
    return nullptr;
  }
}
std::shared_ptr<Object> Environment::set(const std::string &name,
                                         std::shared_ptr<Object> object) {
  store[name] = std::move(object);
  return store[name];
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
