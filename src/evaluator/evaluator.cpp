#include "evaluator.hpp"
#include "../object/object.hpp"
#include <cassert>
#include <functional>

std::unique_ptr<Object>
eval_statements(std::vector<std::unique_ptr<Statement>> statements) {
  std::unique_ptr<Object> ret_val{nullptr};
  for (auto &s : statements) {
    ret_val = eval(std::move(s));
  }
  return ret_val;
}

std::unique_ptr<Object> eval(std::unique_ptr<Node> node) {
  Node *n = node.get();
  if (auto *p = dynamic_cast<Program *>(n)) {
    return eval_statements(std::move(p->statements));
  } else if (auto *e = dynamic_cast<ExpressionStatement *>(n)) {
    return eval(std::move(e->value));
  } else if (auto *e = dynamic_cast<IntegerLiteral *>(n)) {
    return std::make_unique<Integer>(Integer{e->value});
  } else if (auto *b = dynamic_cast<BooleanLiteral *>(n)) {
    return std::make_unique<Boolean>(Boolean{b->value});
  } else {
    return nullptr;
  }
}

// vim:foldmethod=marker
