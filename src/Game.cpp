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

  m_enemyConfig.SR   = 32;
  m_enemyConfig.CR   = 32;
  m_enemyConfig.OR   = 255;
  m_enemyConfig.OG   = 255;
  m_enemyConfig.OB   = 255;
  m_enemyConfig.OT   = 3;
  m_enemyConfig.VMIN = 3;
  m_enemyConfig.VMAX = 10;
  m_enemyConfig.L    = 3;
  m_enemyConfig.SI   = 8;
  m_enemyConfig.SMIN = 90;
  m_enemyConfig.SMAX = 60;

  // 15 15 8 255 0 0 255 100 150 4 9 90 6
  m_bulletConfig.SR = 15;
  m_bulletConfig.CR = 15;
  m_bulletConfig.FR = 8;
  m_bulletConfig.FG = 255;
  m_bulletConfig.FB = 0;
  m_bulletConfig.OR = 0;
  m_bulletConfig.OG = 0;
  m_bulletConfig.OB = 255;
  m_bulletConfig.OT = 2;
  m_bulletConfig.V  = 150;
  m_bulletConfig.L  = 4;
  m_bulletConfig.S  = 9;

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

    const auto left_mouse_pressed = event.type == sf::Event::MouseButtonPressed;

    if (left_mouse_pressed) {
      const Vec2 mousePos = {static_cast<float>(event.mouseButton.x),
                             static_cast<float>(event.mouseButton.y)};

      m_player->cInput->shoot = true;
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
  if (m_currentFrame - m_lastEnemySpawnTime > 120) {
    spawnEnemy();
  }

  if (m_player->cInput->shoot) {
    Vec2 mousePos{static_cast<float>(sf::Mouse::getPosition(m_window).x),
                  static_cast<float>(sf::Mouse::getPosition(m_window).y)};

    spawnBullet(m_player, mousePos);
    m_player->cInput->shoot = false;
  }
}

void Game::sCollision() {
  auto enforceBoundaries = [this](const std::shared_ptr<Entity> &entity) {
    const sf::CircleShape &shape      = entity->cShape->circle;
    const Vec2            &pos        = entity->cTransform->pos;
    const float            radius     = shape.getRadius();
    const auto             windowSize = m_window.getSize();

    if (pos.x + radius > windowSize.x) {
      entity->cTransform->pos.x = windowSize.x - radius;
    }
    if (pos.x - radius < 0) {
      entity->cTransform->pos.x = radius;
    }
    if (pos.y + radius > windowSize.y) {
      entity->cTransform->pos.y = windowSize.y - radius;
    }
    if (pos.y - radius < 0) {
      entity->cTransform->pos.y = radius;
    }
  };

  for (auto e : m_entities.getEntities()) {
    if (e->tag() != EntityTags::Bullet) {
      enforceBoundaries(e);

      return;
    }
  }
}

// TODO finish adding all the properties of the player using the values from the config file
void Game::spawnPlayer() {
  auto  entity       = m_entities.addEntity(EntityTags::Player);
  float mx           = m_window.getSize().x / 2.0f;
  float my           = m_window.getSize().y / 2.0f;
  m_player           = entity;
  entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1, 1), 0);
  entity->cShape     = std::make_shared<CShape>(
      m_playerConfig.SR, m_playerConfig.V,
      sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
      sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
  entity->cInput = std::make_shared<CInput>();
}

// TODO finish adding all the properties of the enemy using the values from the config file
void Game::spawnEnemy() {

  /*
    - Red
    - Orange
    - Yellow
    - Green
    - Blue
    - Indigo
    - Violet
  */
  std::array<sf::Color, 7> COLORS = {
      sf::Color::Red,  sf::Color(255, 165, 0), sf::Color::Yellow,        sf::Color::Green,
      sf::Color::Blue, sf::Color(75, 0, 130),  sf::Color(238, 130, 238),
  };

  int       MAX_VERTICES      = m_enemyConfig.VMAX;
  int       MIN_VERTICES      = m_enemyConfig.VMIN;
  int       RADIUS            = m_enemyConfig.CR;
  int       OUTLINE_THICKNESS = m_enemyConfig.OT;
  float     RANDOM_X          = rand() % m_window.getSize().x;
  float     RANDOM_Y          = rand() % m_window.getSize().y;
  float     RANDOM_ANGLE      = rand() % 360;
  float     RANDOM_VERTICES   = rand() % MAX_VERTICES + MIN_VERTICES;
  sf::Color COLOR             = COLORS[rand() % 7];
  sf::Color OUTLINE           = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

  auto entity = m_entities.addEntity(EntityTags::Enemy);

  m_lastEnemySpawnTime = m_currentFrame;
  entity->cTransform = std::make_shared<CTransform>(Vec2(RANDOM_X, RANDOM_Y), Vec2(1, 1), RANDOM_ANGLE);
  entity->cShape = std::make_shared<CShape>(RADIUS, RANDOM_VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
  // TODO: spawn small enemies at the location of the input enemy m_player
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos) {

  // Calculate the difference between the mouse position and the entity position
  auto difference = Vec2(static_cast<float>(mousePos.x - entity->cTransform->pos.x),
                         static_cast<float>(mousePos.y - entity->cTransform->pos.y));

  difference.normalize();

  // Spawn a bullet entity from a given entity
  const int  SHAPE_RADIUS      = m_bulletConfig.SR;
  const int  OUTLINE_THICKNESS = m_bulletConfig.OT;
  const auto COLOR             = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
  const auto OUTLINE           = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);
  const int  VERTICES          = 10;
  const auto VELOCITY          = Vec2(static_cast<float>(m_bulletConfig.S * difference.x),
                                      static_cast<float>(m_bulletConfig.S * difference.y));

  auto bullet        = m_entities.addEntity(EntityTags::Bullet);
  bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(1, 1), 0);
  bullet->cShape = std::make_shared<CShape>(SHAPE_RADIUS, VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  // Spawn a special weapon entity from a given entity
}
