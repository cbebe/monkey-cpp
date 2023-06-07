#pragma once
#include "../ast/ast.hpp"
#include "../lexer/lexer.hpp"
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
  INDEX,       // array[index]
};

class Parser;

typedef std::shared_ptr<Expression> (Parser::*PrefixParseFn)(void);
typedef std::shared_ptr<Expression> (Parser::*InfixParseFn)(
    std::shared_ptr<Expression>);

class Parser {
public:
  Parser(Lexer);
  std::shared_ptr<Program> parse_program();
  std::vector<std::string> errors{};

private:
  void next_token();
  template <typename TokenType> bool expect_peek();
  template <typename TokenType> void peek_error(Token);
  // Statements
  std::shared_ptr<Statement> parse_statement();
  std::shared_ptr<Statement> parse_let_statement();
  std::shared_ptr<Statement> parse_return_statement();
  std::shared_ptr<Statement> parse_expression_statement();
  std::shared_ptr<BlockStatement> parse_block_statement();

  // Expressions
  std::shared_ptr<Expression> parse_expression(Precedence);
  std::shared_ptr<Expression> parse_identifier();
  std::shared_ptr<Expression> parse_integer_literal();
  std::shared_ptr<Expression> parse_boolean_literal();
  std::shared_ptr<Expression> parse_string_literal();
  std::shared_ptr<Expression> parse_array_literal();
  std::shared_ptr<Expression> parse_function_literal();
  std::shared_ptr<Expression> parse_if_expression();
  std::shared_ptr<Expression> parse_grouped_expression();
  std::shared_ptr<Expression> parse_prefix_expression();
  std::shared_ptr<Expression>
      parse_infix_expression(std::shared_ptr<Expression>);
  std::shared_ptr<Expression>
      parse_call_expression(std::shared_ptr<Expression>);
  std::shared_ptr<Expression>
      parse_index_expression(std::shared_ptr<Expression>);

  std::vector<Identifier> parse_function_parameters();
  template <typename T>
  std::vector<std::shared_ptr<Expression>> parse_expression_list();

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
