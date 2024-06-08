#pragma once
#ifndef COLLISION_HELPERS_H
#define COLLISION_HELPERS_H

#include "./Entity.h"
#include "./EntityManager.h"
#include <bitset>
#include <iostream>

namespace CollisionHelpers {

  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const sf::Vector2u            &window_size);

  void enforcePlayerBounds(const std::shared_ptr<Entity> &entity,
                           const std::bitset<4>          &collides,
                           const sf::Vector2u            &window_size);

  void enforceEnemyBounds(const std::shared_ptr<Entity> &entity,
                          const std::bitset<4>          &collides,
                          const sf::Vector2u            &window_size);

  void enforceBulletBounds(const std::shared_ptr<Entity> &entity,
                           const std::bitset<4>          &collides);

  bool calculateCollisionBetweenEntities(const std::shared_ptr<Entity> &entityA,
                                         const std::shared_ptr<Entity> &entityB);

} // namespace CollisionHelpers

#endif // COLLISION_HELPERS_H
