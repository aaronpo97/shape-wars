
#include "../includes/CollisionHelpers.h"
#include "../includes/MathHelpers.h"
#include <bitset>

namespace CollisionHelpers {

  enum Boundaries { TOP, BOTTOM, LEFT, RIGHT };
  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const sf::Vector2u            &window_size) {
    const Vec2 &pos    = entity->cTransform->pos;
    const float radius = entity->cShape->circle.getRadius();

    std::bitset<4> collidesWithBoundary;
    collidesWithBoundary.set(TOP, pos.y - radius < 0);
    collidesWithBoundary.set(BOTTOM, pos.y + radius > window_size.y);
    collidesWithBoundary.set(LEFT, pos.x - radius < 0);
    collidesWithBoundary.set(RIGHT, pos.x + radius > window_size.x);

    return collidesWithBoundary;
  }

  void enforcePlayerBounds(const std::shared_ptr<Entity> &entity,
                           const std::bitset<4>          &collides,
                           const sf::Vector2u            &window_size) {

    const float radius = entity->cShape->circle.getRadius();
    if (collides[TOP]) {
      entity->cTransform->pos.y = radius;
    }
    if (collides[BOTTOM]) {
      entity->cTransform->pos.y = window_size.y - radius;
    }
    if (collides[LEFT]) {
      entity->cTransform->pos.x = radius;
    }
    if (collides[RIGHT]) {
      entity->cTransform->pos.x = window_size.x - radius;
    }
  }

  void enforceEnemyBounds(const std::shared_ptr<Entity> &entity,
                          const std::bitset<4>          &collides,
                          const sf::Vector2u            &window_size) {

    const float radius = entity->cShape->circle.getRadius();
    if (collides[TOP]) {
      entity->cTransform->pos.y = radius;
      entity->cTransform->velocity.y *= -1;
    }
    if (collides[BOTTOM]) {
      entity->cTransform->pos.y = window_size.y - radius;
      entity->cTransform->velocity.y *= -1;
    }
    if (collides[LEFT]) {
      entity->cTransform->pos.x = radius;
      entity->cTransform->velocity.x *= -1;
    }
    if (collides[RIGHT]) {
      entity->cTransform->pos.x = window_size.x - radius;
      entity->cTransform->velocity.x *= -1;
    }
  }

  void enforceBulletBounds(const std::shared_ptr<Entity> &entity, const std::bitset<4> &collides) {
    if (collides.any()) {

      entity->destroy();
    }
  }

  bool calculateCollisionBetweenEntities(const std::shared_ptr<Entity> &entityA,
                                         const std::shared_ptr<Entity> &entityB) {
    const Vec2 &centerPositionA = entityA->cTransform->pos;
    const float radiusA         = entityA->cShape->circle.getRadius();

    const Vec2 &centerPositionB = entityB->cTransform->pos;
    const float radiusB         = entityB->cShape->circle.getRadius();

    const float dx = centerPositionB.x - centerPositionA.x;
    const float dy = centerPositionB.y - centerPositionA.y;

    /*
    * Calculate the distance between the two entities using the Pythagorean theorem.

    * The distance between two vectors can be visualized as the hypotenuse of a right triangle where `dx`
    * and `dy` are the two sides of the triangle.
    */
    const float dv = MathHelpers::pythagoras(dx, dy);

    /*
     * If the distance between the two entities is less than or equal to the sum of their radii, they are
     * colliding.
     */
    const bool collides = dv < (radiusA + radiusB);
    return collides;
  }

} // namespace CollisionHelpers