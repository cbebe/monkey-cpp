#include "ast.hpp"
#include <iostream>

bool test_string() {
  Identifier lhs{"myVar"};
  Identifier rhs{"anotherVar"};
  auto let_statement{std::make_unique<LetStatement>(
      LetStatement{lhs, std::make_unique<Identifier>(rhs)})};
  Program program{};
  program.statements.push_back(std::move(let_statement));
  std::string prog{program.to_string()};
  if (prog != "let myVar = anotherVar;") {
    std::cout << "program.to_string() wrong. got " << prog << std::endl;
    return false;
  }

  return true;
};
