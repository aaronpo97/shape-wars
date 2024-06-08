#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H
#include <cmath>
namespace MathHelpers {
  inline const float pi = atan(1) * 4;

  inline float degreesToRadians(const float degrees) {
    const float radians = degrees * pi / 180;
    return radians;
  }

  inline float radiansToDegrees(const float radians) {
    const float degrees = radians * 180 / pi;
    return degrees;
  }

  inline float pythagoras(const float a, const float b) {
    const float c = sqrt(a * a + b * b);
    return c;
  }
}; // namespace MathHelpers

#endif // MATH_HELPERS_H