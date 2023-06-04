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
  long value;
  bool operator==(const Int &) const { return true; }
};
struct String {
  std::string value;
  bool operator==(const String &) const { return true; }
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
TOKEN_TYPE(LSquirly);
TOKEN_TYPE(RSquirly);

// Keywords
TOKEN_TYPE(Function);
TOKEN_TYPE(Let);
TOKEN_TYPE(True);
TOKEN_TYPE(False);
TOKEN_TYPE(If);
TOKEN_TYPE(Else);
TOKEN_TYPE(Return);

using TokenVariant =
    std::variant<Illegal, Eof, Ident, Int, String, Assign, Plus, Minus, Bang,
                 Asterisk, Slash, LT, GT, Eq, NotEq, Comma, Semicolon, LParen,
                 RParen, LSquirly, RSquirly, Function, Let, True, False, If,
                 Else, Return>;

template <typename TokenType> bool is_type(TokenVariant t) {
  return std::visit(
      [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, TokenType>) {
          return true;
        }
        return false;
      },
      t);
}
} // namespace token_types

std::string type_to_string(token_types::TokenVariant);

struct Token {
  Token() : value{token_types::Eof{}} {}
  token_types::TokenVariant value;
  Token(token_types::TokenVariant v) : value{v} {}
  std::string to_string() const;
  template <typename TokenType> bool is_type() const {
    return token_types::is_type<TokenType>(value);
  }
  std::string literal() const;
};

template <class> inline constexpr bool always_false_v = false;

template <typename T> std::string literal_string() {
  using namespace token_types;
  std::string ret_val;
  if constexpr (std::is_same_v<T, Illegal>)
    ret_val = "ILLEGAL";
  else if constexpr (std::is_same_v<T, Eof>)
    ret_val = "EOF";
  else if constexpr (std::is_same_v<T, Assign>)
    ret_val = "=";
  else if constexpr (std::is_same_v<T, Plus>)
    ret_val = "+";
  else if constexpr (std::is_same_v<T, Minus>)
    ret_val = "-";
  else if constexpr (std::is_same_v<T, Bang>)
    ret_val = "!";
  else if constexpr (std::is_same_v<T, Slash>)
    ret_val = "/";
  else if constexpr (std::is_same_v<T, Asterisk>)
    ret_val = "*";
  else if constexpr (std::is_same_v<T, LT>)
    ret_val = "<";
  else if constexpr (std::is_same_v<T, GT>)
    ret_val = ">";
  else if constexpr (std::is_same_v<T, Eq>)
    ret_val = "==";
  else if constexpr (std::is_same_v<T, NotEq>)
    ret_val = "!=";
  else if constexpr (std::is_same_v<T, Comma>)
    ret_val = ",";
  else if constexpr (std::is_same_v<T, Semicolon>)
    ret_val = ";";
  else if constexpr (std::is_same_v<T, LParen>)
    ret_val = "(";
  else if constexpr (std::is_same_v<T, RParen>)
    ret_val = ")";
  else if constexpr (std::is_same_v<T, LSquirly>)
    ret_val = "{";
  else if constexpr (std::is_same_v<T, RSquirly>)
    ret_val = "}";
  else if constexpr (std::is_same_v<T, Function>)
    ret_val = "fn";
  else if constexpr (std::is_same_v<T, Let>)
    ret_val = "let";
  else if constexpr (std::is_same_v<T, True>)
    ret_val = "true";
  else if constexpr (std::is_same_v<T, False>)
    ret_val = "false";
  else if constexpr (std::is_same_v<T, If>)
    ret_val = "if";
  else if constexpr (std::is_same_v<T, Else>)
    ret_val = "else";
  else if constexpr (std::is_same_v<T, Return>)
    ret_val = "return";
  else
    static_assert(always_false_v<T>, "non-exhaustive visitor!");

  return ret_val;
}

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
  else if constexpr (std::is_same_v<T, String>)
    ret_val = "STRING";
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
  else if constexpr (std::is_same_v<T, LSquirly>)
    ret_val = "LSQUIRLY";
  else if constexpr (std::is_same_v<T, RSquirly>)
    ret_val = "RSQUIRLY";
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
