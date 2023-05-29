#include "src/lexer.hpp"
#include "src/parser.hpp"
#include <iostream>
#include <vector>

bool test_next_token(std::vector<std::string> tests, std::string input) {
  Lexer l{input};
  for (std::size_t i = 0, e = tests.size(); i != e; ++i) {
    auto got{l.next_token().to_string()};
    if (got != tests[i]) {
      std::cout << "Failed test " << i + 1 << ". got: " << got
                << ". want: " << tests[i] << std::endl;
      return false;
    }
  }

  std::cout << "PASS" << std::endl;
  return true;
}

Token t(token_types::TokenVariant type) { return Token{type}; }

bool first_next_token_test() {
  auto input{"=+(){},;"};
  std::vector<std::string> tests{
      "ASSIGN", "PLUS",  "LPAREN",    "RPAREN", "LBRACE",
      "RBRACE", "COMMA", "SEMICOLON", "EOF",
  };
  return test_next_token(tests, input);
}

bool second_next_token_test() {
  std::string input{"let five = 5;\
let ten = 10;\
\
let add = fn(x, y) {\
  x + y;\
};\
\
let result = add(five, ten);\
"};
  std::vector<std::string> tests{
      "LET",       "IDENT(five)", "ASSIGN",        "INT(5)",     "SEMICOLON",
      "LET",       "IDENT(ten)",  "ASSIGN",        "INT(10)",    "SEMICOLON",
      "LET",       "IDENT(add)",  "ASSIGN",        "FUNCTION",   "LPAREN",
      "IDENT(x)",  "COMMA",       "IDENT(y)",      "RPAREN",     "LBRACE",
      "IDENT(x)",  "PLUS",        "IDENT(y)",      "SEMICOLON",  "RBRACE",
      "SEMICOLON", "LET",         "IDENT(result)", "ASSIGN",     "IDENT(add)",
      "LPAREN",    "IDENT(five)", "COMMA",         "IDENT(ten)", "RPAREN",
      "SEMICOLON", "EOF",
  };
  return test_next_token(tests, input);
}

bool third_next_token_test() {
  std::string input{"\
!-/*5;\
5 < 10 > 5;\
if (5 < 10) {\
	return true;\
} else {\
	return false;\
}\
\
10 == 10;\
10 != 9;\
"};
  std::vector<std::string> tests{

      "BANG",      "MINUS",   "SLASH",     "ASTERISK",  "INT(5)",  "SEMICOLON",
      "INT(5)",    "LT",      "INT(10)",   "GT",        "INT(5)",  "SEMICOLON",
      "IF",        "LPAREN",  "INT(5)",    "LT",        "INT(10)", "RPAREN",
      "LBRACE",    "RETURN",  "TRUE",      "SEMICOLON", "RBRACE",  "ELSE",
      "LBRACE",    "RETURN",  "FALSE",     "SEMICOLON", "RBRACE",  "INT(10)",
      "EQ",        "INT(10)", "SEMICOLON", "INT(10)",   "NOT_EQ",  "INT(9)",
      "SEMICOLON", "EOF",
  };
  return test_next_token(tests, input);
}

std::unique_ptr<Program> parser_pre_checks(Parser &p,
                                           std::unique_ptr<Program> program,
                                           int num_statements) {
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
  if (length != 3) {
    std::cout << "Failed test: program does not contain 3 statements. Got: "
              << length << std::endl;
    return nullptr;
  }

  return std::move(program);
}

bool test_let_statements() {
  std::string input{"\
let x = 5;\
let y = 10;\
let foobar = 838383;\
"};
  Lexer l{input};
  Parser p{l};

  auto program{p.parse_program()};

  std::vector tests{"x", "y", "foobar"};
  program = parser_pre_checks(p, std::move(program), tests.size());
  if (!program) {
    return false;
  }

  for (std::size_t i = 0; i < program->statements.size(); i++) {
    auto statement{dynamic_cast<LetStatement *>(program->statements[i])};
    if (!statement) {
      std::cout << "Failed test: not a let statement" << std::endl;
      return false;
    }
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
  std::string input{"\
return 5;\
return 10;\
return 993322;\
"};
  Lexer l{input};
  Parser p{l};

  auto program{p.parse_program()};

  program = parser_pre_checks(p, std::move(program), 3);
  if (!program) {
    return false;
  }

  for (std::size_t i = 0; i < program->statements.size(); i++) {
    auto statement{dynamic_cast<ReturnStatement *>(program->statements[i])};
    if (!statement) {
      std::cout << "Failed test: not a return statement" << std::endl;
      return false;
    }
  }

  std::cout << "PASS" << std::endl;
  return true;
}

int main() {
  if (!first_next_token_test()) {
    return 1;
  }
  if (!second_next_token_test()) {
    return 1;
  }
  if (!third_next_token_test()) {
    return 1;
  }
  if (!test_let_statements()) {
    return 1;
  }
  if (!test_return_statements()) {
    return 1;
  }

  return 0;
}
