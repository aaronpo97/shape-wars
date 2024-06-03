
#include "../includes/EntityManager.h"
#include "../includes/Entity.h"

#include <iostream>

EntityManager::EntityManager() {}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {

  auto e = std::shared_ptr<Entity>(new Entity(m_totalEntries++, tag));

  // add it to the vector of all entities
  m_entities.push_back(e);

  // add it to the map of tag->entity_vector, if the tag doesn't exist, it will create one
  m_entityMap[tag].push_back(e);

  return e;
}

EntityVector &EntityManager::getEntities() {
  return m_entities;
}
EntityVector &EntityManager::getEntities(const std::string &tag) {
  return m_entityMap[tag];
}

void EntityManager::update() {
  // add all entities in the `m_toAdd` vector to the main entity vector
  for (auto &e : m_toAdd) {
    m_entities.push_back(e);
    m_entityMap[e->tag()].push_back(e);
  }

  for (auto e : m_entities) {
    if (!e->isActive()) {
      m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e), m_entities.end());
      m_entityMap[e->tag()].erase(
          std::remove(m_entityMap[e->tag()].begin(), m_entityMap[e->tag()].end(), e),
          m_entityMap[e->tag()].end());
    }
  }
}
