#include "lexer/lexer.hpp"

#include "test.hpp"
#include <iostream>
#include <vector>

bool test_next_token();

int main() {
  bool pass{true};
  TEST(test_next_token, pass);
  return pass ? 0 : 1;
}

bool test_next_token() {
  auto input{R"(
=+(){},;


let five = 5;
let ten = 10;
let add = fn(x, y) {
  x + y;
};
let result = add(five, ten);


!-/*5;
5 < 10 > 5;
if (5 < 10) {
	return true;
} else {
	return false;
}

10 == 10;
10 != 9;

"foobar"
"foo bar"
)"};
  std::vector<std::string> tests{
      "ASSIGN",
      "PLUS",
      "LPAREN",
      "RPAREN",
      "LSQUIRLY",
      "RSQUIRLY",
      "COMMA",
      "SEMICOLON",

      "LET",
      "IDENT(five)",
      "ASSIGN",
      "INT(5)",
      "SEMICOLON",
      "LET",
      "IDENT(ten)",
      "ASSIGN",
      "INT(10)",
      "SEMICOLON",
      "LET",
      "IDENT(add)",
      "ASSIGN",
      "FUNCTION",
      "LPAREN",
      "IDENT(x)",
      "COMMA",
      "IDENT(y)",
      "RPAREN",
      "LSQUIRLY",
      "IDENT(x)",
      "PLUS",
      "IDENT(y)",
      "SEMICOLON",
      "RSQUIRLY",
      "SEMICOLON",
      "LET",
      "IDENT(result)",
      "ASSIGN",
      "IDENT(add)",
      "LPAREN",
      "IDENT(five)",
      "COMMA",
      "IDENT(ten)",
      "RPAREN",
      "SEMICOLON",

      "BANG",
      "MINUS",
      "SLASH",
      "ASTERISK",
      "INT(5)",
      "SEMICOLON",
      "INT(5)",
      "LT",
      "INT(10)",
      "GT",
      "INT(5)",
      "SEMICOLON",
      "IF",
      "LPAREN",
      "INT(5)",
      "LT",
      "INT(10)",
      "RPAREN",
      "LSQUIRLY",
      "RETURN",
      "TRUE",
      "SEMICOLON",
      "RSQUIRLY",
      "ELSE",
      "LSQUIRLY",
      "RETURN",
      "FALSE",
      "SEMICOLON",
      "RSQUIRLY",

      "INT(10)",
      "EQ",
      "INT(10)",
      "SEMICOLON",
      "INT(10)",
      "NOT_EQ",
      "INT(9)",
      "SEMICOLON",

      "STRING(foobar)",
      "STRING(foo bar)",

      "EOF",
  };
  Lexer l{input};
  for (std::size_t i = 0, e = tests.size(); i != e; ++i) {
    auto got{l.next_token().to_string()};
    if (got != tests[i]) {
      std::cout << "Failed test " << i + 1 << ". got: " << got
                << ". want: " << tests[i] << std::endl;
      return false;
    }
  }

  return true;
}
