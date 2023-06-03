#include "parser/parser_test.hpp"

#include "test.hpp"

int main() {
  bool pass{true};
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
