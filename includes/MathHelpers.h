#ifndef MATHHELPERS_H
#define MATHHELPERS_H
#include <cmath> // Include cmath for mathematical functions
namespace MathHelpers {
  /*
   * atan(1) = π/4
   */
  const float pi = atan(1) * 4;
  /*
   * Convert degrees to radians
   */
  float degreesToRadians(const float degrees) {
    const float radians = degrees * pi / 180;
    return radians;
  }

  /*
   * Convert radians to degrees
   */
  float radiansToDegrees(const float radians) {
    const float degrees = radians * 180 / pi;
    return degrees;
  }

}; // namespace MathHelpers

#endif // MATHHELPERS_H