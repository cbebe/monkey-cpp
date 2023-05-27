#include "lexer.cpp"
#include <iostream>

bool test_next_token() {
  std::string input{"=+(){},;"};
  auto tests = {
      "ASSIGN", "PLUS",   "LPAREN", "RPAREN",
      "LBRACE", "RBRACE", "COMMA",  "SEMICOLON",
  };

  Lexer l{input};
  for (auto c : tests) {
    auto got = l.next_token().to_string();
    if (got != c) {
      std::cout << "Failed test, got: " << got << ". want: " << c << std::endl;
      return false;
    }
  }

  return true;
}

int main() {
  if (!test_next_token()) {
    return 1;
  }

  return 0;
}
