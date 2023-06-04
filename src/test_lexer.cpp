#include "lexer/lexer_test.hpp"

#include "test.hpp"

int main() {
  bool pass{true};
  TEST(test_next_token, pass);
  return pass ? 0 : 1;
}
