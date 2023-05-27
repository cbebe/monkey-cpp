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

using token_type =
    std::variant<Illegal, Eof, Ident, Int, Assign, Plus, Comma, Semicolon,
                 LParen, RParen, LBrace, RBrace, Function, Let>;
} // namespace token_types

template <class> inline constexpr bool always_false_v = false;

struct Token {
  token_types::token_type value;
  Token(token_types::token_type v) : value{v} {}

  std::string to_string() {
    using namespace token_types;
    return std::visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          std::string ret_val;
          if constexpr (std::is_same_v<T, Illegal>)
            ret_val = "ILLEGAL";
          else if constexpr (std::is_same_v<T, Eof>)
            ret_val = "EOF";
          else if constexpr (std::is_same_v<T, Ident>)
            ret_val = "IDENT(" + arg.literal + ")";
          else if constexpr (std::is_same_v<T, Int>)
            ret_val = "INT(" + std::to_string(arg.value) + ")";
          else if constexpr (std::is_same_v<T, Assign>)
            ret_val = "ASSIGN";
          else if constexpr (std::is_same_v<T, Plus>)
            ret_val = "PLUS";
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
          else
            static_assert(always_false_v<T>, "non-exhaustive visitor!");

          return ret_val;
        },
        value);
  }
};
