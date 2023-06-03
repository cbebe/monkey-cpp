#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

const std::vector<std::string_view> PROMPTS{
    "⊂((•‿̮• ))⊃ ", "⊂((・⊥・))⊃", "⊂((*＞⊥σ))⊃", " ⊂((σ⊥σ))⊃ ",
    "⊂((∂⊥<*))⊃ ", "⊂((＞⊥＜))⊃", " ⊂((≧⊥≦))⊃ ", " ⊂((δ⊥δ))⊃ "};

void print_prompt() { std::cout << PROMPTS[rand() % PROMPTS.size()] << " >> "; }

const std::string_view MONKEY_FACE = R"(            __,__
   .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ 0 | 0 /  /   / |
  \ '- ,\.-"""""""-./, -' /
   ''-' /_   ^ ^   _\ '-''
       |  \._   _./  |
       \   \ '~' /   /
        '._ '-=-' _.'
           '-----'
  )";

void parse() {
  print_prompt();
  for (std::string line; std::getline(std::cin, line);) {
    Parser parser{Lexer{line}};
    auto program{parser.parse_program()};
    if (parser.errors.size() > 0) {
      std::cout << MONKEY_FACE;
      std::cout << "Woops! We can into some monkey business here!" << std::endl;
      std::cout << "parser errors:" << std::endl;
      for (auto &e : parser.errors) {
        std::cout << '\t' << e << std::endl;
      }
    } else {
      std::cout << program->to_string() << std::endl;
    }
    print_prompt();
  }
}

int main() {
  srand(time(0));
  parse();
  std::cout << std::endl;
  return 0;
}
