#include "../src/parser.hpp"
#include "tests.hpp"
#include <any>
#include <iostream>

std::unique_ptr<Program>
check_errors(Parser &p, std::unique_ptr<Program> program, bool &pass) {
  if (p.errors.size() > 0) {
    std::cout << "parser error:" << std::endl;
    for (auto &e : p.errors) {
      std::cout << e << std::endl;
    }
    pass = false;
  };
  return program;
}

std::unique_ptr<Program> parser_pre_checks(Parser &p,
                                           std::unique_ptr<Program> program,
                                           size_t num_statements) {
  bool pass{true};
  program = check_errors(p, std::move(program), pass);
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

Parser parse_input(std::string input) { return Parser{Lexer{input}}; }

template <typename Expr, typename T>
bool test_let(std::string input, std::string name, T value) {
  Parser p{parse_input(input)};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }
  auto result{true};
  auto statement{assert_statement_type<LetStatement>(
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
  return test_literal_expr<Expr, T>(std::move(statement->value), value);
}

template <typename Expr, typename T>
bool test_return(std::string input, T value) {
  Parser p{parse_input(input)};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }
  auto result{true};
  auto statement{assert_statement_type<ReturnStatement>(
      std::move(program->statements[0]), result)};
  if (!result) {
    return false;
  }
  return test_literal_expr<Expr, T>(std::move(statement->value), value);
}

bool test_let_statements() {
  return test_let<IntegerLiteral>("let x = 5;", "x", 5) &&
         test_let<BooleanLiteral>("let y = true;", "y", true) &&
         test_let<Identifier>("let foobar = y;", "foobar", "y");
}

bool test_return_statements() {
  return test_return<IntegerLiteral>("return 5;", 5) &&
         test_return<BooleanLiteral>("return true;", true) &&
         test_return<Identifier>("return foobar;", "foobar");
}
