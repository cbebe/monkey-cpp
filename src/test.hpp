#include <iostream>

// Love me some non-syntactic macros
#define TEST(fn, res)                                                          \
  do {                                                                         \
    std::cout << #fn << " ";                                                   \
    bool result{fn()};                                                         \
    std::cout << (result ? "PASS" : "FAIL") << std::endl;                      \
    res &= result;                                                             \
  } while (0)
