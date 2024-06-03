#include "../includes/Entity.h"

bool Entity::isActive() const {
  return m_active;
}

const std::string &Entity::tag() const {
  return m_tag;
}

size_t Entity::id() const {
  return size_t();
}

void Entity::destroy() {
  m_active = false;
}
