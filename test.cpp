#include "lexer.cpp"
#include <iostream>
#include <vector>

bool test_next_token(std::vector<std::string> tests, std::string input) {
  Lexer l{input};
  for (auto c : tests) {
    auto got = l.next_token();
    if (got.to_string() != c) {
      std::cout << "Failed test, got: " << got.to_string() << ". want: " << c
                << std::endl;
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

int main() {
  if (!first_next_token_test()) {
    return 1;
  }
  if (!second_next_token_test()) {
    return 1;
  }

  return 0;
}
