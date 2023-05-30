#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

using PrefixParseFn = std::function<void(Expression *)>;
using InfixParseFn = std::function<Expression *(Expression *)>;

namespace std {
template <> struct hash<token_types::TokenVariant> {
  std::size_t operator()(const token_types::TokenVariant &k) const {
    return std::visit(
        [](auto &&arg) {
          return std::hash<std::string>()(
              type_string<std::decay_t<decltype(arg)>>());
        },
        k);
  }
};
} // namespace std

class Parser {
public:
  Parser(Lexer l);
  std::unique_ptr<Program> parse_program();
  std::vector<std::string> errors{};

private:
  void next_token();
  template <typename TokenType> bool expect_peek();
  template <typename TokenType> void peek_error(Token t);
  Statement *parse_statement();
  LetStatement *parse_let_statement();
  ReturnStatement *parse_return_statement();
  void register_prefix(token_types::TokenVariant, PrefixParseFn fn);
  void register_infix(token_types::TokenVariant, InfixParseFn fn);
  Token cur_token;
  Token peek_token;
  Lexer lexer;
  std::unordered_map<token_types::TokenVariant, PrefixParseFn>
      prefix_parse_fns{};
  std::unordered_map<token_types::TokenVariant, InfixParseFn> infix_parse_fns{};
};
