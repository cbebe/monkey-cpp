#include "../src/parser.hpp"
#include "tests.hpp"
#include <iostream>

std::unique_ptr<Program> parser_pre_checks(Parser &p,
                                           std::unique_ptr<Program> program,
                                           size_t num_statements) {
  if (p.errors.size() > 0) {
    std::cout << "parser error:" << std::endl;
    for (auto &e : p.errors) {
      std::cout << e << std::endl;
    }

    return nullptr;
  };
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

bool test_let_statements() {
  Parser p{parse_input("\
let x = 5;\
let y = 10;\
let foobar = 838383;\
")};
  auto program{p.parse_program()};

  std::vector tests{"x", "y", "foobar"};
  program = parser_pre_checks(p, std::move(program), tests.size());
  if (!program) {
    return false;
  }

  for (std::size_t i = 0; i < program->statements.size(); i++) {
    LetStatement *statement;
    ASSERT_STATEMENT_TYPE(LetStatement, program->statements[i], statement);
    auto ident{statement->identifier.value};
    if (ident != tests[i]) {
      std::cout << "Failed test: got identifier: " << ident
                << ". want: " << tests[i] << std::endl;
      return false;
    }
  }

  std::cout << "PASS" << std::endl;
  return true;
}

bool test_return_statements() {
  Parser p{parse_input("\
return 5;\
return 10;\
return 993322;\
")};
  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 3);
  if (!program) {
    return false;
  }

  for (std::size_t i = 0; i < program->statements.size(); i++) {
    ReturnStatement *statement;
    ASSERT_STATEMENT_TYPE(ReturnStatement, program->statements[i], statement);
  }

  std::cout << "PASS" << std::endl;
  return true;
}
