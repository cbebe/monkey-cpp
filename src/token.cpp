#include "token.hpp"

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
