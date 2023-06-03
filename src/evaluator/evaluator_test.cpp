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
  }};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Boolean>(evaluated.get(), test.expected)) {
      return false;
    }
  }
  return true;
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
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Boolean>(evaluated.get(), test.expected)) {
      return false;
    }
  }
  return true;
}
