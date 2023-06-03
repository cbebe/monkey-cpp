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
  if (auto *b{dynamic_cast<Boolean *>(obj)}) {
    return std::make_unique<Boolean>(b->value ? _FALSE : _TRUE);
  } else if (dynamic_cast<Null *>(obj)) {
    return std::make_unique<Boolean>(_TRUE);
  } else {
    return std::make_unique<Boolean>(_FALSE);
  }
}

std::unique_ptr<Object>
eval_minus_operator_expression(std::unique_ptr<Object> expr) {
  Object *obj{expr.get()};
  if (auto *i{dynamic_cast<Integer *>(obj)}) {
    return std::make_unique<Integer>(-i->value);
  } else {
    return std::make_unique<Null>(_NULL);
  }
}

std::unique_ptr<Object> eval_prefx_expression(Token oper,
                                              std::unique_ptr<Object> right) {
  std::unique_ptr<Object> ret_val{nullptr};
  if (oper.is_type<token_types::Bang>()) {
    ret_val = eval_bang_operator_expression(std::move(right));
  } else if (oper.is_type<token_types::Minus>()) {
    ret_val = eval_minus_operator_expression(std::move(right));
  } else {
    ret_val = std::make_unique<Null>(_NULL);
  }

  return ret_val;
}

std::unique_ptr<Object> eval(std::unique_ptr<Node> node) {
  Node *n = node.get();
  if (auto *p = dynamic_cast<Program *>(n)) {
    return eval_statements(std::move(p->statements));
  } else if (auto *e{dynamic_cast<ExpressionStatement *>(n)}) {
    return eval(std::move(e->value));
  } else if (auto *e{dynamic_cast<PrefixExpression *>(n)}) {
    return eval_prefx_expression(Token{e->oper}, eval(std::move(e->right)));
  } else if (auto *e{dynamic_cast<IntegerLiteral *>(n)}) {
    return std::make_unique<Integer>(Integer{e->value});
  } else if (auto *b{dynamic_cast<BooleanLiteral *>(n)}) {
    return std::make_unique<Boolean>(b->value ? _TRUE : _FALSE);
  } else {
    return nullptr;
  }
}

// vim:foldmethod=marker
