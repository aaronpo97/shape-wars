#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "Entity.h"
#include <memory>

// store all entity objects in a vector

// define a type alias for a vector of shared pointers to Entity objects
typedef std::vector<std::shared_ptr<Entity>> EntityVector;

// also store separate vectors of Entity objects by their tag for quick retrieval

// define a type alias for a map of strings to vectors of shared pointers to Entity
// objects

// example usage - "player", vector of all the player entities
typedef std::map<std::string, EntityVector> EntityMap;

class EntityManager {
  EntityVector m_entities;
  EntityVector m_toAdd;
  EntityMap    m_entityMap;
  /*
   *    `m_totalEntries` is not the amount of entities that are alive, but rather the
   *    total amount of entities that are created.
   */
  size_t m_totalEntries = 0;

public:
  EntityManager();
  void                    update();
  std::shared_ptr<Entity> addEntity(const std::string &tag);
  EntityVector           &getEntities();
  EntityVector           &getEntities(const std::string &tag);
};

#endif // ENTITY_MANAGER_H