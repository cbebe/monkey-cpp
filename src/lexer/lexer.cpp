#include "lexer.hpp"
#include <string>

Lexer::Lexer(std::string input) : input{input} { read_char(); };

Token new_token(token_types::TokenVariant t) { return Token{t}; }

bool is_letter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

bool is_digit(char ch) { return '0' <= ch && ch <= '9'; }

Token lookup_identifier(std::string word) {
  using namespace token_types;
  if (word == "fn") {
    return new_token(Function{});
  } else if (word == "let") {
    return new_token(Let{});
  } else if (word == "if") {
    return new_token(If{});
  } else if (word == "else") {
    return new_token(Else{});
  } else if (word == "true") {
    return new_token(True{});
  } else if (word == "false") {
    return new_token(False{});
  } else if (word == "return") {
    return new_token(Return{});
  } else {
    return new_token(Ident{word});
  }
}

Token Lexer::next_token() {
  using namespace token_types;
  Token tok;
  skip_whitespace();
  switch (ch) {
  case '=': {
    if (peek_char() == '=') {
      read_char();
      tok = new_token(Eq{});
    } else {
      tok = new_token(Assign{});
    }
    break;
  }
  case '!': {
    if (peek_char() == '=') {
      read_char();
      tok = new_token(NotEq{});
    } else {
      tok = new_token(Bang{});
    }
    break;
  }
  // clang-format off
  case '"': tok = new_token(String{read_string()}); break;
  case ';': tok = new_token(Semicolon{}); break;
  case ':': tok = new_token(Colon{}); break;
  case '(': tok = new_token(LParen{}); break;
  case ')': tok = new_token(RParen{}); break;
  case ',': tok = new_token(Comma{}); break;
  case '+': tok = new_token(Plus{}); break;
  case '-': tok = new_token(Minus{}); break;
  case '/': tok = new_token(Slash{}); break;
  case '*': tok = new_token(Asterisk{}); break;
  case '<': tok = new_token(LT{}); break;
  case '>': tok = new_token(GT{}); break;
  case '{': tok = new_token(LSquirly{}); break;
  case '}': tok = new_token(RSquirly{}); break;
  case '[': tok = new_token(LSquarely{}); break;
  case ']': tok = new_token(RSquarely{}); break;
  case   0: tok = new_token(Eof{}); break;
  // clang-format on
  default:
    if (is_letter(ch)) {
      return lookup_identifier(read_identifier());
    } else if (is_digit(ch)) {
      return new_token(Int{read_number()});
    } else {
      tok = new_token(Illegal{});
    }
  }
  read_char();
  return tok;
}

std::string Lexer::read_string() {
  auto start{position + 1};
  while (true) {
    read_char();
    if (ch == '"' || ch == 0) {
      break;
    }
  }
  return input.substr(start, position - start);
}

std::string Lexer::read_identifier() {
  auto start{position};
  while (is_letter(ch)) {
    read_char();
  }
  return input.substr(start, position - start);
}

void Lexer::read_char() {
  if (read_position >= input.length()) {
    ch = 0;
  } else {
    ch = input[read_position];
  }
  position = read_position++;
}

char Lexer::peek_char() {
  if (read_position >= input.length()) {
    return 0;
  } else {
    return input[read_position];
  }
}

void Lexer::skip_whitespace() {
  while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
    read_char();
  }
}

IntType Lexer::read_number() {
  auto start{position};
  while (is_digit(ch)) {
    read_char();
  }
  return std::stol(input.substr(start, position - start));
}
