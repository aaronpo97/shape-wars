#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform {
public:
  Vec2  pos      = {0.0, 0.0};
  Vec2  velocity = {0.0, 0.0};
  float angle    = 0;

  CTransform(const Vec2 &p, const Vec2 &v, const float a) :
      pos(p), velocity(v), angle(a) {}
};

class CShape {
public:
  sf::CircleShape circle;

  CShape(const float      radius,
         const int        points,
         const sf::Color &fill,
         const sf::Color &outline,
         const float      thickness) :
      circle(radius, points) {
    circle.setFillColor(fill);
    circle.setOutlineColor(outline);
    circle.setOutlineThickness(thickness);
    circle.setOrigin(radius, radius);
  }
};

class CCollision {
public:
  float radius = 0;
  CCollision(const float r) :
      radius(r) {}
};

class CScore {
public:
  int score = 0;
  CScore(const int s) :
      score(s) {}
};

class CLifespan {
public:
  int       remaining = 0;   // amount of lifespan remaining on the entity
  const int total     = 100; // the total initial lifespan of the entity

  CLifespan(const int t) :
      total(t), remaining(t) {}
};

class CInput {
public:
  bool up    = false;
  bool down  = false;
  bool left  = false;
  bool right = false;
  bool shoot = false;

  CInput() {}
};
#endif // COMPONENTS_H