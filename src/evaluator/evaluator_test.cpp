#include "../ast/ast.hpp"
#include "../object/object.hpp"
#include "../parser/parser.hpp"
#include "evaluator.hpp"
#include <iostream>
#include <memory>
#include <vector>

std::unique_ptr<Object> h_test_eval(std::string input) {
  Parser p{Lexer{input}};
  auto program{p.parse_program()};
  return eval(std::move(program));
}

template <typename T> T *h_assert_obj_type(Object *obj, bool &result) {
  auto res{dynamic_cast<T *>(obj)};
  if (!res) {
    std::cout << "Failed test. not " << typeid(T).name() << std::endl;
    result = false;
  }
  return res;
}

template <typename T> bool h_assert_value(T got, T want) {
  if (got != want) {
    std::cout << "Failed test. got " << got << ", want " << want << std::endl;
    return false;
  }
  return true;
}

template <class C, typename T> bool h_test_literal(Object *obj, T expected) {
  auto result{true};
  auto integer{h_assert_obj_type<C>(obj, result)};
  if (!result) {
    return false;
  }
  if (!h_assert_value(integer->value, expected)) {
    return false;
  }
  return true;
}

template <typename T> struct test {
  std::string input;
  T expected;
};

bool test_eval_integer_expression() {
  auto tests{std::vector{
      test<long>{"5", 5},
      test<long>{"10", 10},
      test<long>{"-5", -5},
      test<long>{"-10", -10},
      test<long>{"5 + 5 + 5 + 5 - 10", 10},
      test<long>{"2 * 2 * 2 * 2 * 2", 32},
      test<long>{"-50 + 100 + -50", 0},
      test<long>{"5 * 2 + 10", 20},
      test<long>{"5 + 2 * 10", 25},
      test<long>{"20 + 2 * -10", 0},
      test<long>{"50 / 2 * 2 + 10", 60},
      test<long>{"2 * (5 + 10)", 30},
      test<long>{"3 * 3 * 3 + 10", 37},
      test<long>{"3 * (3 * 3) + 10", 37},
      test<long>{"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
  }};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      return false;
    }
  }
  return true;
}

bool test_eval_boolean_expression() {
  auto tests{std::vector{
      test<bool>{"true", true},
      test<bool>{"false", false},

      test<bool>{"1 < 2", true},
      test<bool>{"1 > 2", false},
      test<bool>{"1 < 1", false},
      test<bool>{"1 > 1", false},
      test<bool>{"1 == 1", true},
      test<bool>{"1 != 1", false},
      test<bool>{"1 == 2", false},
      test<bool>{"1 != 2", true},

      test<bool>{"true == true", true},
      test<bool>{"false == false", true},
      test<bool>{"true == false", false},
      test<bool>{"true != false", true},
      test<bool>{"false != true", true},
      test<bool>{"(1 < 2) == true", true},
      test<bool>{"(1 < 2) == false", false},
      test<bool>{"(1 > 2) == true", false},
      test<bool>{"(1 > 2) == false", true},
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Boolean>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_bang_operator() {
  auto tests{std::vector{
      test<bool>{"!true", false},
      test<bool>{"!false", true},
      test<bool>{"!5", false},
      test<bool>{"!!true", true},
      test<bool>{"!!false", false},
      test<bool>{"!!5", true},
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Boolean>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_if_else_expressions() {
  auto ten = std::make_unique<long>(10);
  auto twenty = std::make_unique<long>(20);
  auto p_10 = ten.get();
  auto p_20 = twenty.get();
  auto tests{std::vector{
      test<long *>{"if (true) { 10 }", p_10},
      test<long *>{"if (false) { 10 }", nullptr},
      test<long *>{"if (1) { 10 }", p_10},
      test<long *>{"if (1 < 2) { 10 }", p_10},
      test<long *>{"if (1 > 2) { 10 }", nullptr},
      test<long *>{"if (1 > 2) { 10 } else { 20 }", p_20},
      test<long *>{"if (1 < 2) { 10 } else { 20 }", p_10},
  }};

  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    auto val{evaluated.get()};
    if (test.expected) {
      if (!h_test_literal<Integer>(val, *test.expected)) {
        std::cout << test.input << std::endl;
        std::cout << val->inspect() << std::endl;
        pass &= false;
      }
    } else {
      auto result{true};
      h_assert_obj_type<Null>(val, result);
      if (!result) {
        pass &= false;
      }
    }
  }
  return pass;
}
