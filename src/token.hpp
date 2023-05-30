#pragma once
#include <string>
#include <variant>

// I am so sorry, can't resist my C ways
#define TOKEN_TYPE(CLASS)                                                      \
  struct CLASS {                                                               \
    bool operator==(const CLASS &) const { return true; }                      \
  }

namespace token_types {
TOKEN_TYPE(Illegal);
TOKEN_TYPE(Eof);

// Identifiers + Literals
struct Ident {
  std::string literal;
  bool operator==(const Ident &) const { return true; }
};
struct Int {
  int value;
  bool operator==(const Int &) const { return true; }
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

template <class> inline constexpr bool always_false_v = false;

template <typename T> std::string type_string() {
  using namespace token_types;
  std::string ret_val;
  if constexpr (std::is_same_v<T, Illegal>)
    ret_val = "ILLEGAL";
  else if constexpr (std::is_same_v<T, Eof>)
    ret_val = "EOF";
  else if constexpr (std::is_same_v<T, Ident>)
    ret_val = "IDENT";
  else if constexpr (std::is_same_v<T, Int>)
    ret_val = "INT";
  else if constexpr (std::is_same_v<T, Assign>)
    ret_val = "ASSIGN";
  else if constexpr (std::is_same_v<T, Plus>)
    ret_val = "PLUS";
  else if constexpr (std::is_same_v<T, Minus>)
    ret_val = "MINUS";
  else if constexpr (std::is_same_v<T, Bang>)
    ret_val = "BANG";
  else if constexpr (std::is_same_v<T, Slash>)
    ret_val = "SLASH";
  else if constexpr (std::is_same_v<T, Asterisk>)
    ret_val = "ASTERISK";
  else if constexpr (std::is_same_v<T, LT>)
    ret_val = "LT";
  else if constexpr (std::is_same_v<T, GT>)
    ret_val = "GT";
  else if constexpr (std::is_same_v<T, Eq>)
    ret_val = "EQ";
  else if constexpr (std::is_same_v<T, NotEq>)
    ret_val = "NOT_EQ";
  else if constexpr (std::is_same_v<T, Comma>)
    ret_val = "COMMA";
  else if constexpr (std::is_same_v<T, Semicolon>)
    ret_val = "SEMICOLON";
  else if constexpr (std::is_same_v<T, LParen>)
    ret_val = "LPAREN";
  else if constexpr (std::is_same_v<T, RParen>)
    ret_val = "RPAREN";
  else if constexpr (std::is_same_v<T, LBrace>)
    ret_val = "LBRACE";
  else if constexpr (std::is_same_v<T, RBrace>)
    ret_val = "RBRACE";
  else if constexpr (std::is_same_v<T, Function>)
    ret_val = "FUNCTION";
  else if constexpr (std::is_same_v<T, Let>)
    ret_val = "LET";
  else if constexpr (std::is_same_v<T, True>)
    ret_val = "TRUE";
  else if constexpr (std::is_same_v<T, False>)
    ret_val = "FALSE";
  else if constexpr (std::is_same_v<T, If>)
    ret_val = "IF";
  else if constexpr (std::is_same_v<T, Else>)
    ret_val = "ELSE";
  else if constexpr (std::is_same_v<T, Return>)
    ret_val = "RETURN";
  else
    static_assert(always_false_v<T>, "non-exhaustive visitor!");

  return ret_val;
}