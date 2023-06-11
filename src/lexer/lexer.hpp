#pragma once
#include "../token/token.hpp"

class Lexer {
public:
  Lexer(std::string input);
  Token next_token();

private:
  void read_char();
  char peek_char();
  IntType read_number();
  void skip_whitespace();
  std::string read_identifier();
  std::string read_string();
  std::string input;
  size_t position{0};
  size_t read_position{0};
  char ch{0};
};
