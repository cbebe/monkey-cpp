#include "test.hpp"

#include "ast/ast.hpp"
#include "evaluator/evaluator.hpp"
#include "object/object.hpp"
#include "parser/parser.hpp"

bool test_eval_integer_expression();
bool test_eval_boolean_expression();
bool test_eval_string_expression();
bool test_bang_operator();
bool test_if_else_expressions();
bool test_return_statements();
bool test_error_handling();
bool test_let_statements();
bool test_function_object();
bool test_function_application();
bool test_string_concatenation();
bool test_builtin_functions();
bool test_array_literals();
bool test_array_index_expression();
bool test_hash_literals();

int main() {
  bool pass{true};
  TEST(test_eval_integer_expression, pass);
  TEST(test_eval_boolean_expression, pass);
  TEST(test_eval_string_expression, pass);
  TEST(test_bang_operator, pass);
  TEST(test_if_else_expressions, pass);
  TEST(test_return_statements, pass);
  TEST(test_error_handling, pass);
  TEST(test_let_statements, pass);
  TEST(test_function_object, pass);
  TEST(test_function_application, pass); // leaks!!
  TEST(test_string_concatenation, pass);
  TEST(test_builtin_functions, pass); // leaks!!
  TEST(test_array_literals, pass);
  TEST(test_array_index_expression, pass);
  TEST(test_hash_literals, pass);
  return pass ? 0 : 1;
}

std::shared_ptr<Object> h_test_eval(std::string input) {
  auto env{std::make_shared<Environment>()};
  Parser p{Lexer{input}};
  auto program{p.parse_program()};
  return eval(std::move(program), std::move(env));
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
  auto literal{h_assert_obj_type<C>(obj, result)};
  if (!result) {
    return false;
  }
  if (!h_assert_value(literal->value, expected)) {
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
      test<IntType>{"5", 5},
      test<IntType>{"10", 10},
      test<IntType>{"-5", -5},
      test<IntType>{"-10", -10},
      test<IntType>{"5 + 5 + 5 + 5 - 10", 10},
      test<IntType>{"2 * 2 * 2 * 2 * 2", 32},
      test<IntType>{"-50 + 100 + -50", 0},
      test<IntType>{"5 * 2 + 10", 20},
      test<IntType>{"5 + 2 * 10", 25},
      test<IntType>{"20 + 2 * -10", 0},
      test<IntType>{"50 / 2 * 2 + 10", 60},
      test<IntType>{"2 * (5 + 10)", 30},
      test<IntType>{"3 * 3 * 3 + 10", 37},
      test<IntType>{"3 * (3 * 3) + 10", 37},
      test<IntType>{"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
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

bool test_eval_string_expression() {
  auto tests{std::vector{
      test<std::string>{R"("Hello World!")", "Hello World!"},
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<String>(evaluated.get(), test.expected)) {
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
  auto ten = std::make_unique<IntType>(10);
  auto twenty = std::make_unique<IntType>(20);
  auto p_10 = ten.get();
  auto p_20 = twenty.get();
  auto tests{std::vector{
      test<IntType *>{"if (true) { 10 }", p_10},
      test<IntType *>{"if (false) { 10 }", nullptr},
      test<IntType *>{"if (1) { 10 }", p_10},
      test<IntType *>{"if (1 < 2) { 10 }", p_10},
      test<IntType *>{"if (1 > 2) { 10 }", nullptr},
      test<IntType *>{"if (1 > 2) { 10 } else { 20 }", p_20},
      test<IntType *>{"if (1 < 2) { 10 } else { 20 }", p_10},
  }};

  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    auto val{evaluated.get()};
    if (test.expected) {
      if (!h_test_literal<Integer>(val, *test.expected)) {
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

bool test_return_statements() {
  auto tests{std::vector{
      // clang-format off
      test<IntType>{"return 10;", 10},
      test<IntType>{"return 10; 9;", 10},
      test<IntType>{"return 2 * 5; 9;", 10},
      test<IntType>{"9; return 2 * 5; 9;", 10},
      test<IntType>{"if (10 > 1) { if (10 > 1) { return 10; } return 1; }", 10},
      // clang-format on
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_error_handling() {
  auto tests{std::vector{
      // clang-format off
      test<std::string>{ "5 + true;", "type mismatch: INTEGER + BOOLEAN" },
      test<std::string>{ "5 + true; 5;", "type mismatch: INTEGER + BOOLEAN" },
      test<std::string>{ "-true", "unknown operator: -BOOLEAN" },
      test<std::string>{ "true + false;", "unknown operator: BOOLEAN + BOOLEAN" },
      test<std::string>{ "5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN" },
      test<std::string>{ "if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN" },
      test<std::string>{ R"("Hello" - "World")", "unknown operator: STRING - STRING" },
      // clang-format on
      test<std::string>{
          R"(if (10 > 1) {
  if (10 > 1) {
    return true + false;
  }

  return 1;
})",
          "unknown operator: BOOLEAN + BOOLEAN",
      },
      test<std::string>{"foobar", "identifier not found: foobar"},

      // len(String) function
      // clang-format off
      test<std::string>{R"(len(1))", "argument to `len` not supported, got INTEGER"},
      test<std::string>{R"(len("one", "two"))", "wrong number of arguments. got=2, want=1"},
      // clang-format on
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Error>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_let_statements() {
  auto tests{std::vector{
      test<IntType>{"let a = 5; a;", 5},
      test<IntType>{"let a = 5 * 5; a;", 25},
      test<IntType>{"let a = 5; let b = a; b;", 5},
      test<IntType>{"let a = 5; let b = a; let c = a + b + 5; c;", 15},
  }};
  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_function_object() {
  auto evaluated{h_test_eval("fn(x) { x + 2; };")};
  auto result{true};
  auto literal{h_assert_obj_type<Function>(evaluated.get(), result)};
  if (!result) {
    return false;
  }
  auto param_len{literal->params.size()};
  if (param_len != 1) {
    std::cout << "function has wrong parameters. got: " << param_len
              << std::endl;
    return false;
  }
  if (!h_assert_value<std::string>(literal->params[0].value, "x")) {
    return false;
  }
  if (!h_assert_value<std::string>(literal->body->to_string(), "(x + 2)\n")) {
    return false;
  }

  return true;
}

bool test_function_application() {
  auto tests{std::vector{
      // clang-format off
      test<IntType>{"let identity = fn(x) { x; }; identity(5);", 5}, // leaks!!
      test<IntType>{"let identity = fn(x) { return x; }; identity(5);", 5}, // leaks!!
      test<IntType>{"let double = fn(x) { x * 2; }; double(5);", 10}, // leaks!!
      test<IntType>{"let add = fn(x, y) { x + y; }; add(5, 5);", 10}, // leaks!!
      test<IntType>{"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20}, // leaks!!
      test<IntType>{"fn(x) { x; }(5)", 5}, // this somehow doesn't leak so it's about the function being bound to a variable?
      // clang-format on
  }};

  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_string_concatenation() {
  auto tests{std::vector{
      test<std::string>{R"("Hello" + " " + "World!")", "Hello World!"},
  }};

  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<String>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_builtin_functions() {
  auto tests{std::vector{
      // clang-format off
      test<IntType>{R"(len(""))", 0},
      test<IntType>{R"(len("four"))", 4},
      test<IntType>{R"(len("hello world"))", 11},
      // clang-format on
      test<IntType>{R"(
        let map = fn(arr, f) {
          let iter = fn(arr, accumulated) {
            if (len(arr) == 0) {
              accumulated
            } else {
              iter(rest(arr), push(accumulated, f(first(arr))));
            }
          };

          iter(arr, []);
        };
        let reduce = fn(arr, initial, f) {
          let iter = fn(arr, result) {
            if (len(arr) == 0) {
              result
            } else {
              iter(rest(arr), f(result, first(arr)));
            }
          };

          iter(arr, initial);
        };
        let double = fn(x) { x * 2 };
        let sum = fn(arr) {
          reduce(arr, 0, fn(initial, el) { initial + el });
        };

        let a = [1, 2, 3, 4];
        sum(map(a, double));
        )",
                    20}, // yep, this is the only one that leaks so it's about
                         // binding the function to identifiers...
  }};

  auto pass{true};
  for (auto test : tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  return pass;
}

bool test_array_literals() {
  auto evaluated{h_test_eval("[1, 2 * 2, 3 + 3]")};
  auto result{true};
  auto array{h_assert_obj_type<Array>(evaluated.get(), result)};
  if (!result) {
    return false;
  }
  if (!h_assert_value<size_t>(array->elements.size(), 3)) {
    return false;
  }
  if (!h_test_literal<Integer, IntType>(array->elements[0].get(), 1)) {
    return false;
  }
  if (!h_test_literal<Integer, IntType>(array->elements[1].get(), 4)) {
    return false;
  }
  if (!h_test_literal<Integer, IntType>(array->elements[2].get(), 6)) {
    return false;
  }
  return true;
}

bool test_array_index_expression() {
  auto index_tests{std::vector{
      // clang-format off
      test<IntType>{ "[1, 2, 3][0]", 1 },
      test<IntType>{ "[1, 2, 3][1]", 2 },
      test<IntType>{ "[1, 2, 3][2]", 3 },
      test<IntType>{ "let i = 0; [1][i];", 1 },
      test<IntType>{ "[1, 2, 3][1 + 1];", 3 },
      test<IntType>{ "let myArray = [1, 2, 3]; myArray[2];", 3 },
      test<IntType>{ "let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", 6 },
      test<IntType>{ "let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", 2 },
      // clang-format on
  }};
  auto pass{true};
  for (auto test : index_tests) {
    auto evaluated{h_test_eval(test.input)};
    if (!h_test_literal<Integer>(evaluated.get(), test.expected)) {
      std::cout << test.input << std::endl;
      pass &= false;
    }
  }
  auto nil_tests{std::vector{"[1, 2, 3][3]", "[1, 2, 3][-1]"}};
  for (auto test : nil_tests) {
    auto evaluated{h_test_eval(test)};
    auto result{true};
    h_assert_obj_type<Null>(evaluated.get(), result);
    if (!result) {
      pass &= false;
    }
  }
  return pass;
}

bool test_hash_literals() {
  auto evaluated{h_test_eval(R"(
    let two = "two";
    {
          "one": 10 - 9,
          two: 1 + 1,
          "thr" + "ee": 6 / 2,
          4: 4,
          true: 5,
          false: 6
    })")};
  auto result{true};
  auto hash{h_assert_obj_type<Hash>(evaluated.get(), result)};
  if (!result) {
    return false;
  }
  auto expected{std::unordered_map<HashKey, long>{
      {string("one")->hash_key(), 1},
      {string("two")->hash_key(), 2},
      {string("three")->hash_key(), 3},
      {integer(4)->hash_key(), 4},
      {_TRUE.hash_key(), 5},
      {_FALSE.hash_key(), 6},
  }};
  if (!h_assert_value<size_t>(hash->pairs.size(), expected.size())) {
    return false;
  }
  for (const auto &k : expected) {
    if (hash->pairs.contains(k.first)) {
      if (!h_test_literal<Integer, IntType>(hash->pairs[k.first].value.get(),
                                            k.second)) {
        return false;
      }
    } else {
      std::cout << "no pair for given key in pairs" << std::endl;
      return false;
    }
  }
  return true;
}
