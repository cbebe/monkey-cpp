#include <iostream>

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
} // namespace token_types

int main() {
  std::cout << "Hello, world!\n";
  return 0;
}
