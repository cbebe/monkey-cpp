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
using ExprSubtype = std::unique_ptr<To, std::default_delete<Expression>>;

template <typename To>
ExprSubtype<To> assert_expr_type(std::unique_ptr<Expression> test_expr,
                                 bool &result) {
  auto expr{dynamic_unique_cast<To>(std::move(test_expr))};
  if (!expr) {
    std::cout << "Failed test: not " << typeid(To).name() << std::endl;
    result = false;
  }
  result = true;
  return expr;
}

template <typename To>
ExprSubtype<To> assert_expr_type_statement(ExpressionStatement *statement,
                                           bool &result) {
  return assert_expr_type<To>(std::move(statement->value), result);
}

bool test_single_identifier_expression_statement(Statement *gen,
                                                 std::string value) {
  ExpressionStatement *statement;
  if (!assert_type<ExpressionStatement>(gen, statement)) {
    return false;
  }
  bool result;
  auto expr{assert_expr_type_statement<Identifier>(statement, result)};
  if (!result) {
    return false;
  }

  if (!assert_value(expr, value)) {
    return false;
  }

  return true;
}

bool test_identifier_expression() {
  Parser p{parse_input("foobar;")};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }

  return test_single_identifier_expression_statement(program->statements[0],
                                                     "foobar");
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
  return true;
}

template <typename T> struct infix_test_case {
  std::string input;
  T left;
  token_types::TokenVariant oper;
  T right;
};

template <typename T, class C>
bool test_single_infix_expression(ExprSubtype<InfixExpression> expr, T left,
                                  token_types::TokenVariant oper, T right) {

  if (expr->oper != oper) {
    std::cout << "Wrong operator. want: " << type_to_string(oper)
              << " got: " << type_to_string(expr->oper) << std::endl;
    return false;
  }
  bool result{true};

  auto left_expr{assert_expr_type<C>(std::move(expr->left), result)};
  if (!result) {
    return false;
  }
  if (!assert_value(left_expr, left)) {
    return false;
  }
  auto right_expr{assert_expr_type<C>(std::move(expr->right), result)};
  if (!result) {
    return false;
  }
  if (!assert_value(right_expr, right)) {
    return false;
  }

  return true;
}

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
  return test_single_infix_expression<T, C>(std::move(expr), test.left,
                                            test.oper, test.right);
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
      test_case{
          "1 + (2 + 3) + 4",
          "((1 + (2 + 3)) + 4)",
      },
      test_case{
          "(5 + 5) * 2",
          "((5 + 5) * 2)",
      },
      test_case{
          "2 / (5 + 5)",
          "(2 / (5 + 5))",
      },
      test_case{
          "-(5 + 5)",
          "(-(5 + 5))",
      },
      test_case{
          "!(true == true)",
          "(!(true == true))",
      },
  }};

  for (size_t i = 0; i < tests.size(); i++) {
    auto t{tests[i]};
    Parser p{parse_input(t.input)};
    auto program{p.parse_program()};
    bool pass{true};
    program = check_errors(p, std::move(program), pass);
    if (!pass) {
      return false;
    }
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

  return true;
}

ExprSubtype<IfExpression> test_single_if_expression(std::string input) {
  Parser p{parse_input(input)};
  auto program{p.parse_program()};
  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return nullptr;
  }
  ExpressionStatement *statement;
  if (!assert_type<ExpressionStatement>(program->statements[0], statement)) {
    return nullptr;
  }
  bool result{true};
  auto expr{assert_expr_type_statement<IfExpression>(statement, result)};
  if (!result) {
    return nullptr;
  }
  auto infix_expr{
      assert_expr_type<InfixExpression>(std::move(expr->condition), result)};
  if (!result) {
    return nullptr;
  }
  result = test_single_infix_expression<std::string, Identifier>(
      std::move(infix_expr), "x", token_types::LT{}, "y");
  if (!result) {
    return nullptr;
  }
  size_t length{expr->consequence->statements.size()};
  if (length != 1) {
    std::cout << "Incorrect number of statements. got " << length << " want "
              << 1 << std::endl;
    return nullptr;
  }
  if (!test_single_identifier_expression_statement(
          expr->consequence->statements[0], "x")) {
    return nullptr;
  }

  return expr;
}

bool test_if_expression() {
  auto expr{test_single_if_expression("if (x < y) { x }")};
  if (!expr) {
    return false;
  }
  if (expr->alternative) {
    std::cout << "Alternative statements was not null" << std::endl;
    return false;
  }
  return true;
}

bool test_if_else_expression() {
  auto expr{test_single_if_expression("if (x < y) { x } else { y }")};
  if (!expr) {
    return false;
  }
  if (!expr->alternative) {
    std::cout << "Alternative statements was null" << std::endl;
    return false;
  }
  size_t length{expr->alternative->statements.size()};
  if (length != 1) {
    std::cout << "Incorrect number of statements. got " << length << " want "
              << 1 << std::endl;
    return false;
  }
  if (!test_single_identifier_expression_statement(
          expr->alternative->statements[0], "y")) {
    return false;
  }

  return true;
}
