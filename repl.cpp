#include "src/lexer.hpp"
#include "src/parser.hpp"
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

const std::vector<std::string_view> PROMPTS{
    "⊂((•‿̮• ))⊃ ", "⊂((・⊥・))⊃", "⊂((*＞⊥σ))⊃", " ⊂((σ⊥σ))⊃ ",
    "⊂((∂⊥<*))⊃ ", "⊂((＞⊥＜))⊃", " ⊂((≧⊥≦))⊃ ", " ⊂((δ⊥δ))⊃ "};

void print_prompt() { std::cout << PROMPTS[rand() % PROMPTS.size()] << " >> "; }

enum Command {
  Lex = 0,
  Parse,
};

void lex() {
  print_prompt();
  for (std::string line; std::getline(std::cin, line);) {
    Lexer l{line};
    for (auto t{l.next_token()}; !t.is_type<token_types::Eof>();
         t = l.next_token()) {
      std::cout << t.to_string() << std::endl;
    }
    print_prompt();
  }
}

void parse() {
  std::stringstream ss;
  ss << std::cin.rdbuf();
  Parser parser{Lexer{ss.str()}};
  auto program{parser.parse_program()};
  if (!program) {
    for (auto &e : parser.errors) {
      std::cout << e << std::endl;
    }
  } else {
    std::cout << program->to_string() << std::endl;
  }
}

int main(int argc, const char **argv) {
  srand(time(0));
  Command com{};
  if (argc > 1) {
    std::string command{argv[1]};
    if (command == "lex") {
      com = Lex;
    } else {
      com = Parse;
    }
  }
  if (com == Lex) {
    lex();
  } else {
    parse();
  }
  std::cout << std::endl;
  return 0;
}
