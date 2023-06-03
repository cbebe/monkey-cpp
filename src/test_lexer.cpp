#include "lexer/lexer_test.hpp"

#include "test.hpp"

int main() {
  bool pass{true};
  TEST(first_next_token_test, pass);
  TEST(second_next_token_test, pass);
  TEST(third_next_token_test, pass);
  return pass ? 0 : 1;
}
