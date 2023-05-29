#include "token.hpp"
#include <string>
#include <variant>

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

std::string Token::to_string() const {
  using namespace token_types;
  return std::visit(
      [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        std::string ret_val;
        if constexpr (std::is_same_v<T, Ident>)
          ret_val = type_string<Ident>() + "(" + arg.literal + ")";
        else if constexpr (std::is_same_v<T, Int>)
          ret_val = type_string<Int>() + "(" + std::to_string(arg.value) + ")";
        else
          ret_val = type_string<T>();
        return ret_val;
      },
      value);
}
