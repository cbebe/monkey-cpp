#pragma once
#include "../ast/ast.hpp"
#include "../object/object.hpp"

std::unique_ptr<Object> eval(std::unique_ptr<Node>);
