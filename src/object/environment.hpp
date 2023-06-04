#pragma once
#include "object.hpp"
#include <string>
#include <unordered_map>

class Environment {
public:
  Environment();
  Environment(std::shared_ptr<Environment> env);
  std::string inspect();
  std::shared_ptr<Object> get(const std::string &);
  std::shared_ptr<Object> set(const std::string &, std::shared_ptr<Object>);

private:
  std::unordered_map<std::string, std::shared_ptr<Object>> store{};
  std::shared_ptr<Environment> outer;
};
