#include "token.hpp"

std::string Token::to_string() const {
  using namespace token_types;
  return std::visit(
      [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        std::string ret_val;
        if constexpr (std::is_same_v<T, Ident>)
          ret_val = type_string<T>() + "(" + arg.literal + ")";
        else if constexpr (std::is_same_v<T, Int>)
          ret_val = type_string<T>() + "(" + std::to_string(arg.value) + ")";
        else if constexpr (std::is_same_v<T, String>)
          ret_val = type_string<T>() + "(" + arg.value + ")";
        else
          ret_val = type_string<T>();
        return ret_val;
      },
      value);
}

std::string Token::literal() const { return literal_string(value); }

std::string type_to_string(token_types::TokenVariant v) {
  return Token{v}.to_string();
};

std::string literal_string(token_types::TokenVariant v) {
  return std::visit(
      [](auto &&arg) {
        using namespace token_types;
        using T = std::decay_t<decltype(arg)>;
        std::string ret_val;
        if constexpr (std::is_same_v<T, Illegal>)
          ret_val = "ILLEGAL";
        else if constexpr (std::is_same_v<T, Eof>)
          ret_val = "EOF";
        else if constexpr (std::is_same_v<T, Ident>)
          ret_val = arg.literal;
        else if constexpr (std::is_same_v<T, Int>)
          ret_val = arg.value;
        else if constexpr (std::is_same_v<T, String>)
          ret_val = arg.value;
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
      },
      v);
}
