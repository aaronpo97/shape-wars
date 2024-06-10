#include "../includes/Game.h"

#include <fstream>
#include <iostream>
#include <memory>

#include "../includes/CollisionHelpers.h"
#include "../includes/Components.h"
#include "../includes/MathHelpers.h"
#include "../includes/Tags.h"

Game::Game(const std::string &config) {
  init(config);
}

void Game::init(const std::string &path) {
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

  m_bulletConfig.SR = 15;
  m_bulletConfig.CR = 15;
  m_bulletConfig.FR = 255;
  m_bulletConfig.FG = 255;
  m_bulletConfig.FB = 255;
  m_bulletConfig.OR = 0;
  m_bulletConfig.OG = 0;
  m_bulletConfig.OB = 0;
  m_bulletConfig.OT = 2;
  m_bulletConfig.V  = 150;
  m_bulletConfig.L  = 4;
  m_bulletConfig.S  = 9;

  m_font.loadFromFile("../assets/fonts/Roboto.ttf");

  m_window.create(sf::VideoMode(3200, 1800), "Geometry Wars");

  m_window.setFramerateLimit(60);

  spawnPlayer();
}

void Game::run() {
  // Animation Loop
  while (m_running) {
    m_entities.update();

    if (!m_paused && m_lives > 0) {
      sEnemySpawner();
      sMovement();
      sCollision();
      sLifespan();
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
  bool &up_triggered    = m_player->cInput->up;
  bool &left_triggered  = m_player->cInput->left;
  bool &down_triggered  = m_player->cInput->down;
  bool &right_triggered = m_player->cInput->right;
  bool &shoot_triggered = m_player->cInput->shoot;

  sf::Event event;

  while (m_window.pollEvent(event)) {
    const bool key_released     = event.type == sf::Event::KeyReleased;
    const bool key_pressed      = event.type == sf::Event::KeyPressed;
    const bool mouse_pressed    = event.type == sf::Event::MouseButtonPressed;
    const bool close_triggered  = event.type == sf::Event::Closed;
    const bool resize_triggered = event.type == sf::Event::Resized;
    const bool lost_focus       = event.type == sf::Event::LostFocus;

    const bool left_mouse_pressed =
        mouse_pressed && event.mouseButton.button == sf::Mouse::Left;
    const bool right_mouse_pressed =
        mouse_pressed && event.mouseButton.button == sf::Mouse::Right;
    const sf::Keyboard::Key key_code = event.key.code;

    /**
     * Sets the up-, down-, left-, and right_triggered to true if the corresponding key is
     * pressed.
     *
     * Otherwise it stays at its current value.
     */
    if (key_pressed) {
      switch (key_code) {
        case sf::Keyboard::W: {
          up_triggered = true;
          break;
        }
        case sf::Keyboard::A: {
          left_triggered = true;
          break;
        }
        case sf::Keyboard::S: {
          down_triggered = true;
          break;
        }
        case sf::Keyboard::D: {
          right_triggered = true;
          break;
        }
        case sf::Keyboard::P: {
          m_paused ? setPaused(false) : setPaused(true);
          break;
        }
        case sf::Keyboard::R: {
          for (auto e : m_entities.getEntities()) {
            e->destroy();
          }
          m_lives = 5;
          m_score = 0;
          spawnPlayer();
          break;
        }
        default: {
          break;
        }
      }
    }

    if (key_released) {
      switch (key_code) {
        case sf::Keyboard::W: {
          up_triggered = false;
          break;
        }
        case sf::Keyboard::A: {
          left_triggered = false;
          break;
        }
        case sf::Keyboard::S: {
          down_triggered = false;
          break;
        }
        case sf::Keyboard::D: {
          right_triggered = false;
          break;
        }
        default: {
          break;
        }
      }
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

    if (resize_triggered || lost_focus) {
      m_window.setView(
          sf::View(sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y)));
    }
  }
}

void Game::sLifespan() {
  for (std::shared_ptr<Entity> entity : m_entities.getEntities()) {
    const std::shared_ptr<CLifespan> &lifespan = entity->cLifespan;
    const std::string                &tag      = entity->tag();

    if (!lifespan) {
      continue;
    }

    // get the percentage of the lifespan remaining
    const float percentage_remaining =
        static_cast<float>(lifespan->remaining) / static_cast<float>(lifespan->total);

    const sf::Uint8 updated_alpha = percentage_remaining * 255;

    lifespan->remaining -= 1;

    const sf::Color &fill    = entity->cShape->circle.getFillColor();
    const sf::Color &outline = entity->cShape->circle.getOutlineColor();

    entity->cShape->circle.setFillColor(sf::Color(fill.r, fill.g, fill.b, updated_alpha));
    entity->cShape->circle.setOutlineColor(
        sf::Color(outline.r, outline.g, outline.b, updated_alpha));

    if (lifespan->remaining <= 0) {
      entity->destroy();
    };
  }
}

void Game::sRender() {
  if (!m_running) {
    m_window.close();
  }

  m_window.clear();

  sf::Text scoreText;
  scoreText.setFont(m_font);
  scoreText.setString("Score: " + std::to_string(m_score));
  scoreText.setCharacterSize(32);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setPosition(10, 10);

  sf::Text livesText;
  livesText.setFont(m_font);
  livesText.setString("Lives: " + std::to_string(m_lives));
  livesText.setCharacterSize(32);
  livesText.setFillColor(sf::Color::White);
  livesText.setPosition(10, 50);

  sf::Text pausedText;
  pausedText.setFont(m_font);
  pausedText.setString("PAUSED");
  pausedText.setCharacterSize(32);
  pausedText.setFillColor(sf::Color::White);
  pausedText.setPosition(10, 90);

  sf::Text gameOverText;
  gameOverText.setFont(m_font);
  gameOverText.setString("GAME OVER");
  gameOverText.setCharacterSize(32);
  gameOverText.setFillColor(sf::Color::White);
  gameOverText.setPosition(10, 130);

  sf::Text restartText;
  restartText.setFont(m_font);
  restartText.setString("Press R to restart");
  restartText.setCharacterSize(32);
  restartText.setFillColor(sf::Color::White);
  restartText.setPosition(10, 170);

  m_window.draw(scoreText);
  m_window.draw(livesText);

  if (m_paused) {
    m_window.draw(pausedText);
  }

  if (m_lives <= 0) {
    m_window.draw(gameOverText);
    m_window.draw(restartText);
  }

  for (auto e : m_entities.getEntities()) {
    e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    m_window.draw(e->cShape->circle);
  }

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
      bool collision =
          CollisionHelpers::calculateCollisionBetweenEntities(playerEntity, enemyEntity);
      if (collision) {
        playerEntity->destroy();
        m_score = m_score > 0 ? m_score - 1 : 0;
        spawnPlayer();
        m_lives -= 1;
        enemyEntity->destroy();
      }
    }

    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(playerEntity, m_window.getSize());
    CollisionHelpers::enforcePlayerBounds(playerEntity, collidesWithBoundary,
                                          m_window.getSize());
  }

  for (auto enemyEntity : enemyEntities) {
    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(enemyEntity, m_window.getSize());
    CollisionHelpers::enforceEnemyBounds(enemyEntity, collidesWithBoundary,
                                         m_window.getSize());
  }

  for (auto bulletEntity : bulletEntities) {
    for (auto enemyEntity : enemyEntities) {
      bool collision =
          CollisionHelpers::calculateCollisionBetweenEntities(bulletEntity, enemyEntity);
      if (collision) {
        bulletEntity->destroy();
        enemyEntity->destroy();

        spawnSmallEnemies(enemyEntity);
        m_score = m_score + 2;
      }
    }

    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(bulletEntity, m_window.getSize());

    CollisionHelpers::enforceBulletBounds(bulletEntity, collidesWithBoundary);
  }

  for (auto smallEnemyEntity : m_entities.getEntities(EntityTags::SmallEnemy)) {
    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(smallEnemyEntity, m_window.getSize());

    if (collidesWithBoundary.any()) {
      smallEnemyEntity->destroy();
    }

    for (auto bulletEntity : bulletEntities) {
      bool collision =
          CollisionHelpers::calculateCollisionBetweenEntities(smallEnemyEntity, bulletEntity);
      if (collision) {
        bulletEntity->destroy();
        smallEnemyEntity->destroy();
        m_score = m_score + 5;
        spawnSmallEnemies(smallEnemyEntity);
      }
    }
  }
}

// TODO finish adding all the properties of the player using the values from the
// config file
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
  entity->cInput     = std::make_shared<CInput>();
  entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
}

// TODO finish adding all the properties of the enemy using the values from the
// config file
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
  const sf::Color OUTLINE = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

  const float RANDOM_POS_X = rand() % m_window.getSize().x;
  const float RANDOM_POS_Y = rand() % m_window.getSize().y;
  const Vec2  RANDOM_POS   = Vec2(RANDOM_POS_X, RANDOM_POS_Y);

  int random_number = rand() % 2;

  const float RANDOM_VEL_X = (random_number == 0) ? -1 : 1;
  const float RANDOM_VEL_Y = (random_number == 0) ? -1 : 1;
  const Vec2  RANDOM_VEL   = Vec2(RANDOM_VEL_X, RANDOM_VEL_Y);

  auto entity = m_entities.addEntity(EntityTags::Enemy);

  m_lastEnemySpawnTime = m_currentFrame;
  entity->cTransform   = std::make_shared<CTransform>(
      Vec2(RANDOM_POS_X, RANDOM_POS_Y), Vec2(RANDOM_VEL_X, RANDOM_VEL_Y), RANDOM_ANGLE);

  entity->cShape =
      std::make_shared<CShape>(RADIUS, RANDOM_VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);

  entity->cLifespan  = std::make_shared<CLifespan>(2000);
  entity->cCollision = std::make_shared<CCollision>(RADIUS);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> parentEntity) {

  const std::shared_ptr<CShape>     &parentCShape     = parentEntity->cShape;
  const std::shared_ptr<CTransform> &parentCTransform = parentEntity->cTransform;
  const std::shared_ptr<CCollision> &parentCCollision = parentEntity->cCollision;
  const std::shared_ptr<CLifespan>  &parentCLifespan  = parentEntity->cLifespan;

  Vec2 &parentPosition      = parentCTransform->pos;
  Vec2  parentPositionCopy  = parentPosition;
  Vec2  normalizedParentPos = parentPositionCopy.normalize();

  // Set each enemy to the same color as the original, half the size
  const sf::Color &fill                   = parentCShape->circle.getFillColor();
  const sf::Color &outline                = parentCShape->circle.getOutlineColor();
  const float     &thickness              = parentCShape->circle.getOutlineThickness();
  Vec2            &velocity               = parentCTransform->velocity;
  const float      smEnemyShapeRadius     = parentCShape->circle.getRadius() * 0.5f;
  const float      smEnemyCollisionRadius = parentEntity->cCollision->radius * 0.5f;
  float            angle                  = 0;
  const size_t     vertices               = parentCShape->circle.getPointCount();

  for (size_t i = 0; i < vertices; i += 1) {
    std::shared_ptr<Entity> smallEnemy = m_entities.addEntity(EntityTags::SmallEnemy);

    float radians              = MathHelpers::degreesToRadians(angle);
    float percentage_remaining = static_cast<float>(parentCLifespan->remaining) /
                                 static_cast<float>(parentCLifespan->total);

    Vec2 velocity =
        Vec2(cos(radians) * normalizedParentPos.x + sin(radians) * normalizedParentPos.y,
             sin(radians) * normalizedParentPos.x - cos(radians) * normalizedParentPos.y);

    float vector_length = MathHelpers::pythagoras(velocity.x, velocity.y);

    Vec2 normalizedVelocity =
        Vec2(velocity.x / vector_length, velocity.y / vector_length).normalize();

    Vec2 newVelocity = Vec2(normalizedVelocity.x * parentEntity->cTransform->velocity.x,
                            normalizedVelocity.y * parentEntity->cTransform->velocity.y);

    smallEnemy->cShape =
        std::make_shared<CShape>(smEnemyShapeRadius, vertices, fill, outline, thickness);
    smallEnemy->cTransform = std::make_shared<CTransform>(parentPosition, newVelocity, 0);
    smallEnemy->cLifespan  = std::make_shared<CLifespan>(parentCLifespan->total / 10);

    smallEnemy->cLifespan->remaining = smallEnemy->cLifespan->total * percentage_remaining;
    smallEnemy->cCollision           = std::make_shared<CCollision>(smEnemyCollisionRadius);

    angle += 360 / vertices;
  }
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
  const sf::Color COLOR   = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
  const sf::Color OUTLINE = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);
  const int       VERTICES          = 10;
  const Vec2     &STARTING_POSITION = entity->cTransform->pos;
  const int       ANGLE             = 0;
  // Spawn a bullet entity from a given entity

  std::shared_ptr<Entity> bullet = m_entities.addEntity(EntityTags::Bullet);
  bullet->cTransform = std::make_shared<CTransform>(STARTING_POSITION, VELOCITY, ANGLE);
  bullet->cShape =
      std::make_shared<CShape>(SHAPE_RADIUS, VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
  bullet->cLifespan  = std::make_shared<CLifespan>(500);
  bullet->cCollision = std::make_shared<CCollision>(SHAPE_RADIUS);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  // Spawn a special weapon entity from a given entity
}
