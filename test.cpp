#include "src/lexer.hpp"
#include "src/parser.hpp"
#include <iostream>
#include <vector>

bool test_next_token(std::vector<std::string> tests, std::string input) {
  Lexer l{input};
  for (std::size_t i = 0, e = tests.size(); i != e; ++i) {
    auto got{l.next_token()};
    if (got.to_string() != tests[i]) {
      std::cout << "Failed test " << i + 1 << ". got: " << got.to_string()
                << ". want: " << tests[i] << std::endl;
      return false;
    }
  }

  std::cout << "PASS" << std::endl;
  return true;
}

Token t(token_types::token_type type) { return Token{type}; }

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

bool test_let_statements() {
  std::string input{"\
let x = 5;\
let y = 10;\
let foobar = 838383;\
"};
  Lexer l{input};
  Parser p{l};

  auto program{p.parse_program()};
  if (!program) {
    std::cout << "Failed test: program is null" << std::endl;
    return false;
  }
  if (program->statements.size() != 3) {
    std::cout << "Failed test: program does not contain 3 statements"
              << std::endl;
    return false;
  }

  std::vector tests{"x", "y", "foobar"};

  for (std::size_t i = 0; i < program->statements.size(); i++) {
    auto statement{dynamic_cast<LetStatement *>(&program->statements[i])};
    if (!statement) {
      std::cout << "Failed test: not a let statement" << std::endl;
      return false;
    }
    auto ident{statement->identifier.value};
    if (ident != tests[i]) {
      std::cout << "Failed test: got identifier: " << ident
                << ". want: " << tests[i] << std::endl;
    }
  }

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

  return 0;
}
