#include "../includes/Game.h"
#include "../includes/Components.h"
#include "../includes/Tags.h"
#include <fstream>
#include <iostream>

Game::Game(const std::string &config) {
  init(config);
}

void Game::init(const std::string &path) {
  // TODO: read in config file
  // use the premade PlayerConfig, EnemyConfig, and BulletConfig structs
  //   std::ifstream fin(path);

  //   fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.FR >> m_playerConfig.FG >>
  //       m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >>
  //       m_playerConfig.OT >> m_playerConfig.V >> m_playerConfig.S;

  //   fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.OR >> m_enemyConfig.OG >>
  //       m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >>
  //       m_enemyConfig.L >> m_enemyConfig.SI >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;

  //   fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.FR >> m_bulletConfig.FG >>
  //       m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >>
  //       m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L >> m_bulletConfig.S;

  m_playerConfig.SR = 32;
  m_playerConfig.CR = 32;
  m_playerConfig.S  = 5;
  m_playerConfig.FR = 5;
  m_playerConfig.FG = 5;
  m_playerConfig.FB = 5;
  m_playerConfig.OR = 255;
  m_playerConfig.OG = 0;
  m_playerConfig.OB = 0;
  m_playerConfig.OT = 4;
  m_playerConfig.V  = 8;

  m_window.create(sf::VideoMode(1600, 900), "Geometry Wars");

  m_window.setFramerateLimit(60);

  spawnPlayer();
}

void Game::run() {

  // TODO add pause functionality, some systems should not run when paused, others should
  // Animation Loop
  while (m_running) {
    m_entities.update();

    if (!m_paused) {
      sEnemySpawner();
      sMovement();
      sCollision();
      sUserInput();
    }

    if (m_currentFrame - m_lastEnemySpawnTime > 120) {
      spawnEnemy();
    }

    sRender();
    m_currentFrame++;
  }
}

void Game::setPaused(bool paused) {
  // Set the game to paused or resumed
}

void Game::sMovement() {

  Vec2 playerVelocity;
  if (m_player->cInput->left) {
    playerVelocity.x -= m_playerConfig.S;
  }
  if (m_player->cInput->right) {
    playerVelocity.x += m_playerConfig.S;
  }
  if (m_player->cInput->up) {
    playerVelocity.y -= m_playerConfig.S;
  }
  if (m_player->cInput->down) {
    playerVelocity.y += m_playerConfig.S;
  }

  for (auto e : m_entities.getEntities()) {
    if (e->tag() != EntityTags::Player) {
      // Updates the position of the entities
      e->cTransform->pos += e->cTransform->velocity;
      // Rotates the entity
      e->cTransform->angle += 2.0f;
      e->cShape->circle.setRotation(e->cTransform->angle);
      return;
    }

    // Updates the position of the player
    m_player->cTransform->pos += playerVelocity;
    // Rotates the player
    e->cTransform->angle += 2.0f;
    e->cShape->circle.setRotation(e->cTransform->angle);

    return;
  }
}

void Game::sUserInput() {
  sf::Event event;
  while (m_window.pollEvent(event)) {

    const auto key_released = event.type == sf::Event::KeyReleased;
    const auto key_pressed  = event.type == sf::Event::KeyPressed;
    auto       key_code     = event.key.code;

    if (key_pressed) {
      m_player->cInput->up    = key_code == sf::Keyboard::W ? true : m_player->cInput->up;
      m_player->cInput->left  = key_code == sf::Keyboard::A ? true : m_player->cInput->left;
      m_player->cInput->down  = key_code == sf::Keyboard::S ? true : m_player->cInput->down;
      m_player->cInput->right = key_code == sf::Keyboard::D ? true : m_player->cInput->right;
    }

    if (key_released) {
      m_player->cInput->up    = key_code == sf::Keyboard::W ? false : m_player->cInput->up;
      m_player->cInput->left  = key_code == sf::Keyboard::A ? false : m_player->cInput->left;
      m_player->cInput->down  = key_code == sf::Keyboard::S ? false : m_player->cInput->down;
      m_player->cInput->right = key_code == sf::Keyboard::D ? false : m_player->cInput->right;
    }

    if (event.type == sf::Event::Closed) {
      m_running = false;
    }
  }
}
void Game::sLifespan() {
  // Lifespan system
}

void Game::sRender() {
  if (!m_running) {
    m_window.close();
  }

  m_window.clear();

  for (auto e : m_entities.getEntities()) {
    e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    m_window.draw(e->cShape->circle);
  }
  // m_window.draw(m_scoreText);

  m_window.display();
}

void Game::sEnemySpawner() {
  // Enemy spawner system
}

void Game::sCollision() {
  // Collision detection system
}

// TODO finish adding all the properties of the player using the values from the config file
void Game::spawnPlayer() {
  auto  entity       = m_entities.addEntity(EntityTags::Player);
  float mx           = m_window.getSize().x / 2.0f;
  float my           = m_window.getSize().y / 2.0f;
  entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1, 1), 0);
  entity->cShape     = std::make_shared<CShape>(32.f, 8, sf::Color(80, 80, 80), sf::Color::Red, 4);
  entity->cInput     = std::make_shared<CInput>();
  m_player           = entity;
}

// TODO finish adding all the properties of the enemy using the values from the config file
void Game::spawnEnemy() {

  // the rainbow
  auto Red    = sf::Color::Red;
  auto Orange = sf::Color(255, 165, 0);
  auto Yellow = sf::Color::Yellow;
  auto Green  = sf::Color::Green;
  auto Blue   = sf::Color::Blue;
  auto Indigo = sf::Color(75, 0, 130);
  auto Violet = sf::Color(238, 130, 238);

  std::array<sf::Color, 7> colors = {Red, Orange, Yellow, Green, Blue, Indigo, Violet};

  auto  entity         = m_entities.addEntity(EntityTags::Enemy);
  float randomX        = rand() % m_window.getSize().x;
  float randomY        = rand() % m_window.getSize().y;
  float randomAngle    = rand() % 360;
  auto  randomColor    = colors[rand() % 7];
  auto  randomOutline  = colors[rand() % 7];
  float randomVertices = rand() % 10 + 3;
  m_lastEnemySpawnTime = m_currentFrame;
  entity->cTransform   = std::make_shared<CTransform>(Vec2(randomX, randomY), Vec2(1, 1), randomAngle);
  entity->cShape       = std::make_shared<CShape>(32.f, randomVertices, randomColor, randomOutline, 4);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
  // TODO: spawn small enemies at the location of the input enemy m_player
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos) {
  // Spawn a bullet entity from a given entity towards a mouse position
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  // Spawn a special weapon entity from a given entity
}
