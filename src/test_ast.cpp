#include "ast/ast_test.hpp"
#include "test.hpp"

int main() {
  bool pass{true};
  TEST(test_string, pass);
  return pass ? 0 : 1;
}
