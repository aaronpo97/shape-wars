#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H
#include <cmath>
namespace MathHelpers {
  const float pi = atan(1) * 4;

  float degreesToRadians(const float degrees) {
    const float radians = degrees * pi / 180;
    return radians;
  }

  float radiansToDegrees(const float radians) {
    const float degrees = radians * 180 / pi;
    return degrees;
  }

  float pythagoras(const float a, const float b) {
    const float c = sqrt(a * a + b * b);
    return c;
  }
}; // namespace MathHelpers

#endif // MATHHELPERS_H