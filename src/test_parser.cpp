#include "test.hpp"

#include "parser/parser.hpp"
#include <any>
#include <iostream>

bool test_let_statements();
bool test_return_statements();

bool test_identifier_expression();
bool test_integer_literal_expression();
bool test_boolean_literal_expression();
bool test_string_literal_expression();
bool test_prefix_expression();
bool test_index_expression();
bool test_infix_expression();
bool test_operator_precedence();

bool test_if_expression();
bool test_if_else_expression();
bool test_function_literal_parsing();
bool test_function_parameter_parsing();

bool test_call_expression_parsing();
bool test_call_parameter_parsing();

bool test_array_literal_parsing();
bool test_empty_array_literal_parsing();

int main() {
  bool pass{true};
  TEST(test_let_statements, pass);
  TEST(test_return_statements, pass);
  TEST(test_identifier_expression, pass);
  TEST(test_integer_literal_expression, pass);
  TEST(test_boolean_literal_expression, pass);
  TEST(test_string_literal_expression, pass);
  TEST(test_prefix_expression, pass);
  TEST(test_index_expression, pass);
  TEST(test_infix_expression, pass);
  TEST(test_operator_precedence, pass);
  TEST(test_if_expression, pass);
  TEST(test_if_else_expression, pass);
  TEST(test_function_literal_parsing, pass);
  TEST(test_function_parameter_parsing, pass);
  TEST(test_call_expression_parsing, pass);
  TEST(test_call_parameter_parsing, pass);
  TEST(test_array_literal_parsing, pass);
  TEST(test_empty_array_literal_parsing, pass);
  return pass ? 0 : 1;
}

// TODO: This is a mess, use exceptions or something

// {{{ "Helpers"
std::shared_ptr<Program>
h_check_errors(Parser &p, std::shared_ptr<Program> program, bool &pass);
std::shared_ptr<Program> h_parser_pre_checks(Parser &p,
                                             std::shared_ptr<Program> program,
                                             size_t num_statements);
Parser h_parse_input(std::string input);

template <class StatementType>
bool h_assert_type(std::shared_ptr<Statement> to_convert,
                   std::shared_ptr<Statement> &var) {
  var = std::shared_ptr<StatementType>(to_convert);
  if (!var) {
    std::cout << "Failed test: not a " << typeid(StatementType).name()
              << std::endl;
    return false;
  }
  return true;
}

template <typename To> using ExprSubtype = std::shared_ptr<To>;

template <typename To> using StatementSubtype = std::shared_ptr<To>;

template <typename To>
StatementSubtype<To>
h_assert_statement_type(std::shared_ptr<Statement> test_statement,
                        bool &result) {
  auto statement{std::dynamic_pointer_cast<To>(std::move(test_statement))};
  if (!statement) {
    std::cout << "Failed test: not " << typeid(To).name() << std::endl;
    result = false;
  }
  result = true;
  return statement;
}

template <typename To>
ExprSubtype<To> h_assert_expr_type(std::shared_ptr<Expression> test_expr,
                                   bool &result) {
  auto expr{std::dynamic_pointer_cast<To>(std::move(test_expr))};
  if (!expr) {
    std::cout << "Failed test: not " << typeid(To).name() << std::endl;
    result = false;
  }
  result = true;
  return expr;
}

template <class Expr, typename T>
bool h_assert_value(Expr &expr, T test_value) {
  if (expr->value != test_value) {
    std::cout << "Failed test - want: " << test_value
              << ". got: " << expr->value << std::endl;
    return false;
  }
  return true;
}

template <class Expr, typename T>
bool h_test_literal_expr(std::shared_ptr<Expression> test_expr, T test_value) {
  auto result{true};
  ExprSubtype<Expr> expr{
      h_assert_expr_type<Expr>(std::move(test_expr), result)};
  if (!result) {
    return false;
  }
  return h_assert_value(expr, test_value);
}

template <class Expr, typename T>
bool h_assert_stack_value(Expr expr, T test_value) {
  if (expr.value != test_value) {
    std::cout << "Failed test - want: " << test_value << ". got: " << expr.value
              << std::endl;
    return false;
  }
  return true;
}

template <typename To>
ExprSubtype<To>
h_assert_expr_type_statement(StatementSubtype<ExpressionStatement> statement,
                             bool &result) {
  return h_assert_expr_type<To>(std::move(statement->value), result);
}

template <typename To>
ExprSubtype<To>
h_get_single_expression_statement(std::shared_ptr<Statement> gen) {
  bool result{true};
  auto statement{
      h_assert_statement_type<ExpressionStatement>(std::move(gen), result)};
  auto expr{h_assert_expr_type_statement<To>(std::move(statement), result)};
  if (!result) {
    return nullptr;
  }

  return expr;
}

template <typename To, typename Value>
ExprSubtype<To>
h_get_single_expression_statement(std::shared_ptr<Statement> gen, Value value) {
  auto expr{h_get_single_expression_statement<To>(gen)};
  if (!h_assert_value(expr, value)) {
    return nullptr;
  }

  return expr;
}

template <typename To, typename Value>
bool test_single_expression_statement(std::shared_ptr<Statement> gen,
                                      Value value) {
  return !!h_get_single_expression_statement<To>(gen, value);
}

template <typename To>
ExprSubtype<To> get_single_expression_program(std::string input) {
  Parser p{h_parse_input(input)};
  auto program{p.parse_program()};
  program = h_parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return nullptr;
  }
  return h_get_single_expression_statement<To>(
      std::move(program->statements[0]));
}

template <typename To, typename Value>
bool h_test_single_expression_program(std::string input, Value value) {
  auto expr{get_single_expression_program<To>(input)};
  return h_assert_value(expr, value);
}

std::shared_ptr<Program>
h_check_errors(Parser &p, std::shared_ptr<Program> program, bool &pass) {
  if (p.errors.size() > 0) {
    std::cout << "parser error:" << std::endl;
    for (auto &e : p.errors) {
      std::cout << e << std::endl;
    }
    pass = false;
  };
  return program;
}

std::shared_ptr<Program> h_parser_pre_checks(Parser &p,
                                             std::shared_ptr<Program> program,
                                             size_t num_statements) {
  bool pass{true};
  program = h_check_errors(p, std::move(program), pass);
  if (!pass) {
    return nullptr;
  }
  if (!program) {
    std::cout << "Failed test: program is null" << std::endl;
    return nullptr;
  }
  auto length{program->statements.size()};
  if (length != num_statements) {
    std::cout << "Failed test: program does not contain " << num_statements
              << " statements. Got: " << length << std::endl;
    return nullptr;
  }

  return program;
}

Parser h_parse_input(std::string input) { return Parser{Lexer{input}}; }

template <typename Expr, typename T>
bool h_test_let(std::string input, std::string name, T value) {
  Parser p{h_parse_input(input)};
  auto program{p.parse_program()};

  program = h_parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }
  auto result{true};
  auto statement{h_assert_statement_type<LetStatement>(
      std::move(program->statements[0]), result)};
  if (!result) {
    return false;
  }
  auto ident{statement->identifier.value};
  if (ident != name) {
    std::cout << "Failed test: got identifier: " << ident << ". want: " << name
              << std::endl;
    return false;
  }
  return h_test_literal_expr<Expr, T>(std::move(statement->value), value);
}

template <typename Expr, typename T>
bool h_test_return(std::string input, T value) {
  Parser p{h_parse_input(input)};
  auto program{p.parse_program()};

  program = h_parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }
  auto result{true};
  auto statement{h_assert_statement_type<ReturnStatement>(
      std::move(program->statements[0]), result)};
  if (!result) {
    return false;
  }
  return h_test_literal_expr<Expr, T>(std::move(statement->value), value);
}

template <typename T> struct prefix_test_case {
  std::string input;
  token_types::TokenVariant oper;
  T value;
};

template <typename T, class C>
bool h_do_prefix_test_case(prefix_test_case<T> test) {
  auto expr{get_single_expression_program<PrefixExpression>(test.input)};
  if (!expr) {
    return false;
  }
  if (expr->oper != test.oper) {
    std::cout << "Wrong operator. want: " << type_to_string(test.oper)
              << " got: " << type_to_string(expr->oper) << std::endl;
    return false;
  }
  if (!h_test_literal_expr<C>(std::move(expr->right), test.value)) {
    return false;
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

  if (!h_test_literal_expr<C>(std::move(expr->left), left)) {
    return false;
  }
  if (!h_test_literal_expr<C>(std::move(expr->right), right)) {
    return false;
  }

  return true;
}

template <class C, typename T>
bool h_do_infix_test_case(infix_test_case<T> test) {
  auto expr{get_single_expression_program<InfixExpression>(test.input)};
  if (!expr) {
    return false;
  }
  return test_single_infix_expression<C, T>(std::move(expr), test.left,
                                            test.oper, test.right);
}

template <typename Expr>
ExprSubtype<Expr>
test_single_block_statement(std::shared_ptr<BlockStatement> block) {
  size_t length{block->statements.size()};
  if (length != 1) {
    std::cout << "Incorrect number of statements. got " << length << " want "
              << 1 << std::endl;
    return nullptr;
  }
  return h_get_single_expression_statement<Expr>(
      std::move(block->statements[0]));
}

ExprSubtype<IfExpression> test_single_if_expression(std::string input) {
  auto expr{get_single_expression_program<IfExpression>(input)};
  if (!expr) {
    return nullptr;
  }
  auto result{true};
  auto infix_expr{
      h_assert_expr_type<InfixExpression>(std::move(expr->condition), result)};
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
  if (!h_assert_value(consequence_expr, "x")) {
    return nullptr;
  }

  return expr;
}
// }}}

// {{{ Actual Tests
bool test_let_statements() {
  return h_test_let<IntegerLiteral>("let x = 5;", "x", 5) &&
         h_test_let<BooleanLiteral>("let y = true;", "y", true) &&
         h_test_let<Identifier>("let foobar = y;", "foobar", "y");
}

bool test_return_statements() {
  return h_test_return<IntegerLiteral>("return 5;", 5) &&
         h_test_return<BooleanLiteral>("return true;", true) &&
         h_test_return<Identifier>("return foobar;", "foobar");
}

bool test_identifier_expression() {
  return h_test_single_expression_program<Identifier>("foobar;", "foobar");
}

bool test_integer_literal_expression() {
  return h_test_single_expression_program<IntegerLiteral>("5;", 5);
}

bool test_boolean_literal_expression() {
  return h_test_single_expression_program<BooleanLiteral>("true;", true);
}

bool test_string_literal_expression() {
  return h_test_single_expression_program<StringLiteral>("\"hello world\";",
                                                         "hello world");
}

bool test_prefix_expression() {
  auto tests = std::vector<std::any>{
      prefix_test_case<long>{"!5;", token_types::Bang{}, 5},
      prefix_test_case<long>{"-15;", token_types::Minus{}, 15},
      prefix_test_case<bool>{"!true;", token_types::Bang{}, true},
      prefix_test_case<bool>{"!false;", token_types::Bang{}, false},
  };

  for (size_t i = 0; i < tests.size(); i++) {
    auto test{tests[i]};
    auto int_test{std::any_cast<prefix_test_case<long>>(&test)};
    auto bool_test{std::any_cast<prefix_test_case<bool>>(&test)};
    bool ret_val{true};
    if (int_test) {
      if (!h_do_prefix_test_case<long, IntegerLiteral>(*int_test)) {
        ret_val = false;
      }
    } else if (bool_test) {
      if (!h_do_prefix_test_case<bool, BooleanLiteral>(*bool_test)) {
        ret_val = false;
      }
    } else {
      std::cout << "Not a valid test case " << i << std::endl;
      return false;
    }
    if (!ret_val) {
      std::cout << "Failed test case " << i << std::endl;
      return false;
    }
  }
  return true;
}

bool test_index_expression() {
  auto expr{get_single_expression_program<IndexExpression>("myArray[1 + 1]")};
  if (!expr) {
    return false;
  }

  if (!h_test_literal_expr<Identifier>(std::move(expr->left), "myArray")) {
    return false;
  }

  auto result{true};
  auto index{
      h_assert_expr_type<InfixExpression>(std::move(expr->index), result)};
  if (!result) {
    return false;
  }
  return test_single_infix_expression<IntegerLiteral>(std::move(index), 1,
                                                      token_types::Plus{}, 1);
}

bool test_infix_expression() {
  using namespace token_types;
  auto tests = std::vector<std::any>{
      infix_test_case<long>{"3 + 10;", 3, Plus{}, 10},
      infix_test_case<long>{"5 - 5;", 5, Minus{}, 5},
      infix_test_case<long>{"5 * 5;", 5, Asterisk{}, 5},
      infix_test_case<long>{"5 / 5;", 5, Slash{}, 5},
      infix_test_case<long>{"5 > 5;", 5, GT{}, 5},
      infix_test_case<long>{"5 < 5;", 5, LT{}, 5},
      infix_test_case<long>{"5 == 5;", 5, Eq{}, 5},
      infix_test_case<long>{"5 != 5;", 5, NotEq{}, 5},
      infix_test_case<bool>{"true == true", true, Eq{}, true},
      infix_test_case<bool>{"false == false", false, Eq{}, false},
      infix_test_case<bool>{"true != false", true, NotEq{}, false},
  };
  for (size_t i = 0; i < tests.size(); i++) {
    auto test{tests[i]};
    auto int_test{std::any_cast<infix_test_case<long>>(&test)};
    auto bool_test{std::any_cast<infix_test_case<bool>>(&test)};
    bool ret_val{true};
    if (int_test) {
      if (!h_do_infix_test_case<IntegerLiteral>(*int_test)) {
        ret_val = false;
      }
    } else if (bool_test) {
      if (!h_do_infix_test_case<BooleanLiteral>(*bool_test)) {
        ret_val = false;
      }
    } else {
      std::cout << "Not a valid test case " << i << std::endl;
      return false;
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

  // clang-format off
  auto tests{std::vector{
      test_case{ "a + add(b * c) + d", "((a + add((b * c))) + d)" },
      test_case{ "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))" },
      test_case{ "add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))" },
      test_case{ "true", "true" },
      test_case{ "false", "false" },
      test_case{ "3 > 5 == false", "((3 > 5) == false)" },
      test_case{ "3 < 5 == true", "((3 < 5) == true)" },
      test_case{ "-a * b", "((-a) * b)" },
      test_case{ "!-a", "(!(-a))" },
      test_case{ "a + b + c", "((a + b) + c)" },
      test_case{ "a + b - c", "((a + b) - c)" },
      test_case{ "a * b * c", "((a * b) * c)" },
      test_case{ "a * b / c", "((a * b) / c)" },
      test_case{ "a + b / c", "(a + (b / c))" },
      test_case{ "a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)" },
      test_case{ "3 + 4; -5 * 5", "(3 + 4)((-5) * 5)" },
      test_case{ "5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))" },
      test_case{ "5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))" },
      test_case{ "3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))" },
      test_case{ "1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)" },
      test_case{ "(5 + 5) * 2", "((5 + 5) * 2)" },
      test_case{ "2 / (5 + 5)", "(2 / (5 + 5))" },
      test_case{ "-(5 + 5)", "(-(5 + 5))" },
      test_case{ "!(true == true)", "(!(true == true))" },
      test_case{ "a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)" },
      test_case{ "add(a * b[2], b[1], 2 * [1, 2][1])", "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))" },
  }};
  // clang-format on

  for (size_t i = 0; i < tests.size(); i++) {
    auto t{tests[i]};
    Parser p{h_parse_input(t.input)};
    auto program{p.parse_program()};
    bool pass{true};
    program = h_check_errors(p, std::move(program), pass);
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
  if (!h_assert_value(alternative_expr, "y")) {
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
  if (!h_assert_stack_value(expr->params[0], "x")) {
    return false;
  }
  if (!h_assert_stack_value(expr->params[1], "y")) {
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

      if (!h_assert_stack_value(expr->params[j], test.expected[j])) {
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
  auto arg0{h_assert_expr_type<IntegerLiteral>(std::move(expr->arguments[0]),
                                               result)};
  if (!result) {
    return false;
  }
  if (!h_assert_value(arg0, 1)) {
    return false;
  };
  auto arg1{h_assert_expr_type<InfixExpression>(std::move(expr->arguments[1]),
                                                result)};
  if (!result) {
    return false;
  }
  if (!test_single_infix_expression<IntegerLiteral>(
          std::move(arg1), 2, token_types::Asterisk{}, 3)) {
    return false;
  };
  auto arg2{h_assert_expr_type<InfixExpression>(std::move(expr->arguments[2]),
                                                result)};
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
    if (!h_test_literal_expr<Identifier>(std::move(function), test.ident)) {
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

bool test_empty_array_literal_parsing() {
  auto input{"[]"};
  auto expr{get_single_expression_program<ArrayLiteral>(input)};
  if (!expr) {
    return false;
  }
  auto elems{expr->elements.size()};
  if (elems != 0) {
    std::cout << "Incorrect number of elements. got " << elems << " want " << 0;
    return false;
  }
  return true;
}

bool test_array_literal_parsing() {
  auto input{"[1, 2 * 2, 3 + 3]"};
  auto expr{get_single_expression_program<ArrayLiteral>(input)};
  if (!expr) {
    return false;
  }
  auto elems{expr->elements.size()};
  if (elems != 3) {
    std::cout << "Incorrect number of elements. got " << elems << " want " << 3;
    return false;
  }

  if (!h_test_literal_expr<IntegerLiteral>(expr->elements[0], 1)) {
    return false;
  }

  auto result{false};
  auto mult_expr{
      h_assert_expr_type<InfixExpression>(expr->elements[1], result)};
  if (!result) {
    return false;
  }
  auto add_expr{h_assert_expr_type<InfixExpression>(expr->elements[2], result)};
  if (!result) {
    return false;
  }

  using namespace token_types;
  if (!test_single_infix_expression<IntegerLiteral>(mult_expr, 2, Asterisk{},
                                                    2)) {
    return false;
  }
  if (!test_single_infix_expression<IntegerLiteral>(add_expr, 3, Plus{}, 3)) {
    return false;
  }

  return true;
}
// }}}

// vim:foldmethod=marker
