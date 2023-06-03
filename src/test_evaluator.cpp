#include "evaluator/evaluator_test.hpp"
#include "test.hpp"

int main() {
  bool pass{true};
  TEST(test_eval_integer_expression, pass);
  TEST(test_eval_boolean_expression, pass);
  TEST(test_bang_operator, pass);
  TEST(test_if_else_expressions, pass);
  TEST(test_return_statements, pass);
  TEST(test_error_handling, pass);
  TEST(test_let_statements, pass);
  return pass ? 0 : 1;
}
