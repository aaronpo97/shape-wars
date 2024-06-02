#ifndef VEC2_TEST_H
#define VEC2_TEST_H

#include <iostream>

class Vec2Test {
private:
  void test_equals();
  void test_not_equals();
  void test_add();
  void test_subtract();
  void test_multiply();
  void test_divide();
  void test_length();
  void test_normalize();
  void test_increment_assignment();
  void test_decrement_assignment();
  void test_multiply_assignment();
  void test_divide_assignment();
  void test_all();

public:
  Vec2Test() {}

  void test();
};

#endif // VEC2_TEST_H