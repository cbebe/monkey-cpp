#include "../src/parser.hpp"
#include "tests.hpp"
#include <iostream>
#include <vector>

template <class Expr, typename T> bool assert_value(Expr &expr, T test_value) {
  if (expr->value != test_value) {
    std::cout << "Failed test: not " << test_value << std::endl;
    return false;
  }
  return true;
}

template <typename To>
std::unique_ptr<To, std::default_delete<Expression>>
assert_expr_type(std::unique_ptr<Expression> test_expr, bool &result) {
  auto expr{dynamic_unique_cast<To>(std::move(test_expr))};
  if (!expr) {
    std::cout << "Failed test: not " << typeid(To).name() << std::endl;
    result = false;
  }
  result = true;
  return expr;
}

template <typename To>
std::unique_ptr<To, std::default_delete<Expression>>
assert_expr_type_statement(ExpressionStatement *statement, bool &result) {
  return assert_expr_type<To>(std::move(statement->value), result);
}

bool test_identifier_expression() {
  Parser p{parse_input("foobar;")};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }

  ExpressionStatement *statement;
  if (!assert_type<ExpressionStatement>(program->statements[0], statement)) {
    return false;
  }
  bool result;
  auto expr{assert_expr_type_statement<Identifier>(statement, result)};
  if (!result) {
    return false;
  }

  if (!assert_value(expr, "foobar")) {
    return false;
  }
  return true;
}

bool test_integer_literal_expression() {
  Parser p{parse_input("5;")};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }

  ExpressionStatement *statement;
  if (!assert_type<ExpressionStatement>(program->statements[0], statement)) {
    return false;
  }
  bool result;
  auto expr{assert_expr_type_statement<IntegerLiteral>(statement, result)};
  if (!result) {
    return false;
  }
  if (!assert_value(expr, 5)) {
    return false;
  }

  std::cout << "PASS" << std::endl;
  return true;
}

bool test_prefix_expression() {
  struct prefix_test_case {
    std::string input;
    token_types::TokenVariant oper;
    int value;
  };
  auto tests = std::vector{prefix_test_case{"!5;", token_types::Bang{}, 5},
                           prefix_test_case{"-15;", token_types::Minus{}, 15}};
  for (size_t i = 0; i < 2; i++) {
    Parser p{parse_input(tests[i].input)};
    auto program{p.parse_program()};
    program = parser_pre_checks(p, std::move(program), 1);
    if (!program) {
      return false;
    }
    ExpressionStatement *statement;
    if (!assert_type<ExpressionStatement>(program->statements[0], statement)) {
      return false;
    }

    bool result;
    auto expr{assert_expr_type_statement<PrefixExpression>(statement, result)};
    if (!result) {
      return false;
    }
    if (expr->oper != tests[i].oper) {
      std::cout << "Wrong operator. want: " << type_to_string(tests[i].oper)
                << " got: " << type_to_string(expr->oper) << std::endl;
      return false;
    }
    auto right_expr{
        assert_expr_type<IntegerLiteral>(std::move(expr->right), result)};
    if (!result) {
      return false;
    }
    if (!assert_value(right_expr, tests[i].value)) {
      return false;
    }
  }
  std::cout << "PASS" << std::endl;
  return true;
}

bool test_infix_expression() {
  struct infix_test_case {
    std::string input;
    int left;
    token_types::TokenVariant oper;
    int right;
  };
  using namespace token_types;
  auto tests = std::vector{
      infix_test_case{"5 + 5;", 5, Plus{}, 5},
      infix_test_case{"5 - 5;", 5, Minus{}, 5},
      infix_test_case{"5 * 5;", 5, Asterisk{}, 5},
      infix_test_case{"5 / 5;", 5, Slash{}, 5},
      infix_test_case{"5 > 5;", 5, GT{}, 5},
      infix_test_case{"5 < 5;", 5, LT{}, 5},
      infix_test_case{"5 == 5;", 5, Eq{}, 5},
      infix_test_case{"5 != 5;", 5, NotEq{}, 5},
  };
  for (auto test : tests) {
    Parser p{parse_input(test.input)};
    auto program{p.parse_program()};
    program = parser_pre_checks(p, std::move(program), 1);
    if (!program) {
      return false;
    }
    ExpressionStatement *statement;
    if (!assert_type<ExpressionStatement>(program->statements[0], statement)) {
      return false;
    }

    bool result;
    auto expr{assert_expr_type_statement<InfixExpression>(statement, result)};
    if (!result) {
      return false;
    }
    if (expr->oper != test.oper) {
      std::cout << "Wrong operator. want: " << type_to_string(test.oper)
                << " got: " << type_to_string(expr->oper) << std::endl;
      return false;
    }

    auto left_expr{
        assert_expr_type<IntegerLiteral>(std::move(expr->left), result)};
    if (!result) {
      return false;
    }
    if (!assert_value(left_expr, test.right)) {
      return false;
    }
    auto right_expr{
        assert_expr_type<IntegerLiteral>(std::move(expr->right), result)};
    if (!result) {
      return false;
    }
    if (!assert_value(right_expr, test.right)) {
      return false;
    }
  }
  std::cout << "PASS" << std::endl;
  return true;
}

bool test_operator_precedence() {
  struct test_case {
    std::string input;
    std::string expected;
  };

  auto tests{std::vector{
      test_case{
          "-a * b",
          "((-a) * b)",
      },
      test_case{
          "!-a",
          "(!(-a))",
      },
      test_case{
          "a + b + c",
          "((a + b) + c)",
      },
      test_case{
          "a + b - c",
          "((a + b) - c)",
      },
      test_case{
          "a * b * c",
          "((a * b) * c)",
      },
      test_case{
          "a * b / c",
          "((a * b) / c)",
      },
      test_case{
          "a + b / c",
          "(a + (b / c))",
      },
      test_case{
          "a + b * c + d / e - f",
          "(((a + (b * c)) + (d / e)) - f)",
      },
      test_case{
          "3 + 4; -5 * 5",
          "(3 + 4)((-5) * 5)",
      },
      test_case{
          "5 > 4 == 3 < 4",
          "((5 > 4) == (3 < 4))",
      },
      test_case{
          "5 < 4 != 3 > 4",
          "((5 < 4) != (3 > 4))",
      },
      test_case{
          "3 + 4 * 5 == 3 * 1 + 4 * 5",
          "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
      },
  }};

  for (size_t i = 0; i < tests.size(); i++) {
    auto t{tests[i]};
    Parser p{parse_input(t.input)};
    auto program{p.parse_program()};
    if (!program) {
      std::cout << i << std::endl;
      return false;
    }
    auto prog_str{program->to_string()};
    if (prog_str != t.expected) {
      std::cout << "Failed test " << i << " - want: '" << t.expected
                << "' got: '" << prog_str << "'" << std::endl;
      return false;
    }
  }
  std::cout << "PASS" << std::endl;
  return true;
}
