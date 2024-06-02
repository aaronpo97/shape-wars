#ifndef TEST_SUITE_CPP
#define TEST_SUITE_CPP

#include <functional>
#include <iostream>

namespace TEST {
  void assertTrue(bool               condition,
                  const std::string &success_message = "Pass",
                  const std::string &error_message   = "Fail") {
    if (!condition) {
      std::cerr << "- " << error_message << std::endl << std::endl;
      throw std::runtime_error(error_message);
    }

    std::cout << "- " << success_message << std::endl << std::endl;
  }

  void when(const std::string &description, const std::function<void()> &fn) {
    std::cout << description << std::endl;
    fn();
  }
} // namespace TEST

#endif // TEST_SUITE_CPP