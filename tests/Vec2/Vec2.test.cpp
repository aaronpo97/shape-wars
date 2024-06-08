#include "./Vec2.test.h"
#include "../../includes/Vec2.h"
#include "../TestSuite.h"

void Vec2Test::test_equals() {
  std::cout << "Vec2::operator== test" << std::endl << std::endl << std::endl;
  TEST::when("If two vectors are not equal, should return false", []() {
    Vec2 vec1(1, 2);
    Vec2 vec2(1, 3);

    bool result = vec1 == vec2;
    TEST::assertTrue(!result);
  });

  TEST::when("If two vectors are equal, should return true", []() {
    Vec2 vec1(1, 2);
    Vec2 vec2(1, 2);

    bool result = vec1 == vec2;
    TEST::assertTrue(result);
  });

  std::cout << "Vec2::operator== passed" << std::endl;
}

void Vec2Test::test_not_equals() {
  std::cout << "Vec2::operator!= test" << std::endl << std::endl << std::endl;
  TEST::when("If two vectors are not equal, should return true", []() {
    Vec2 vec1(1, 2);
    Vec2 vec2(1, 3);

    bool result = vec1 != vec2;
    TEST::assertTrue(result);
  });

  TEST::when("If two vectors are equal, should return false", []() {
    Vec2 vec1(1, 2);
    Vec2 vec2(1, 2);

    bool result = vec1 != vec2;
    TEST::assertTrue(!result);
  });

  std::cout << "Vec2::operator!= passed" << std::endl;
}

void Vec2Test::test_add() {
  std::cout << "Vec2::operator+ test" << std::endl << std::endl << std::endl;
  TEST::when("Adding two vectors, it returns a new vector with the correct sum", []() {
    Vec2 vec1(1, 2);
    Vec2 vec2(3, 4);

    Vec2 result = vec1 + vec2;
    TEST::assertTrue(result.x == 4 && result.y == 6);
  });

  std::cout << "Vec2::operator+ passed" << std::endl;
}

void Vec2Test::test_subtract() {
  std::cout << "Vec2::operator- test" << std::endl << std::endl << std::endl;
  TEST::when(
      "Subtracting two vectors, it returns a new vector with the correct difference",
      []() {
        Vec2 vec1(3, 4);
        Vec2 vec2(1, 2);

        Vec2 result = vec1 - vec2;
        TEST::assertTrue(result.x == 2 && result.y == 2);
      });

  std::cout << "Vec2::operator- passed" << std::endl;
}

void Vec2Test::test_multiply() {
  std::cout << "Vec2::operator* test" << std::endl << std::endl << std::endl;
  TEST::when(
      "Multiplying a vector by a scalar, it returns a new vector with each component "
      "multiplied by the scalar",
      []() {
        Vec2  vec(2, 3);
        float scalar = 2;

        Vec2 result = vec * scalar;
        TEST::assertTrue(result.x == 4 && result.y == 6);
      });

  std::cout << "Vec2::operator* passed" << std::endl;
}

void Vec2Test::test_divide() {
  std::cout << "Vec2::operator/ test" << std::endl << std::endl << std::endl;
  TEST::when("Dividing a vector by a scalar, it returns a new vector with each "
             "component divided "
             "by the scalar",
             []() {
               Vec2  vec(4, 6);
               float scalar = 2;

               Vec2 result = vec / scalar;
               TEST::assertTrue(result.x == 2 && result.y == 3);
             });

  std::cout << "Vec2::operator/ passed" << std::endl;
}

void Vec2Test::test_length() {
  std::cout << "Vec2::length test" << std::endl << std::endl << std::endl;
  TEST::when("Computing the length of a vector, it returns the correct magnitude", []() {
    Vec2 vec(3, 4);

    float result = vec.length();
    TEST::assertTrue(result == 5);
  });

  std::cout << "Vec2::length passed" << std::endl;
}

void Vec2Test::test_normalize() {
  std::cout << "Vec2::normalize test" << std::endl << std::endl << std::endl;
  TEST::when("Normalizing a vector, it returns a unit vector in the same direction",
             []() {
               Vec2 vec(3, 4);

               vec.normalize();
               TEST::assertTrue(vec.length() == 1);
             });

  std::cout << "Vec2::normalize passed" << std::endl;
}

void Vec2Test::test_increment_assignment() {
  std::cout << "Vec2::operator+= test" << std::endl << std::endl << std::endl;
  TEST::when("Incrementing a vector by another vector, it returns the correct sum", []() {
    Vec2 vec1(1, 2);
    Vec2 vec2(3, 4);

    vec1 += vec2;
    TEST::assertTrue(vec1.x == 4 && vec1.y == 6);
  });

  std::cout << "Vec2::operator+= passed" << std::endl;
}

void Vec2Test::test_decrement_assignment() {
  std::cout << "Vec2::operator-= test" << std::endl << std::endl << std::endl;
  TEST::when("Decrementing a vector by another vector, it returns the correct difference",
             []() {
               Vec2 vec1(3, 4);
               Vec2 vec2(1, 2);

               vec1 -= vec2;
               TEST::assertTrue(vec1.x == 2 && vec1.y == 2);
             });

  std::cout << "Vec2::operator-= passed" << std::endl;
}

void Vec2Test::test_multiply_assignment() {
  std::cout << "Vec2::operator*= test" << std::endl << std::endl << std::endl;
  TEST::when("Multiplying a vector by a scalar, it returns the correct product", []() {
    Vec2  vec(2, 3);
    float scalar = 2;

    vec *= scalar;
    TEST::assertTrue(vec.x == 4 && vec.y == 6);
  });

  std::cout << "Vec2::operator*= passed" << std::endl;
}

void Vec2Test::test_divide_assignment() {
  std::cout << "Vec2::operator/= test" << std::endl << std::endl << std::endl;
  TEST::when("Dividing a vector by a scalar, it returns the correct quotient", []() {
    Vec2  vec(4, 6);
    float scalar = 2;

    vec /= scalar;
    TEST::assertTrue(vec.x == 2 && vec.y == 3);
  });

  std::cout << "Vec2::operator/= passed" << std::endl;
}

void Vec2Test::test_all() {
  test_equals();
  test_not_equals();
  test_add();
  test_subtract();
  test_multiply();
  test_divide();
  test_length();
  test_normalize();
  test_increment_assignment();
  test_decrement_assignment();
  test_multiply_assignment();
  test_divide_assignment();
}

void Vec2Test::test() {
  test_all();
}