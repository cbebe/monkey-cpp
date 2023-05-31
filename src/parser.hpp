#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <unordered_map>

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

enum Precedence {
  _ = 0,
  LOWEST,
  EQUALS,      // ==
  LESSGREATER, // > or <
  SUM,         // +
  PRODUCT,     // *
  PREFIX,      // -X or !X
  CALL,        // myFunction(X)
};

class Parser;

typedef Expression *(Parser::*PrefixParseFn)(void);
typedef Expression *(Parser::*InfixParseFn)(Expression *);

class Parser {
public:
  Parser(Lexer);
  std::unique_ptr<Program> parse_program();
  std::vector<std::string> errors{};

private:
  void next_token();
  template <typename TokenType> bool expect_peek();
  template <typename TokenType> void peek_error(Token);
  // Statements
  Statement *parse_statement();
  LetStatement *parse_let_statement();
  ReturnStatement *parse_return_statement();
  ExpressionStatement *parse_expression_statement();

  // Expressions
  Expression *parse_expression(Precedence);
  Expression *parse_identifier();
  Expression *parse_integer_literal();
  Expression *parse_boolean_literal();
  Expression *parse_prefix_expression();
  Expression *parse_infix_expression(Expression *);

  void register_prefix(token_types::TokenVariant, PrefixParseFn);
  void register_infix(token_types::TokenVariant, InfixParseFn);

  Precedence peek_predence();
  Precedence cur_predence();

  Token cur_token;
  Token peek_token;
  Lexer lexer;
  std::unordered_map<token_types::TokenVariant, PrefixParseFn>
      prefix_parse_fns{};
  std::unordered_map<token_types::TokenVariant, InfixParseFn> infix_parse_fns{};
};

template <typename TokenType> void Parser::peek_error(Token t) {
  auto message = "expected next token to be " + type_string<TokenType>() +
                 ". got " + t.to_string();
  errors.push_back(message);
}

template <typename TokenType> bool Parser::expect_peek() {
  if (peek_token.is_type<TokenType>()) {
    next_token();
    return true;
  } else {
    peek_error<TokenType>(peek_token);
    return false;
  }
}
