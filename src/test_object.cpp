#include "test.hpp"

#include "object/object.hpp"

bool test_string_hash_key();

int main() {
  bool pass{true};
  TEST(test_string_hash_key, pass);
  return pass ? 0 : 1;
}

bool test_string_hash_key() {
  String hello_1{"Hello World"};
  String hello_2{"Hello World"};
  String diff_1{"My name is johnny"};
  String diff_2{"My name is johnny"};

  if ((hello_1.hash_key() != hello_2.hash_key()) ||
      diff_1.hash_key() != diff_2.hash_key()) {
    std::cout << "strings with same content have different hash keys"
              << std::endl;
    return false;
  }
  if (hello_1.hash_key() == diff_1.hash_key()) {
    std::cout << "strings with different content have same hash keys"
              << std::endl;
    return false;
  }

  return true;
};
