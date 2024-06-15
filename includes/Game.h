#include "./ConfigReader.h"
#include "./Entity.h"
#include "EntityManager.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
class Game {
private:
  sf::RenderWindow m_window;
  EntityManager    m_entities;
  sf::Font         m_font;
  sf::Text         m_text;
  ConfigReader     m_configReader;
  int              m_score                 = 0;
  int              m_currentFrame          = 0;
  int              m_lastEnemySpawnTime    = 0;
  bool             m_paused                = false;
  bool             m_running               = true;
  int              m_lives                 = 5;
  int              m_specialWeaponCounter  = 0;
  int              m_maxSpecialWeaponUsage = 30;
  int              m_specialWeaponCooldown = 70;

  std::shared_ptr<Entity> m_player;

  enum class SoundType { SHOOT, ENEMY_SHOOT, SPECIAL_WEAPON, PLAYER_HIT };

  sf::SoundBuffer shootBuffer;
  sf::Sound       shootSound;
  sf::SoundBuffer enemyHit;
  sf::Sound       enemyShootSound;
  sf::SoundBuffer specialWeaponBuffer;
  sf::Sound       specialWeaponSound;
  sf::SoundBuffer playerHitBuffer;
  sf::Sound       playerHitSound;
  void            playSound(SoundType soundType);

  void init(const std::string &config);
  void setPaused(bool paused);
  // systems
  void sMovement();
  void sUserInput();
  void sLifespan();
  void sRender();
  void sSpawner();
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
