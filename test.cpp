#include "tests/tests.hpp"

// Love me some non-syntactic macros
#define TEST(fn)                                                               \
  do {                                                                         \
    if (!fn()) {                                                               \
      return 1;                                                                \
    }                                                                          \
  } while (0)

int main() {
  TEST(first_next_token_test);
  TEST(second_next_token_test);
  TEST(third_next_token_test);
  TEST(test_let_statements);
  TEST(test_return_statements);
  TEST(test_identifier_expression);
  TEST(test_integer_literal_expression);
  TEST(test_prefix_expression);
  TEST(test_infix_expression);

  return 0;
}
