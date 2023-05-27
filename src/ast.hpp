#pragma once
#include <string>
#include <variant>

struct Statement {};
struct Expression {};

class Node : std::variant<Statement, Expression> {
  std::string token_literal();
};

class Program : Node {};
