#include "../includes/Game.h"
#include "../includes/CollisionHelpers.h"
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
  m_enemyConfig.VMAX = 6;
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

  m_font.loadFromFile("../assets/fonts/Roboto.ttf");

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
    }

    sUserInput();
    sRender();
    m_currentFrame++;
  }
}

void Game::setPaused(bool paused) {
  m_paused = paused;
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
    } else {
      // Updates the position of the player
      m_player->cTransform->pos += playerVelocity;
      // Rotates the player
      e->cTransform->angle += 2.0f;
      e->cShape->circle.setRotation(e->cTransform->angle);
    }
  }

  // TODO: if the enemy hits the boundary, it should move in the opposite direction
}

void Game::sUserInput() {
  /*
   * Declare references to the input booleans for easier access.
   */
  bool &up_triggered    = m_player->cInput->up;
  bool &left_triggered  = m_player->cInput->left;
  bool &down_triggered  = m_player->cInput->down;
  bool &right_triggered = m_player->cInput->right;
  bool &shoot_triggered = m_player->cInput->shoot;

  sf::Event event;

  while (m_window.pollEvent(event)) {
    const bool key_released          = event.type == sf::Event::KeyReleased;
    const bool key_pressed           = event.type == sf::Event::KeyPressed;
    const bool mouse_pressed         = event.type == sf::Event::MouseButtonPressed;
    const bool close_triggered       = event.type == sf::Event::Closed;
    const bool left_mouse_pressed    = mouse_pressed && event.mouseButton.button == sf::Mouse::Left;
    const bool right_mouse_pressed   = mouse_pressed && event.mouseButton.button == sf::Mouse::Right;
    const sf::Keyboard::Key key_code = event.key.code;

    /**
     * Sets the up-, down-, left-, and right_triggered to true if the corresponding key is pressed.
     *
     * Otherwise it stays at its current value.
     */
    if (key_pressed) {
      up_triggered    = (key_code == sf::Keyboard::W) || up_triggered;
      left_triggered  = (key_code == sf::Keyboard::A) || left_triggered;
      down_triggered  = (key_code == sf::Keyboard::S) || down_triggered;
      right_triggered = (key_code == sf::Keyboard::D) || right_triggered;

      if (key_code == sf::Keyboard::P) {
        m_paused ? setPaused(false) : setPaused(true);
      }
    }

    if (key_released) {
      up_triggered    = key_code == sf::Keyboard::W ? false : up_triggered;
      left_triggered  = key_code == sf::Keyboard::A ? false : left_triggered;
      down_triggered  = key_code == sf::Keyboard::S ? false : down_triggered;
      right_triggered = key_code == sf::Keyboard::D ? false : right_triggered;
    }

    if (mouse_pressed) {
      if (right_mouse_pressed) {
        spawnSpecialWeapon(m_player);
      }
      if (left_mouse_pressed) {
        shoot_triggered = true;
      }
    }

    if (close_triggered) {
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

  // display the score
  std::string scoreText = "Score: " + std::to_string(m_score);

  sf::Text text;
  text.setFont(m_font);
  text.setString(scoreText);
  text.setCharacterSize(32);
  text.setFillColor(sf::Color::White);
  text.setPosition(10, 10);

  m_window.draw(text);

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

  const auto &playerEntities = m_entities.getEntities(EntityTags::Player);
  const auto &enemyEntities  = m_entities.getEntities(EntityTags::Enemy);
  const auto &bulletEntities = m_entities.getEntities(EntityTags::Bullet);

  for (auto playerEntity : playerEntities) {
    for (auto enemyEntity : enemyEntities) {
      bool collision = CollisionHelpers::calculateCollisionBetweenEntities(playerEntity, enemyEntity);
      if (collision) {
        playerEntity->destroy();
        m_score = m_score > 0 ? m_score - 1 : 0;
        spawnPlayer();
      }
    }

    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(playerEntity, m_window.getSize());
    CollisionHelpers::enforcePlayerBounds(playerEntity, collidesWithBoundary, m_window.getSize());
  }

  for (auto enemyEntity : enemyEntities) {
    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(enemyEntity, m_window.getSize());
    CollisionHelpers::enforceEnemyBounds(enemyEntity, collidesWithBoundary, m_window.getSize());
  }

  for (auto bulletEntity : bulletEntities) {

    for (auto enemyEntity : enemyEntities) {
      bool collision = CollisionHelpers::calculateCollisionBetweenEntities(bulletEntity, enemyEntity);
      if (collision) {
        bulletEntity->destroy();
        enemyEntity->destroy();
        m_score = m_score + 2;
      }
    }

    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(bulletEntity, m_window.getSize());

    CollisionHelpers::enforceBulletBounds(bulletEntity, collidesWithBoundary);
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

  srand(time(NULL));
  /*
    - Red
    - Orange
    - Yellow
    - Green
    - Blue
    - Indigo
    - Violet
  */
  const std::array<sf::Color, 7> COLORS = {
      sf::Color::Red,  sf::Color(255, 165, 0), sf::Color::Yellow,        sf::Color::Green,
      sf::Color::Blue, sf::Color(75, 0, 130),  sf::Color(238, 130, 238),
  };
  const int       MAX_VERTICES      = m_enemyConfig.VMAX;
  const int       MIN_VERTICES      = m_enemyConfig.VMIN;
  const int       RADIUS            = m_enemyConfig.CR;
  const int       OUTLINE_THICKNESS = m_enemyConfig.OT;
  const float     RANDOM_ANGLE      = rand() % 360;
  const float     RANDOM_VERTICES   = rand() % MAX_VERTICES + MIN_VERTICES;
  const sf::Color COLOR             = COLORS[rand() % 7];
  const sf::Color OUTLINE           = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

  const float RANDOM_POS_X = rand() % m_window.getSize().x;
  const float RANDOM_POS_Y = rand() % m_window.getSize().y;
  const Vec2  RANDOM_POS   = Vec2(RANDOM_POS_X, RANDOM_POS_Y);

  int random_number = rand() % 2;

  const float RANDOM_VEL_X = (random_number == 0) ? -1 : 1;
  const float RANDOM_VEL_Y = (random_number == 0) ? -1 : 1;
  const Vec2  RANDOM_VEL   = Vec2(RANDOM_VEL_X, RANDOM_VEL_Y);

  auto entity = m_entities.addEntity(EntityTags::Enemy);

  m_lastEnemySpawnTime = m_currentFrame;
  entity->cTransform   = std::make_shared<CTransform>(Vec2(RANDOM_POS_X, RANDOM_POS_Y),
                                                      Vec2(RANDOM_VEL_X, RANDOM_VEL_Y), RANDOM_ANGLE);

  entity->cShape = std::make_shared<CShape>(RADIUS, RANDOM_VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
  // TODO: spawn small enemies at the location of the input enemy m_player
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos) {

  /*
   * This Difference is used to determine the direction of the bullet.
   * Moves to left:   `x < 0`
   * Moves to right:  `x > 0`
   * Moves up:        `y < 0`
   * Moves down:      `y > 0`
   */
  Vec2 difference = Vec2(static_cast<float>(mousePos.x - entity->cTransform->pos.x),
                         static_cast<float>(mousePos.y - entity->cTransform->pos.y))
                        .normalize();

  const Vec2 VELOCITY = Vec2(static_cast<float>(m_bulletConfig.S * difference.x),
                             static_cast<float>(m_bulletConfig.S * difference.y));

  const int       SHAPE_RADIUS      = m_bulletConfig.SR;
  const int       OUTLINE_THICKNESS = m_bulletConfig.OT;
  const sf::Color COLOR             = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
  const sf::Color OUTLINE           = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);
  const int       VERTICES          = 10;
  const Vec2     &STARTING_POSITION = entity->cTransform->pos;
  const int       ANGLE             = 0;
  // Spawn a bullet entity from a given entity

  std::shared_ptr<Entity> bullet = m_entities.addEntity(EntityTags::Bullet);
  bullet->cTransform             = std::make_shared<CTransform>(STARTING_POSITION, VELOCITY, ANGLE);
  bullet->cShape = std::make_shared<CShape>(SHAPE_RADIUS, VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  // Spawn a special weapon entity from a given entity
}
