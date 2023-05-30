#pragma once
#include <string>
#include <variant>

// I am so sorry, can't resist my C ways
#define TOKEN_TYPE(CLASS)                                                      \
  struct CLASS {                                                               \
    bool operator==(const CLASS &other) const { return true; }                 \
  }

namespace token_types {
TOKEN_TYPE(Illegal);
TOKEN_TYPE(Eof);

// Identifiers + Literals
struct Ident {
  std::string literal;
  bool operator==(const Ident &other) const { return true; }
};
struct Int {
  int value;
  bool operator==(const Int &other) const { return true; }
};

// Operators
TOKEN_TYPE(Assign);
TOKEN_TYPE(Plus);
TOKEN_TYPE(Minus);
TOKEN_TYPE(Bang);
TOKEN_TYPE(Asterisk);
TOKEN_TYPE(Slash);

TOKEN_TYPE(LT);
TOKEN_TYPE(GT);
TOKEN_TYPE(Eq);
TOKEN_TYPE(NotEq);

// Delimiters
TOKEN_TYPE(Comma);
TOKEN_TYPE(Semicolon);

TOKEN_TYPE(LParen);
TOKEN_TYPE(RParen);
TOKEN_TYPE(LBrace);
TOKEN_TYPE(RBrace);

// Keywords
TOKEN_TYPE(Function);
TOKEN_TYPE(Let);
TOKEN_TYPE(True);
TOKEN_TYPE(False);
TOKEN_TYPE(If);
TOKEN_TYPE(Else);
TOKEN_TYPE(Return);

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
