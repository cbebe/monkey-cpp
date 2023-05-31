#pragma once
#include "../src/parser.hpp"
#include <iostream>

bool first_next_token_test();
bool second_next_token_test();
bool third_next_token_test();

std::unique_ptr<Program> parser_pre_checks(Parser &p,
                                           std::unique_ptr<Program> program,
                                           size_t num_statements);
Parser parse_input(std::string input);

template <class StatementType>
bool assert_type(Statement *to_convert, StatementType *&var) {
  var = dynamic_cast<StatementType *>(to_convert);
  if (!var) {
    std::cout << "Failed test: not a " << typeid(StatementType).name()
              << std::endl;
    return false;
  }
  return true;
}

bool test_let_statements();
bool test_return_statements();

template <typename To, typename From, typename Deleter>
std::unique_ptr<To, Deleter>
dynamic_unique_cast(std::unique_ptr<From, Deleter> &&p) {
  if (To *cast = dynamic_cast<To *>(p.get())) {
    std::unique_ptr<To, Deleter> result(cast, std::move(p.get_deleter()));
    (void)p.release();
    return result;
  }
  return std::unique_ptr<To, Deleter>(nullptr);
}

bool test_identifier_expression();
bool test_integer_literal_expression();
bool test_prefix_expression();
bool test_infix_expression();
bool test_operator_precedence();
