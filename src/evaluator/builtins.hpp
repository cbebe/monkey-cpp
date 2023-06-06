#pragma once
#include "../object/object.hpp"
#include <string>
#include <unordered_map>

extern std::unordered_map<std::string, std::shared_ptr<Builtin>> builtins;
