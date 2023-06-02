#include "tests/tests.hpp"

// Love me some non-syntactic macros
#define TEST(fn, res)                                                          \
  do {                                                                         \
    std::cout << #fn << " ";                                                   \
    bool result{fn()};                                                         \
    std::cout << (result ? "PASS" : "FAIL") << std::endl;                      \
    res &= result;                                                             \
  } while (0)

int main() {
  bool pass{true};
  TEST(first_next_token_test, pass);
  TEST(second_next_token_test, pass);
  TEST(third_next_token_test, pass);
  TEST(test_let_statements, pass);
  TEST(test_return_statements, pass);
  TEST(test_identifier_expression, pass);
  TEST(test_integer_literal_expression, pass);
  TEST(test_boolean_literal_expression, pass);
  TEST(test_prefix_expression, pass);
  TEST(test_infix_expression, pass);
  TEST(test_operator_precedence, pass);
  TEST(test_if_expression, pass);
  TEST(test_if_else_expression, pass);
  TEST(test_function_literal_parsing, pass);
  TEST(test_function_parameter_parsing, pass);
  TEST(test_call_expression_parsing, pass);
  TEST(test_call_parameter_parsing, pass);

  return pass ? 0 : 1;
}
