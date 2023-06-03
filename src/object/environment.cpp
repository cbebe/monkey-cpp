#include "environment.hpp"

std::shared_ptr<Object> Environment::get(const std::string &name) {
  return store[name];
}
std::shared_ptr<Object> Environment::set(const std::string &name,
                                         std::shared_ptr<Object> object) {
  store[name] = object;
  return object;
}
