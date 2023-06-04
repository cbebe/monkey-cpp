#include "evaluator.hpp"
#include <functional>
#include <iostream>

const Boolean _TRUE = Boolean{true};
const Boolean _FALSE = Boolean{false};
const Null _NULL = Null{};

std::shared_ptr<Object> null() { return std::make_shared<Null>(_NULL); }
std::shared_ptr<Object> integer(long value) {
  return std::make_shared<Integer>(value);
}
std::shared_ptr<Object> boolean(bool value) {
  return std::make_shared<Boolean>(value ? _TRUE : _FALSE);
}
std::shared_ptr<Object> return_value(std::shared_ptr<Object> value) {
  return std::make_shared<ReturnValue>(std::move(value));
}
std::shared_ptr<Object> error(std::string message) {
  return std::make_shared<Error>(message);
}
std::shared_ptr<Object> function(std::vector<Identifier> params,
                                 std::unique_ptr<BlockStatement> body,
                                 std::shared_ptr<Environment> env) {
  return std::make_shared<Function>(std::move(params), std::move(body),
                                    std::move(env));
}

std::shared_ptr<Object> unknown_infix(ObjectType left, const Token &oper,
                                      ObjectType right) {
  return error("unknown operator: " + std::to_string(left) + " " +
               oper.literal() + " " + std::to_string(right));
}

std::shared_ptr<Object> unknown_prefix(const Token &oper, ObjectType right) {
  return error("unknown operator: " + oper.literal() + std::to_string(right));
}

bool is_truthy(Object *obj) {
  switch (obj->type()) {
  case INTEGER_OBJ:
    return static_cast<Integer *>(obj)->value;
  case BOOLEAN_OBJ:
    return static_cast<Boolean *>(obj)->value;
  case NULL_OBJ:
    return false;
  default:
    return true;
  }
}

bool is_error(Object *obj) {
  if (obj) {
    return obj->type() == ERROR_OBJ;
  }
  return false;
}

std::shared_ptr<Object>
eval_bang_operator_expression(std::shared_ptr<Object> expr) {
  return boolean(!is_truthy(expr.get()));
}

std::shared_ptr<Object>
eval_minus_operator_expression(std::shared_ptr<Object> expr) {
  Object *obj{expr.get()};
  if (obj->type() == INTEGER_OBJ) {
    return integer(-(static_cast<Integer *>(obj)->value));
  } else {
    return unknown_prefix(Token{token_types::Minus{}}, obj->type());
  }
}

std::shared_ptr<Object> eval_prefix_expression(Token oper,
                                               std::shared_ptr<Object> right) {
  std::shared_ptr<Object> ret_val{};
  if (oper.is_type<token_types::Bang>()) {
    ret_val = eval_bang_operator_expression(std::move(right));
  } else if (oper.is_type<token_types::Minus>()) {
    ret_val = eval_minus_operator_expression(std::move(right));
  } else {
    ret_val = unknown_prefix(oper, right->type());
  }

  return ret_val;
}

std::shared_ptr<Object>
eval_integer_infix_expression(std::shared_ptr<Object> left, Token oper,
                              std::shared_ptr<Object> right) {
  using namespace token_types;
  auto lhs{static_cast<Integer *>(left.get())->value};
  auto rhs{static_cast<Integer *>(right.get())->value};
  if (oper.is_type<Plus>()) {
    return integer(lhs + rhs);
  } else if (oper.is_type<Minus>()) {
    return integer(lhs - rhs);
  } else if (oper.is_type<Asterisk>()) {
    return integer(lhs * rhs);
  } else if (oper.is_type<Slash>()) {
    return integer(lhs / rhs);
  } else if (oper.is_type<LT>()) {
    return boolean(lhs < rhs);
  } else if (oper.is_type<GT>()) {
    return boolean(lhs > rhs);
  } else if (oper.is_type<Eq>()) {
    return boolean(lhs == rhs);
  } else if (oper.is_type<NotEq>()) {
    return boolean(lhs != rhs);
  } else {
    return unknown_infix(left->type(), oper, right->type());
  }
}

std::shared_ptr<Object> eval_infix_expression(std::shared_ptr<Object> left,
                                              Token oper,
                                              std::shared_ptr<Object> right) {
  if (left->type() == INTEGER_OBJ && right->type() == INTEGER_OBJ) {
    return eval_integer_infix_expression(std::move(left), oper,
                                         std::move(right));
  } else if (left->type() == BOOLEAN_OBJ && right->type() == BOOLEAN_OBJ) {
    auto lhs{static_cast<Boolean *>(left.get())->value};
    auto rhs{static_cast<Boolean *>(right.get())->value};
    if (oper.is_type<token_types::Eq>()) {
      return boolean(lhs == rhs);
    } else if (oper.is_type<token_types::NotEq>()) {
      return boolean(lhs != rhs);
    } else {
      return unknown_infix(left->type(), oper, right->type());
    }
  } else if (left->type() != right->type()) {
    return error("type mismatch: " + std::to_string(left->type()) + " " +
                 oper.literal() + " " + std::to_string(right->type()));
  } else {
    return unknown_infix(left->type(), oper, right->type());
  }
}

std::shared_ptr<Object>
eval_if_expression(std::unique_ptr<Expression> condition,
                   std::unique_ptr<BlockStatement> consequence,
                   std::unique_ptr<BlockStatement> alternative,
                   std::shared_ptr<Environment> env) {
  auto cond{eval(std::move(condition), env)};
  if (is_error(cond.get())) {
    return cond;
  }
  if (is_truthy(cond.get())) {
    return eval(std::move(consequence), env);
  } else if (alternative != nullptr) {
    return eval(std::move(alternative), env);
  } else {
    return null();
  }
}

std::shared_ptr<Object>
eval_program(std::vector<std::unique_ptr<Statement>> statements,
             std::shared_ptr<Environment> env) {
  std::shared_ptr<Object> result{};
  for (auto &s : statements) {
    result = eval(std::move(s), env);
    if (dynamic_cast<Error *>(result.get())) {
      return result;
    } else if (auto *ret_val{dynamic_cast<ReturnValue *>(result.get())}) {
      return std::move(ret_val->value);
    }
  }
  return result;
}

std::shared_ptr<Object>
eval_block_statement(std::vector<std::unique_ptr<Statement>> statements,
                     std::shared_ptr<Environment> env) {
  std::shared_ptr<Object> result{};
  for (auto &s : statements) {
    result = eval(std::move(s), env);
    if (result) {
      auto type{result->type()};
      if (type == RETURN_VALUE_OBJ || type == ERROR_OBJ) {
        return result;
      }
    }
  }
  return result;
}

std::shared_ptr<Object> eval_identifier(const std::string &name,
                                        std::shared_ptr<Environment> env) {
  auto val{env->get(name)};
  if (!val) {
    return error("identifier not found: " + name);
  }
  return val;
}
std::vector<std::shared_ptr<Object>>
eval_expressions(std::vector<std::unique_ptr<Expression>> expressions,
                 std::shared_ptr<Environment> env) {
  std::vector<std::shared_ptr<Object>> args{};
  for (size_t i = 0; i < expressions.size(); i++) {
    auto evaluated{eval(std::move(expressions[i]), env)};
    if (is_error(evaluated.get())) {
      return std::vector<std::shared_ptr<Object>>{std::move(evaluated)};
    }
    args.push_back(std::move(evaluated));
  }
  return args;
}

std::shared_ptr<Environment>
extend_function_env(std::shared_ptr<Function> func,
                    std::vector<std::shared_ptr<Object>> args) {
  auto extended{std::make_shared<Environment>(std::move(func->env))};
  for (size_t i = 0; i < args.size(); i++) {
    extended->set(func->params[i].value, std::move(args[i]));
  }
  return extended;
}

std::shared_ptr<Object> unwrap_return_value(std::shared_ptr<Object> obj) {
  if (obj->type() == RETURN_VALUE_OBJ) {
    return std::static_pointer_cast<ReturnValue>(obj)->value;
  }
  return obj;
}

std::shared_ptr<Object>
apply_function(std::shared_ptr<Object> obj,
               std::vector<std::shared_ptr<Object>> args) {
  if (obj->type() != FUNCTION_OBJ) {
    return error("not a function: " + std::to_string(obj->type()));
  }
  auto function{std::static_pointer_cast<Function>(obj)};
  auto extended_env{extend_function_env(function, args)};
  std::cout << extended_env->inspect() << std::endl;
  auto evaluated{eval_block_statement(std::move(function->body->statements),
                                      extended_env)};
  return unwrap_return_value(std::move(evaluated));
}

#include <iostream>
std::shared_ptr<Object> eval(std::unique_ptr<Node> node,
                             std::shared_ptr<Environment> env) {
  Node *n = node.get();
  if (auto *p = dynamic_cast<Program *>(n)) {
    return eval_program(std::move(p->statements), std::move(env));
  } else if (auto *e{dynamic_cast<ReturnStatement *>(n)}) {
    auto val{eval(std::move(e->value), std::move(env))};
    if (is_error(val.get())) {
      return val;
    }
    return return_value(std::move(val));
  } else if (auto *e{dynamic_cast<ExpressionStatement *>(n)}) {
    return eval(std::move(e->value), std::move(env));
  } else if (auto *e{dynamic_cast<LetStatement *>(n)}) {
    auto val{eval(std::move(e->value), env)};
    if (is_error(val.get())) {
      return val;
    }
    return env->set(e->identifier.value, std::move(val));
  } else if (auto *b{dynamic_cast<BlockStatement *>(n)}) {
    return eval_block_statement(std::move(b->statements), std::move(env));
  } else if (auto *i{dynamic_cast<IfExpression *>(n)}) {
    return eval_if_expression(std::move(i->condition),
                              std::move(i->consequence),
                              std::move(i->alternative), std::move(env));
  } else if (auto *e{dynamic_cast<PrefixExpression *>(n)}) {
    auto val{eval(std::move(e->right), env)};
    if (is_error(val.get())) {
      return val;
    }
    return eval_prefix_expression(Token{e->oper}, std::move(val));
  } else if (auto *e{dynamic_cast<CallExpression *>(n)}) {
    auto val{eval(std::move(e->function), env)};
    if (is_error(val.get())) {
      return val;
    }
    auto args{eval_expressions(std::move(e->arguments), std::move(env))};
    if (args.size() == 1 && is_error(args[0].get())) {
      return args[0];
    }
    return apply_function(std::move(val), std::move(args));
  } else if (auto *e{dynamic_cast<InfixExpression *>(n)}) {
    auto left{eval(std::move(e->left), env)};
    if (is_error(left.get())) {
      return left;
    }
    auto right{eval(std::move(e->right), std::move(env))};
    if (is_error(right.get())) {
      return right;
    }
    return eval_infix_expression(std::move(left), Token{e->oper},
                                 std::move(right));
  } else if (auto *f{dynamic_cast<FunctionLiteral *>(n)}) {
    return function(std::move(f->params), std::move(f->body), std::move(env));
  } else if (auto *e{dynamic_cast<IntegerLiteral *>(n)}) {
    return integer(e->value);
  } else if (auto *b{dynamic_cast<BooleanLiteral *>(n)}) {
    return boolean(b->value);
  } else if (auto *b{dynamic_cast<Identifier *>(n)}) {
    return eval_identifier(b->value, std::move(env));
  } else {
    return nullptr;
  }
}

// vim:foldmethod=marker
