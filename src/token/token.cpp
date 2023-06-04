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

std::string Token::literal() const {
  using namespace token_types;
  return std::visit(
      [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        std::string ret_val;
        if constexpr (std::is_same_v<T, Ident>)
          ret_val = arg.literal;
        else if constexpr (std::is_same_v<T, Int>)
          ret_val = arg.value;
        else if constexpr (std::is_same_v<T, String>)
          ret_val = arg.value;
        else
          ret_val = literal_string<T>();
        return ret_val;
      },
      value);
}

std::string type_to_string(token_types::TokenVariant v) {
  return Token{v}.to_string();
};
