#pragma once
#include "../src/parser.hpp"
#include <iostream>

bool first_next_token_test();
bool second_next_token_test();
bool third_next_token_test();

std::unique_ptr<Program>
check_errors(Parser &p, std::unique_ptr<Program> program, bool &pass);
std::unique_ptr<Program> parser_pre_checks(Parser &p,
                                           std::unique_ptr<Program> program,
                                           size_t num_statements);
Parser parse_input(std::string input);

template <class StatementType>
bool assert_type(std::unique_ptr<Statement> to_convert,
                 std::unique_ptr<Statement> &var) {
  var = std::unique_ptr<StatementType>(to_convert);
  if (!var) {
    std::cout << "Failed test: not a " << typeid(StatementType).name()
              << std::endl;
    return false;
  }
  return true;
}

bool test_let_statements();
bool test_return_statements();

template <typename To, typename From, typename Deleter>
std::unique_ptr<To, Deleter>
dynamic_unique_cast(std::unique_ptr<From, Deleter> &&p) {
  if (To *cast = dynamic_cast<To *>(p.get())) {
    std::unique_ptr<To, Deleter> result(cast, std::move(p.get_deleter()));
    (void)p.release();
    return result;
  }
  return std::unique_ptr<To, Deleter>(nullptr);
}

template <typename To>
using ExprSubtype = std::unique_ptr<To, std::default_delete<Expression>>;

template <typename To>
using StatementSubtype = std::unique_ptr<To, std::default_delete<Statement>>;

template <typename To>
StatementSubtype<To>
assert_statement_type(std::unique_ptr<Statement> test_statement, bool &result) {
  auto statement{dynamic_unique_cast<To>(std::move(test_statement))};
  if (!statement) {
    std::cout << "Failed test: not " << typeid(To).name() << std::endl;
    result = false;
  }
  result = true;
  return statement;
}

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

template <class Expr, typename T> bool assert_value(Expr &expr, T test_value) {
  if (expr->value != test_value) {
    std::cout << "Failed test - want: " << test_value
              << ". got: " << expr->value << std::endl;
    return false;
  }
  return true;
}

template <class Expr, typename T>
bool test_literal_expr(std::unique_ptr<Expression> test_expr, T test_value) {
  auto result{true};
  ExprSubtype<Expr> expr{assert_expr_type<Expr>(std::move(test_expr), result)};
  if (!result) {
    return false;
  }
  return assert_value(expr, test_value);
}

template <class Expr, typename T>
bool assert_stack_value(Expr expr, T test_value) {
  if (expr.value != test_value) {
    std::cout << "Failed test - want: " << test_value << ". got: " << expr.value
              << std::endl;
    return false;
  }
  return true;
}

template <typename To>
ExprSubtype<To>
assert_expr_type_statement(StatementSubtype<ExpressionStatement> statement,
                           bool &result) {
  return assert_expr_type<To>(std::move(statement->value), result);
}

template <typename To>
ExprSubtype<To>
get_single_expression_statement(std::unique_ptr<Statement> gen) {
  bool result{true};
  auto statement{
      assert_statement_type<ExpressionStatement>(std::move(gen), result)};
  auto expr{assert_expr_type_statement<To>(std::move(statement), result)};
  if (!result) {
    return nullptr;
  }

  return expr;
}

template <typename To, typename Value>
ExprSubtype<To> get_single_expression_statement(std::unique_ptr<Statement> gen,
                                                Value value) {
  auto expr{get_single_expression_statement<To>(gen)};
  if (!assert_value(expr, value)) {
    return nullptr;
  }

  return expr;
}

template <typename To, typename Value>
bool test_single_expression_statement(std::unique_ptr<Statement> gen,
                                      Value value) {
  return !!get_single_expression_statement<To>(gen, value);
}

template <typename To>
ExprSubtype<To> get_single_expression_program(std::string input) {
  Parser p{parse_input(input)};
  auto program{p.parse_program()};
  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return nullptr;
  }
  return get_single_expression_statement<To>(std::move(program->statements[0]));
}

template <typename To, typename Value>
bool test_single_expression_program(std::string input, Value value) {
  auto expr{get_single_expression_program<To>(input)};
  return assert_value(expr, value);
}

bool test_identifier_expression();
bool test_integer_literal_expression();
bool test_boolean_literal_expression();
bool test_prefix_expression();
bool test_infix_expression();
bool test_operator_precedence();

bool test_if_expression();
bool test_if_else_expression();
bool test_function_literal_parsing();
bool test_function_parameter_parsing();

bool test_call_expression_parsing();
bool test_call_parameter_parsing();
