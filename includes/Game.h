#include "./ConfigReader.h"
#include "./Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>

class Game {
private:
  sf::RenderWindow        m_window;
  EntityManager           m_entities;
  sf::Font                m_font;
  sf::Text                m_text;
  ConfigReader            m_configReader;
  int                     m_score              = 0;
  int                     m_currentFrame       = 0;
  int                     m_lastEnemySpawnTime = 0;
  bool                    m_paused             = false;
  bool                    m_running            = true;
  std::shared_ptr<Entity> m_player;
  int                     m_lives = 5;

  void init(const std::string &config);
  void setPaused(bool paused);
  // systems
  void sMovement();
  void sUserInput();
  void sLifespan();
  void sRender();
  void sEnemySpawner();
  void sCollision();
  // helpers
  void spawnPlayer();
  void spawnEnemy();
  void spawnSmallEnemies(std::shared_ptr<Entity> entity);
  void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);
  void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:
  Game(const std::string &config);
  void run();
};
