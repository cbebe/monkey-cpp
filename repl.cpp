#include "lexer.hpp"
#include <iostream>
#include <variant>
#include <vector>

const std::vector<std::string> PROMPTS = {
    "⊂((•‿̮• ))⊃ >", "⊂((・⊥・))⊃",     "⊂((*＞⊥σ))⊃",
    "⊂((σ⊥σ))⊃",    "⊂((〃￣⊥￣〃))⊃", "⊂((∂⊥<*))⊃",
    "⊂((＞⊥＜))⊃",  "⊂((≧⊥≦))⊃",       "⊂((δ⊥δ))⊃"};

void print_prompt() { std::cout << PROMPTS[rand() % 9] << " >> "; }
int main() {
  srand(time(0));
  print_prompt();
  for (std::string line; std::getline(std::cin, line);) {
    Lexer l{line};
    for (auto t = l.next_token(); t.to_string() != "EOF"; t = l.next_token()) {
      std::cout << t.to_string() << std::endl;
    }
    print_prompt();
  }
  std::cout << std::endl;
  return 0;
}
