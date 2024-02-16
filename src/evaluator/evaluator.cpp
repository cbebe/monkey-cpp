#include "evaluator.hpp"
#include "builtins.hpp"
#include <functional>

std::shared_ptr<Null> null() { return std::make_unique<Null>(_NULL); }
std::shared_ptr<Integer> integer(IntType value) {
  return std::make_unique<Integer>(value);
}
std::shared_ptr<Boolean> boolean(bool value) {
  return std::make_unique<Boolean>(value ? _TRUE : _FALSE);
}
std::shared_ptr<String> string(const std::string &value) {
  return std::make_unique<String>(value);
}
std::shared_ptr<ReturnValue> return_value(std::shared_ptr<Object> value) {
  return std::make_unique<ReturnValue>(value);
}
std::shared_ptr<Error> error(std::string message) {
  return std::make_unique<Error>(message);
}
std::shared_ptr<Function> function(std::vector<Identifier> params,
                                   std::shared_ptr<BlockStatement> body,
                                   std::shared_ptr<Environment> env) {
  return std::make_unique<Function>(std::move(params), std::move(body), env);
}
std::shared_ptr<Array> array(std::vector<std::shared_ptr<Object>> elements) {
  return std::make_unique<Array>(elements);
}
std::shared_ptr<Hash> hash(std::unordered_map<HashKey, HashPair> pairs) {
  return std::make_unique<Hash>(pairs);
}

std::shared_ptr<Error> unknown_infix(ObjectType left,
                                     token_types::TokenVariant oper,
                                     ObjectType right) {
  return error("unknown operator: " + std::to_string(left) + " " +
               literal_string(oper) + " " + std::to_string(right));
}

std::shared_ptr<Error> unknown_prefix(token_types::TokenVariant oper,
                                      ObjectType right) {
  return error("unknown operator: " + literal_string(oper) +
               std::to_string(right));
}

std::shared_ptr<Error> unusable_hash(ObjectType key) {
  return error("unusable as hash key: " + std::to_string(key));
}

bool is_truthy(Object *obj) {
  switch (obj->type()) {
  case ObjectType::INTEGER_OBJ:
    return static_cast<Integer *>(obj)->value;
  case ObjectType::BOOLEAN_OBJ:
    return static_cast<Boolean *>(obj)->value;
  case ObjectType::NULL_OBJ:
    return false;
  default:
    return true;
  }
}

bool is_error(Object *obj) {
  if (obj) {
    return obj->type() == ObjectType::ERROR_OBJ;
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
  if (obj->type() == ObjectType::INTEGER_OBJ) {
    return integer(-(static_cast<Integer *>(obj)->value));
  } else {
    return unknown_prefix(token_types::Minus{}, obj->type());
  }
}

std::shared_ptr<Object> eval_prefix_expression(token_types::TokenVariant oper,
                                               std::shared_ptr<Object> right) {
  std::shared_ptr<Object> ret_val{};
  if (is_type<token_types::Bang>(oper)) {
    ret_val = eval_bang_operator_expression(right);
  } else if (is_type<token_types::Minus>(oper)) {
    ret_val = eval_minus_operator_expression(right);
  } else {
    ret_val = unknown_prefix(oper, right->type());
  }

  return ret_val;
}

std::shared_ptr<Object>
eval_integer_infix_expression(std::shared_ptr<Object> left,
                              const token_types::TokenVariant &oper,
                              std::shared_ptr<Object> right) {
  using namespace token_types;
  auto lhs{static_cast<Integer *>(left.get())->value};
  auto rhs{static_cast<Integer *>(right.get())->value};
  if (is_type<Plus>(oper)) {
    return integer(lhs + rhs);
  } else if (is_type<Minus>(oper)) {
    return integer(lhs - rhs);
  } else if (is_type<Asterisk>(oper)) {
    return integer(lhs * rhs);
  } else if (is_type<Slash>(oper)) {
    return integer(lhs / rhs);
  } else if (is_type<LT>(oper)) {
    return boolean(lhs < rhs);
  } else if (is_type<GT>(oper)) {
    return boolean(lhs > rhs);
  } else if (is_type<Eq>(oper)) {
    return boolean(lhs == rhs);
  } else if (is_type<NotEq>(oper)) {
    return boolean(lhs != rhs);
  } else {
    return unknown_infix(left->type(), oper, right->type());
  }
}

std::shared_ptr<Object>
eval_boolean_infix_expression(std::shared_ptr<Object> left,
                              const token_types::TokenVariant &oper,
                              std::shared_ptr<Object> right) {
  auto lhs{static_cast<Boolean *>(left.get())->value};
  auto rhs{static_cast<Boolean *>(right.get())->value};
  if (is_type<token_types::Eq>(oper)) {
    return boolean(lhs == rhs);
  } else if (is_type<token_types::NotEq>(oper)) {
    return boolean(lhs != rhs);
  } else {
    return unknown_infix(left->type(), oper, right->type());
  }
}

std::shared_ptr<Object>
eval_string_infix_expression(std::shared_ptr<Object> left,
                             const token_types::TokenVariant &oper,
                             std::shared_ptr<Object> right) {
  auto lhs{static_cast<String *>(left.get())->value};
  auto rhs{static_cast<String *>(right.get())->value};
  if (is_type<token_types::Plus>(oper)) {
    return string(lhs + rhs);
  } else {
    return unknown_infix(left->type(), oper, right->type());
  }
}

std::shared_ptr<Object>
eval_infix_expression(std::shared_ptr<Object> left,
                      const token_types::TokenVariant &oper,
                      std::shared_ptr<Object> right) {
  if (left->type() == ObjectType::INTEGER_OBJ &&
      right->type() == ObjectType::INTEGER_OBJ) {
    return eval_integer_infix_expression(left, oper, right);
  } else if (left->type() == ObjectType::BOOLEAN_OBJ &&
             right->type() == ObjectType::BOOLEAN_OBJ) {
    return eval_boolean_infix_expression(left, oper, right);
  } else if (left->type() == ObjectType::STRING_OBJ &&
             right->type() == ObjectType::STRING_OBJ) {
    return eval_string_infix_expression(left, oper, right);
  } else if (left->type() != right->type()) {
    return error("type mismatch: " + std::to_string(left->type()) + " " +
                 literal_string(oper) + " " + std::to_string(right->type()));
  } else {
    return unknown_infix(left->type(), oper, right->type());
  }
}

std::shared_ptr<Object>
eval_array_index_expression(std::shared_ptr<Object> array,
                            std::shared_ptr<Object> index) {
  auto arr{static_cast<Array *>(array.get())->elements};
  auto max_idx{(IntType)arr.size() - 1};
  auto idx{static_cast<Integer *>(index.get())->value};

  if (idx < 0 || idx > max_idx) {
    return null();
  }

  return arr[idx];
}

std::shared_ptr<Object>
eval_hash_index_expression(std::shared_ptr<Object> hash,
                           std::shared_ptr<Object> index) {
  auto pairs{static_cast<Hash *>(hash.get())->pairs};
  auto key{std::dynamic_pointer_cast<Hashable>(index)};
  if (!key) {
    return unusable_hash(index->type());
  }

  auto hash_key{key->hash_key()};
  if (!pairs.contains(hash_key)) {
    return null();
  }

  return pairs[hash_key].value;
}

std::shared_ptr<Object> eval_index_expression(std::shared_ptr<Object> left,
                                              std::shared_ptr<Object> index) {
  if (left->type() == ObjectType::ARRAY_OBJ &&
      index->type() == ObjectType::INTEGER_OBJ) {
    return eval_array_index_expression(left, index);
  } else if (left->type() == ObjectType::HASH_OBJ) {
    return eval_hash_index_expression(left, index);
  } else {
    return error("index operator not supported: " +
                 std::to_string(left->type()));
  }
}

std::shared_ptr<Object>
eval_if_expression(std::shared_ptr<Expression> condition,
                   std::shared_ptr<BlockStatement> consequence,
                   std::shared_ptr<BlockStatement> alternative,
                   std::shared_ptr<Environment> env) {
  auto cond{eval(condition, env)};
  if (is_error(cond.get())) {
    return cond;
  }
  if (is_truthy(cond.get())) {
    return eval(consequence, env);
  } else if (alternative != nullptr) {
    return eval(alternative, env);
  } else {
    return null();
  }
}

std::shared_ptr<Object>
eval_program(std::vector<std::shared_ptr<Statement>> statements,
             std::shared_ptr<Environment> env) {
  std::shared_ptr<Object> result{};
  for (auto &s : statements) {
    result = eval(s, env);
    if (dynamic_cast<Error *>(result.get())) {
      return result;
    } else if (auto *ret_val{dynamic_cast<ReturnValue *>(result.get())}) {
      return ret_val->value;
    }
  }
  return result;
}

std::shared_ptr<Object>
eval_block_statement(std::vector<std::shared_ptr<Statement>> statements,
                     std::shared_ptr<Environment> env) {
  std::shared_ptr<Object> result{};
  for (auto &s : statements) {
    result = eval(s, env);
    if (result) {
      auto type{result->type()};
      if (type == ObjectType::RETURN_VALUE_OBJ ||
          type == ObjectType::ERROR_OBJ) {
        return result;
      }
    }
  }
  return result;
}

std::shared_ptr<Object> eval_identifier(const std::string &name,
                                        std::shared_ptr<Environment> env) {
  auto val{env->get(name)};
  if (val) {
    return val;
  }
  if (builtins.contains(name)) {
    return builtins[name];
  }
  return error("identifier not found: " + name);
}
std::vector<std::shared_ptr<Object>>
eval_expressions(std::vector<std::shared_ptr<Expression>> expressions,
                 std::shared_ptr<Environment> env) {
  std::vector<std::shared_ptr<Object>> args{};
  for (size_t i = 0; i < expressions.size(); i++) {
    auto evaluated{eval(expressions[i], env)};
    if (is_error(evaluated.get())) {
      return std::vector<std::shared_ptr<Object>>{evaluated};
    }
    args.push_back(evaluated);
  }
  return args;
}

std::shared_ptr<Environment>
extend_function_env(std::shared_ptr<Function> func,
                    std::vector<std::shared_ptr<Object>> args) {
  auto extended{std::make_shared<Environment>(func->env)};
  for (size_t i = 0; i < args.size(); i++) {
    extended->set(func->params[i].value, args[i]);
  }
  return extended;
}

std::shared_ptr<Object> unwrap_return_value(std::shared_ptr<Object> obj) {
  if (obj->type() == ObjectType::RETURN_VALUE_OBJ) {
    return std::static_pointer_cast<ReturnValue>(obj)->value;
  }
  return obj;
}

std::shared_ptr<Object>
apply_function(std::shared_ptr<Object> obj,
               std::vector<std::shared_ptr<Object>> args) {
  if (obj->type() == ObjectType::FUNCTION_OBJ) {
    auto function{std::static_pointer_cast<Function>(obj)};
    auto extended_env{extend_function_env(function, args)};
    auto evaluated{
        eval_block_statement(function->body->statements, extended_env)};
    return unwrap_return_value(evaluated);
  } else if (obj->type() == ObjectType::BUILTIN_OBJ) {
    auto function{std::static_pointer_cast<Builtin>(obj)};
    return function->fn(args);
  }

  return error("not a function: " + std::to_string(obj->type()));
}

std::shared_ptr<Object> eval_hash_literal(
    std::unordered_map<std::shared_ptr<Expression>, std::shared_ptr<Expression>>
        node_pairs,
    std::shared_ptr<Environment> env) {
  std::unordered_map<HashKey, HashPair> pairs{};
  for (const auto &k : node_pairs) {
    auto key{eval(k.first, env)};
    if (is_error(key.get())) {
      return key;
    }
    if (auto *hash_key{dynamic_cast<Hashable *>(key.get())}) {
      auto value{eval(k.second, env)};
      if (is_error(value.get())) {
        return value;
      }
      pairs[hash_key->hash_key()] = HashPair{key, value};
    } else {
      return unusable_hash(key->type());
    }
  }

  return hash(pairs);
}

std::shared_ptr<Object> eval(std::shared_ptr<Node> node,
                             std::shared_ptr<Environment> env) {
  Node *n = node.get();
  if (auto *p = dynamic_cast<Program *>(n)) {
    return eval_program(p->statements, env);
  } else if (auto *e{dynamic_cast<ReturnStatement *>(n)}) {
    auto val{eval(e->value, env)};
    if (is_error(val.get())) {
      return val;
    }
    return return_value(val);
  } else if (auto *e{dynamic_cast<ExpressionStatement *>(n)}) {
    return eval(e->value, env);
  } else if (auto *e{dynamic_cast<LetStatement *>(n)}) {
    auto val{eval(e->value, env)};
    if (is_error(val.get())) {
      return val;
    }
    return env->set(std::move(e->identifier.value), std::move(val));
  } else if (auto *b{dynamic_cast<BlockStatement *>(n)}) {
    return eval_block_statement(b->statements, env);
  } else if (auto *i{dynamic_cast<IfExpression *>(n)}) {
    return eval_if_expression(i->condition, i->consequence, i->alternative,
                              env);
  } else if (auto *e{dynamic_cast<PrefixExpression *>(n)}) {
    auto val{eval(e->right, env)};
    if (is_error(val.get())) {
      return val;
    }
    return eval_prefix_expression(e->oper, val);
  } else if (auto *e{dynamic_cast<IndexExpression *>(n)}) {
    auto left{eval(e->left, env)};
    if (is_error(left.get())) {
      return left;
    }
    auto index{eval(e->index, env)};
    if (is_error(index.get())) {
      return index;
    }
    return eval_index_expression(left, index);
  } else if (auto *a{dynamic_cast<ArrayLiteral *>(n)}) {
    auto elements{eval_expressions(a->elements, env)};
    if (elements.size() == 1 && is_error(elements[0].get())) {
      return elements[0];
    }
    return array(elements);
  } else if (auto *a{dynamic_cast<HashLiteral *>(n)}) {
    return eval_hash_literal(a->pairs, env);
  } else if (auto *e{dynamic_cast<CallExpression *>(n)}) {
    auto val{eval(e->function, env)};
    if (is_error(val.get())) {
      return val;
    }
    auto args{eval_expressions(e->arguments, env)};
    if (args.size() == 1 && is_error(args[0].get())) {
      return args[0];
    }
    return apply_function(val, args);
  } else if (auto *e{dynamic_cast<InfixExpression *>(n)}) {
    auto left{eval(e->left, env)};
    if (is_error(left.get())) {
      return left;
    }
    auto right{eval(e->right, env)};
    if (is_error(right.get())) {
      return right;
    }
    return eval_infix_expression(left, e->oper, right);
  } else if (auto *f{dynamic_cast<FunctionLiteral *>(n)}) {
    return function(std::move(f->params), std::move(f->body), env);
  } else if (auto *e{dynamic_cast<IntegerLiteral *>(n)}) {
    return integer(e->value);
  } else if (auto *b{dynamic_cast<BooleanLiteral *>(n)}) {
    return boolean(b->value);
  } else if (auto *s{dynamic_cast<StringLiteral *>(n)}) {
    return string(s->value);
  } else if (auto *b{dynamic_cast<Identifier *>(n)}) {
    return eval_identifier(b->value, env);
  } else {
    return nullptr;
  }
}

// vim:foldmethod=marker
