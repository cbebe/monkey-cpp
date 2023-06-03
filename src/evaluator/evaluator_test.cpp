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

  // the down-cast is real
  auto node{std::unique_ptr<Node>{std::move(program)}};
  return std::unique_ptr<Object>(eval(std::move(node)));
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

bool h_test_integer_object(Object *obj, long expected) {
  auto result{true};
  auto integer{h_assert_obj_type<Integer>(obj, result)};
  if (!result) {
    return false;
  }
  return h_assert_value(integer->value, expected);
}

bool test_eval_integer_expression() {
  struct test {
    std::string input;
    long expected;
  };
  auto tests{std::vector{
      test{"5", 5},
      test{"10", 10},
  }};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_integer_object(evaluated.get(), test.expected)) {
      return false;
    }
  }
  return true;
}
