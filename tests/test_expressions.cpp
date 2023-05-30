#include "../src/parser.hpp"
#include "tests.hpp"
#include <iostream>
#include <vector>

bool test_identifier_expression() {
  Parser p{parse_input("foobar;")};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 1);
  if (!program) {
    return false;
  }

  ExpressionStatement *statement;
  ASSERT_STATEMENT_TYPE(ExpressionStatement, program->statements[0], statement);
  auto ident{dynamic_unique_cast<Identifier>(std::move(statement->value))};
  if (!ident) {
    std::cout << "Failed test: not an identifier" << std::endl;
    return false;
  }
  if (ident->value != "foobar") {
    std::cout << "Failed test: not foobar" << std::endl;
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
  ASSERT_STATEMENT_TYPE(ExpressionStatement, program->statements[0], statement);
  auto ident{dynamic_unique_cast<IntegerLiteral>(std::move(statement->value))};
  if (!ident) {
    std::cout << "Failed test: not an integer literal" << std::endl;
    return false;
  }
  if (ident->value != 5) {
    std::cout << "Failed test: not 5" << std::endl;
    return false;
  }
  std::cout << "PASS" << std::endl;
  return true;
}
