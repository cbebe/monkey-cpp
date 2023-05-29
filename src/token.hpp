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

using TokenVariant =
    std::variant<Illegal, Eof, Ident, Int, Assign, Plus, Minus, Bang, Asterisk,
                 Slash, LT, GT, Eq, NotEq, Comma, Semicolon, LParen, RParen,
                 LBrace, RBrace, Function, Let, True, False, If, Else, Return>;
} // namespace token_types

struct Token {
  Token() : value{token_types::Eof{}} {}
  token_types::TokenVariant value;
  Token(token_types::TokenVariant v) : value{v} {}
  std::string to_string() const;
  template <typename TokenType> bool is_type() const {
    return std::visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, TokenType>) {
            return true;
          }
          return false;
        },
        value);
  }
};

template <typename TokenType> std::string type_string();
