#pragma once
#include "../ast/ast.hpp"
#include "../object/environment.hpp"
#include "../object/object.hpp"

std::shared_ptr<Object> eval(std::unique_ptr<Node>,
                             std::shared_ptr<Environment>);
