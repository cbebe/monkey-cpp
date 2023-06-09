#include "test.hpp"

#include "ast/ast.hpp"

bool test_string();

int main() {
  bool pass{true};
  TEST(test_string, pass);
  return pass ? 0 : 1;
}

bool test_string() {
  Identifier lhs{"myVar"};
  Identifier rhs{"anotherVar"};
  auto let_statement{
      std::make_shared<LetStatement>(lhs, std::make_shared<Identifier>(rhs))};
  Program program{};
  program.statements.push_back(std::move(let_statement));
  std::string prog{program.to_string()};
  if (prog != "let myVar = anotherVar;") {
    std::cout << "program.to_string() wrong. got " << prog << std::endl;
    return false;
  }

  return true;
};
