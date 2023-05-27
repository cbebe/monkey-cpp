#pragma once
#include <string>
#include <variant>

namespace token_types {
struct Illegal {};
struct Eof {};

// Identifiers + Literals
struct Ident {
  std::string literal;
};
struct Int {
  int value;
};

// Operators
struct Assign {};
struct Plus {};
struct Minus {};
struct Bang {};
struct Asterisk {};
struct Slash {};

struct LT {};
struct GT {};
struct Eq {};
struct NotEq {};

// Delimiters
struct Comma {};
struct Semicolon {};

struct LParen {};
struct RParen {};
struct LBrace {};
struct RBrace {};

// Keywords
struct Function {};
struct Let {};
struct True {};
struct False {};
struct If {};
struct Else {};
struct Return {};

using token_type =
    std::variant<Illegal, Eof, Ident, Int, Assign, Plus, Minus, Bang, Asterisk,
                 Slash, LT, GT, Eq, NotEq, Comma, Semicolon, LParen, RParen,
                 LBrace, RBrace, Function, Let, True, False, If, Else, Return>;
} // namespace token_types

struct Token {
  token_types::token_type value;
  Token(token_types::token_type v) : value{v} {}
  std::string to_string();
};
