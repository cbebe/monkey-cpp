#include "../src/parser.hpp"
#include "tests.hpp"
#include <any>
#include <iostream>
#include <vector>

bool test_identifier_expression() {
  return test_single_expression_program<Identifier>("foobar;", "foobar");
}

bool test_boolean_literal_expression() {
  return test_single_expression_program<BooleanLiteral>("true;", true);
}

bool test_integer_literal_expression() {
  return test_single_expression_program<IntegerLiteral>("5;", 5);
}

template <typename T> struct prefix_test_case {
  std::string input;
  token_types::TokenVariant oper;
  T value;
};

template <typename T, class C>
bool do_prefix_test_case(prefix_test_case<T> test) {
  auto expr{get_single_expression_program<PrefixExpression>(test.input)};
  if (!expr) {
    return false;
  }
  if (expr->oper != test.oper) {
    std::cout << "Wrong operator. want: " << type_to_string(test.oper)
              << " got: " << type_to_string(expr->oper) << std::endl;
    return false;
  }
  if (!test_literal_expr<C>(std::move(expr->right), test.value)) {
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

template <class C, typename T>
bool test_single_infix_expression(ExprSubtype<InfixExpression> expr, T left,
                                  token_types::TokenVariant oper, T right) {

  if (expr->oper != oper) {
    std::cout << "Wrong operator. want: " << type_to_string(oper)
              << " got: " << type_to_string(expr->oper) << std::endl;
    return false;
  }

  if (!test_literal_expr<C>(std::move(expr->left), left)) {
    return false;
  }
  if (!test_literal_expr<C>(std::move(expr->right), right)) {
    return false;
  }

  return true;
}

template <class C, typename T>
bool do_infix_test_case(infix_test_case<T> test) {
  auto expr{get_single_expression_program<InfixExpression>(test.input)};
  if (!expr) {
    return false;
  }
  return test_single_infix_expression<C, T>(std::move(expr), test.left,
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
      if (!do_infix_test_case<BooleanLiteral>(*bool_test)) {
        ret_val = false;
      }
    } else {
      if (!do_infix_test_case<IntegerLiteral>(*int_test)) {
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
          "a + add(b * c) + d",
          "((a + add((b * c))) + d)",
      },
      test_case{
          "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
          "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
      },
      test_case{
          "add(a + b + c * d / f + g)",
          "add((((a + b) + ((c * d) / f)) + g))",
      },
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

template <typename Expr>
ExprSubtype<Expr>
test_single_block_statement(std::unique_ptr<BlockStatement> block) {
  size_t length{block->statements.size()};
  if (length != 1) {
    std::cout << "Incorrect number of statements. got " << length << " want "
              << 1 << std::endl;
    return nullptr;
  }
  return get_single_expression_statement<Expr>(std::move(block->statements[0]));
}

ExprSubtype<IfExpression> test_single_if_expression(std::string input) {
  auto expr{get_single_expression_program<IfExpression>(input)};
  if (!expr) {
    return nullptr;
  }
  auto result{true};
  auto infix_expr{
      assert_expr_type<InfixExpression>(std::move(expr->condition), result)};
  if (!result) {
    return nullptr;
  }
  result = test_single_infix_expression<Identifier>(std::move(infix_expr), "x",
                                                    token_types::LT{}, "y");
  if (!result) {
    return nullptr;
  }
  auto consequence_expr{
      test_single_block_statement<Identifier>(std::move(expr->consequence))};
  if (!consequence_expr) {
    return nullptr;
  }
  if (!assert_value(consequence_expr, "x")) {
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
  auto alternative_expr{
      test_single_block_statement<Identifier>(std::move(expr->alternative))};
  if (!alternative_expr) {
    return false;
  }
  if (!assert_value(alternative_expr, "y")) {
    return false;
  }

  return true;
}

bool test_function_literal_parsing() {
  auto expr{
      get_single_expression_program<FunctionLiteral>("fn(x, y) { x + y; }")};
  if (!expr) {
    return false;
  }
  size_t params{expr->params.size()};
  if (params != 2) {
    std::cout << "Incorrect number of parameters. got " << params << " want "
              << 2;
    std::cout << expr->to_string() << std::endl;
    return false;
  }
  if (!assert_stack_value(expr->params[0], "x")) {
    return false;
  }
  if (!assert_stack_value(expr->params[1], "y")) {
    return false;
  }

  auto body_expr{
      test_single_block_statement<InfixExpression>(std::move(expr->body))};
  if (!body_expr) {
    return false;
  }

  return test_single_infix_expression<Identifier>(std::move(body_expr), "x",
                                                  token_types::Plus{}, "y");
}

bool test_function_parameter_parsing() {
  struct param_test {
    std::string input;
    std::vector<std::string> expected;
  };
  auto tests{std::vector{
      param_test{"fn(){}", std::vector<std::string>{}},
      param_test{"fn(x){}", std::vector<std::string>{"x"}},
      param_test{"fn(x,y,z){}", std::vector<std::string>{"x", "y", "z"}},
  }};
  for (size_t i = 0; i < tests.size(); i++) {
    param_test test{tests[i]};
    auto expr{get_single_expression_program<FunctionLiteral>(test.input)};
    if (!expr) {
      return false;
    }
    size_t params{expr->params.size()};
    if (params != test.expected.size()) {
      std::cout << "Incorrect number of parameters. got " << params << " want "
                << test.expected.size();
      std::cout << expr->to_string() << std::endl;
      return false;
    }
    for (size_t j = 0; j < expr->params.size(); j++) {

      if (!assert_stack_value(expr->params[j], test.expected[j])) {
        return false;
      }
    }
  }
  return true;
}

bool test_call_expression_parsing() {
  auto expr{
      get_single_expression_program<CallExpression>("add(1, 2 * 3, 4 + 5)")};
  if (!expr) {
    return false;
  }
  size_t params{expr->arguments.size()};
  if (params != 3) {
    std::cout << "Incorrect number of arguments. got " << params << " want "
              << 3;
    std::cout << expr->to_string() << std::endl;
    return false;
  }

  auto result{true};
  auto arg0{
      assert_expr_type<IntegerLiteral>(std::move(expr->arguments[0]), result)};
  if (!result) {
    return false;
  }
  if (!assert_value(arg0, 1)) {
    return false;
  };
  auto arg1{
      assert_expr_type<InfixExpression>(std::move(expr->arguments[1]), result)};
  if (!result) {
    return false;
  }
  if (!test_single_infix_expression<IntegerLiteral>(
          std::move(arg1), 2, token_types::Asterisk{}, 3)) {
    return false;
  };
  auto arg2{
      assert_expr_type<InfixExpression>(std::move(expr->arguments[2]), result)};
  if (!result) {
    return false;
  }
  if (!test_single_infix_expression<IntegerLiteral>(std::move(arg2), 4,
                                                    token_types::Plus{}, 5)) {
    return false;
  };

  return true;
}

bool test_call_parameter_parsing() {
  struct param_test {
    std::string input;
    std::string ident;
    std::vector<std::string> expected;
  };
  auto tests{std::vector{
      param_test{"add();", "add", std::vector<std::string>{}},
      param_test{"add(1)", "add", std::vector<std::string>{"1"}},
      param_test{"mult(1, 2 * 3,  4 + 5)", "mult",
                 std::vector<std::string>{"1", "(2 * 3)", "(4 + 5)"}},
  }};
  for (size_t i = 0; i < tests.size(); i++) {
    param_test test{tests[i]};
    auto expr{get_single_expression_program<CallExpression>(test.input)};
    if (!expr) {
      return false;
    }
    auto function{std::move(expr->function)};
    if (!test_literal_expr<Identifier>(std::move(function), test.ident)) {
      return false;
    }
    auto args{std::move(expr->arguments)};
    size_t params{args.size()};
    if (params != test.expected.size()) {
      std::cout << "Incorrect number of arguments. got " << params << " want "
                << test.expected.size();
      return false;
    }
    for (size_t j = 0; j < args.size(); j++) {
      if (args[j]->to_string() != test.expected[j]) {
        return false;
      }
    }
  }
  return true;
}
