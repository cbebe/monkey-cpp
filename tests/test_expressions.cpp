#include "../src/parser.hpp"
#include "tests.hpp"
#include <any>
#include <iostream>
#include <vector>

template <class Expr, typename T> bool assert_value(Expr &expr, T test_value) {
  if (expr->value != test_value) {
    std::cout << "Failed test - want: " << test_value
              << ". got: " << expr->value << std::endl;
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

bool test_boolean_literal_expression() {
  Parser p{parse_input("true;")};
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
  auto expr{assert_expr_type_statement<BooleanLiteral>(statement, result)};
  if (!result) {
    return false;
  }
  if (!assert_value(expr, true)) {
    return false;
  }

  std::cout << "PASS" << std::endl;
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

template <typename T> struct prefix_test_case {
  std::string input;
  token_types::TokenVariant oper;
  T value;
};

template <typename T, class C>
bool do_prefix_test_case(prefix_test_case<T> test) {
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
  auto expr{assert_expr_type_statement<PrefixExpression>(statement, result)};
  if (!result) {
    return false;
  }
  if (expr->oper != test.oper) {
    std::cout << "Wrong operator. want: " << type_to_string(test.oper)
              << " got: " << type_to_string(expr->oper) << std::endl;
    return false;
  }
  auto right_expr{assert_expr_type<C>(std::move(expr->right), result)};
  if (!result) {
    return false;
  }
  if (!assert_value(right_expr, test.value)) {
    return false;
  }

  return true;
}

bool test_prefix_expression() {
  auto tests = std::vector<std::any>{
      prefix_test_case<int>{"!5;", token_types::Bang{}, 5},
      prefix_test_case<int>{"-15;", token_types::Minus{}, 15},
      prefix_test_case<bool>{"!true;", token_types::Bang{}, true},
      prefix_test_case<bool>{"!false;", token_types::Bang{}, false},
  };

  for (size_t i = 0; i < tests.size(); i++) {
    auto test{tests[i]};
    auto int_test{std::any_cast<prefix_test_case<int>>(&test)};
    bool ret_val{true};
    if (!int_test) {
      auto bool_test{std::any_cast<prefix_test_case<bool>>(&test)};
      if (!do_prefix_test_case<bool, BooleanLiteral>(*bool_test)) {
        ret_val = false;
      }
    } else {
      if (!do_prefix_test_case<int, IntegerLiteral>(*int_test)) {
        ret_val = false;
      }
    }
    if (!ret_val) {
      std::cout << "Failed test case " << i << std::endl;
      return false;
    }
  }
  std::cout << "PASS" << std::endl;
  return true;
}

template <typename T> struct infix_test_case {
  std::string input;
  T left;
  token_types::TokenVariant oper;
  T right;
};

template <typename T, class C>
bool do_infix_test_case(infix_test_case<T> test) {
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

  auto left_expr{assert_expr_type<C>(std::move(expr->left), result)};
  if (!result) {
    return false;
  }
  if (!assert_value(left_expr, test.left)) {
    return false;
  }
  auto right_expr{assert_expr_type<C>(std::move(expr->right), result)};
  if (!result) {
    return false;
  }
  if (!assert_value(right_expr, test.right)) {
    return false;
  }

  return true;
}

bool test_infix_expression() {
  using namespace token_types;
  auto tests = std::vector<std::any>{
      infix_test_case<int>{"3 + 10;", 3, Plus{}, 10},
      infix_test_case<int>{"5 - 5;", 5, Minus{}, 5},
      infix_test_case<int>{"5 * 5;", 5, Asterisk{}, 5},
      infix_test_case<int>{"5 / 5;", 5, Slash{}, 5},
      infix_test_case<int>{"5 > 5;", 5, GT{}, 5},
      infix_test_case<int>{"5 < 5;", 5, LT{}, 5},
      infix_test_case<int>{"5 == 5;", 5, Eq{}, 5},
      infix_test_case<int>{"5 != 5;", 5, NotEq{}, 5},
      infix_test_case<bool>{"true == true", true, Eq{}, true},
      infix_test_case<bool>{"false == false", false, Eq{}, false},
      infix_test_case<bool>{"true != false", true, NotEq{}, false},
  };
  for (size_t i = 0; i < tests.size(); i++) {
    auto test{tests[i]};
    auto int_test{std::any_cast<infix_test_case<int>>(&test)};
    bool ret_val{true};
    if (!int_test) {
      auto bool_test{std::any_cast<infix_test_case<bool>>(&test)};
      if (!do_infix_test_case<bool, BooleanLiteral>(*bool_test)) {
        ret_val = false;
      }
    } else {
      if (!do_infix_test_case<int, IntegerLiteral>(*int_test)) {
        ret_val = false;
      }
    }
    if (!ret_val) {
      std::cout << "Failed test case " << i << std::endl;
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
          "true",
          "true",
      },
      test_case{
          "false",
          "false",
      },
      test_case{
          "3 > 5 == false",
          "((3 > 5) == false)",
      },
      test_case{
          "3 < 5 == true",
          "((3 < 5) == true)",
      },
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
