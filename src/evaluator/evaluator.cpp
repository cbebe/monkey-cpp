#include "evaluator.hpp"
#include "../object/object.hpp"
#include <cassert>
#include <functional>

const Boolean _TRUE = Boolean{true};
const Boolean _FALSE = Boolean{false};
const Null _NULL = Null{};

std::unique_ptr<Object>
eval_statements(std::vector<std::unique_ptr<Statement>> statements) {
  std::unique_ptr<Object> ret_val{nullptr};
  for (auto &s : statements) {
    ret_val = eval(std::move(s));
  }
  return ret_val;
}

std::unique_ptr<Object>
eval_bang_operator_expression(std::unique_ptr<Object> expr) {
  Object *obj{expr.get()};
  switch (obj->type()) {
  case INTEGER_OBJ:
    return std::make_unique<Boolean>(
        static_cast<Integer *>(obj)->value ? _FALSE : _TRUE);
  case BOOLEAN_OBJ:
    return std::make_unique<Boolean>(
        static_cast<Boolean *>(obj)->value ? _FALSE : _TRUE);
  case NULL_OBJ:
    return std::make_unique<Boolean>(_TRUE);
  default:
    return std::make_unique<Boolean>(_FALSE);
  }
}

std::unique_ptr<Object> null() { return std::make_unique<Null>(_NULL); }

std::unique_ptr<Object>
eval_minus_operator_expression(std::unique_ptr<Object> expr) {
  Object *obj{expr.get()};
  if (obj->type() == INTEGER_OBJ) {
    return std::make_unique<Integer>(-(static_cast<Integer *>(obj)->value));
  } else {
    return null();
  }
}

std::unique_ptr<Object> eval_prefix_expression(Token oper,
                                               std::unique_ptr<Object> right) {
  std::unique_ptr<Object> ret_val{nullptr};
  if (oper.is_type<token_types::Bang>()) {
    ret_val = eval_bang_operator_expression(std::move(right));
  } else if (oper.is_type<token_types::Minus>()) {
    ret_val = eval_minus_operator_expression(std::move(right));
  } else {
    ret_val = null();
  }

  return ret_val;
}

std::unique_ptr<Object>
eval_integer_infix_expression(std::unique_ptr<Object> left, Token oper,
                              std::unique_ptr<Object> right) {
  using namespace token_types;
  long ret_val{};
  auto lhs{static_cast<Integer *>(left.get())->value};
  auto rhs{static_cast<Integer *>(right.get())->value};
  if (oper.is_type<Plus>()) {
    ret_val = lhs + rhs;
  } else if (oper.is_type<Minus>()) {
    ret_val = lhs - rhs;
  } else if (oper.is_type<Asterisk>()) {
    ret_val = lhs * rhs;
  } else if (oper.is_type<Slash>()) {
    ret_val = lhs / rhs;
  } else {
    return null();
  }
  return std::make_unique<Integer>(ret_val);
}

std::unique_ptr<Object> eval_infix_expression(std::unique_ptr<Object> left,
                                              Token oper,
                                              std::unique_ptr<Object> right) {
  if (left->type() == INTEGER_OBJ && right->type() == INTEGER_OBJ) {
    return eval_integer_infix_expression(std::move(left), oper,
                                         std::move(right));
  } else {
    return null();
  }
}

std::unique_ptr<Object> eval(std::unique_ptr<Node> node) {
  Node *n = node.get();
  if (auto *p = dynamic_cast<Program *>(n)) {
    return eval_statements(std::move(p->statements));
  } else if (auto *e{dynamic_cast<ExpressionStatement *>(n)}) {
    return eval(std::move(e->value));
  } else if (auto *e{dynamic_cast<PrefixExpression *>(n)}) {
    return eval_prefix_expression(Token{e->oper}, eval(std::move(e->right)));
  } else if (auto *e{dynamic_cast<InfixExpression *>(n)}) {
    return eval_infix_expression(eval(std::move(e->left)), Token{e->oper},
                                 eval(std::move(e->right)));
  } else if (auto *e{dynamic_cast<IntegerLiteral *>(n)}) {
    return std::make_unique<Integer>(e->value);
  } else if (auto *b{dynamic_cast<BooleanLiteral *>(n)}) {
    return std::make_unique<Boolean>(b->value ? _TRUE : _FALSE);
  } else {
    return nullptr;
  }
}

// vim:foldmethod=marker
