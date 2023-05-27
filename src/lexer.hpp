#pragma once
#include "token.hpp"
#include <string>

class Lexer {
public:
  Lexer(std::string input);
  Token next_token();

private:
  void read_char();
  char peek_char();
  int read_number();
  void skip_whitespace();
  std::string read_identifier();
  std::string input;
  int position{0};
  int read_position{0};
  char ch{0};
};
