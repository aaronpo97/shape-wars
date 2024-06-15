#include "../includes/Game.h"

#include <fstream>
#include <iostream>
#include <memory>

#include "../includes/CollisionHelpers.h"
#include "../includes/Components.h"
#include "../includes/MathHelpers.h"
#include "../includes/Tags.h"

Game::Game(const std::string &config_path) {
  init(config_path);
}

void Game::init(const std::string &config_path) {
  m_configReader = ConfigReader(config_path);
  m_font.loadFromFile("../assets/fonts/Roboto.ttf");
  const auto &WindowConfig = m_configReader.getWindowConfig();
  m_window.create(sf::VideoMode(WindowConfig.W, WindowConfig.H), "SFML works!");
  m_window.setFramerateLimit(WindowConfig.FL);

  shootBuffer.loadFromFile("../assets/sounds/placeholder.wav");
  enemyShootBuffer.loadFromFile("../assets/sounds/placeholder.wav");
  specialWeaponBuffer.loadFromFile("../assets/sounds/placeholder.wav");
  playerHitBuffer.loadFromFile("../assets/sounds/placeholder.wav");

  spawnPlayer();
}

void Game::run() {
  // Animation Loop
  while (m_running) {
    m_entities.update();

    if (!m_paused && m_lives > 0) {
      sSpawner();
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
  const PlayerConfig &m_playerConfig = m_configReader.getPlayerConfig();
  Vec2                playerVelocity;

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
  bool &up_triggered      = m_player->cInput->up;
  bool &left_triggered    = m_player->cInput->left;
  bool &down_triggered    = m_player->cInput->down;
  bool &right_triggered   = m_player->cInput->right;
  bool &shoot_triggered   = m_player->cInput->shoot;
  bool &special_triggered = m_player->cInput->special;

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
          setPaused(!m_paused && m_lives > 0 ? true : false);
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
        special_triggered = true;
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

  m_window.clear(sf::Color(25, 25, 25));

  for (auto e : m_entities.getEntities()) {
    e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    m_window.draw(e->cShape->circle);
  }

  const sf::Vector2f initialTextPosition = {10, 10};

  auto calculateNextTextPosition = [](const sf::Text &text) {
    return sf::Vector2f(text.getPosition().x,
                        text.getPosition().y + text.getGlobalBounds().height + 20);
  };

  sf::Text scoreText;
  scoreText.setFont(m_font);
  scoreText.setString("Score: " + std::to_string(m_score));
  scoreText.setCharacterSize(32);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setPosition(initialTextPosition);

  sf::Text livesText;
  livesText.setFont(m_font);
  livesText.setString("Lives: " + std::to_string(m_lives));
  livesText.setCharacterSize(32);
  livesText.setFillColor(sf::Color::White);
  livesText.setPosition(calculateNextTextPosition(scoreText));

  sf::Text pausedText;
  pausedText.setFont(m_font);
  pausedText.setString("PAUSED");
  pausedText.setCharacterSize(32);
  pausedText.setFillColor(sf::Color::White);
  pausedText.setPosition(calculateNextTextPosition(livesText));

  sf::Text gameOverText;
  gameOverText.setFont(m_font);
  gameOverText.setString("GAME OVER");
  gameOverText.setCharacterSize(32);
  gameOverText.setFillColor(sf::Color::White);
  gameOverText.setPosition(calculateNextTextPosition(livesText));

  sf::Text restartText;
  restartText.setFont(m_font);
  restartText.setString("Press R to restart");
  restartText.setCharacterSize(32);
  restartText.setFillColor(sf::Color::White);
  restartText.setPosition(calculateNextTextPosition(gameOverText));

  m_window.draw(scoreText);
  m_window.draw(livesText);

  if (m_paused) {
    m_window.draw(pausedText);
  }

  if (m_lives <= 0) {
    m_window.draw(gameOverText);
    m_window.draw(restartText);
  }

  m_window.display();
}

void Game::sSpawner() {
  const EnemyConfig         &m_enemyConfig         = m_configReader.getEnemyConfig();
  const SpecialWeaponConfig &m_specialWeaponConfig = m_configReader.getSpecialWeaponConfig();

  const int framesSinceLastEnemySpawn = m_currentFrame - m_lastEnemySpawnTime;

  const int enemySpawnInterval    = m_enemyConfig.SI;
  const int specialWeaponCooldown = m_specialWeaponConfig.CD;

  const bool bulletSpawnEnabled        = m_player->cInput->shoot;
  bool       specialWeaponSpawnEnabled = m_player->cInput->special;
  const bool enemySpawnEnabled         = framesSinceLastEnemySpawn > enemySpawnInterval;

  if (enemySpawnEnabled) {
    playSound(SoundType::ENEMY_SHOOT);
    spawnEnemy();
  }

  if (bulletSpawnEnabled) {
    playSound(SoundType::SHOOT);
    Vec2 mousePos{static_cast<float>(sf::Mouse::getPosition(m_window).x),
                  static_cast<float>(sf::Mouse::getPosition(m_window).y)};

    spawnBullet(m_player, mousePos);
    m_player->cInput->shoot = false;
  }

  if (specialWeaponSpawnEnabled && m_specialWeaponCounter < m_maxSpecialWeaponUsage) {
    playSound(SoundType::SPECIAL_WEAPON);
    spawnSpecialWeapon(m_player);
    m_specialWeaponCounter++;
    m_player->cInput->special = false;
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
        playSound(SoundType::PLAYER_HIT);
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

    for (auto bulletEntity : bulletEntities) {
      bool collision =
          CollisionHelpers::calculateCollisionBetweenEntities(enemyEntity, bulletEntity);
      if (collision) {
        playSound(SoundType::ENEMY_SHOOT);
        bulletEntity->destroy();
        enemyEntity->destroy();
        m_score = m_score + 2;
        spawnSmallEnemies(enemyEntity);
      }
    }

    for (auto specialBulletEntity : m_entities.getEntities(EntityTags::SpecialBullet)) {
      bool collision = CollisionHelpers::calculateCollisionBetweenEntities(
          enemyEntity, specialBulletEntity);
      if (collision) {
        playSound(SoundType::ENEMY_SHOOT);

        specialBulletEntity->destroy();
        enemyEntity->destroy();
        m_score = m_score + 5;
        spawnSmallEnemies(enemyEntity);
      }
    }
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
        playSound(SoundType::ENEMY_SHOOT);
        bulletEntity->destroy();
        smallEnemyEntity->destroy();
        m_score = m_score + 5;
        spawnSmallEnemies(smallEnemyEntity);
      }
      std::bitset<4> collidesWithBoundary =
          CollisionHelpers::detectOutOfBounds(bulletEntity, m_window.getSize());

      CollisionHelpers::enforceBulletBounds(bulletEntity, collidesWithBoundary);
    }

    for (auto specialBulletEntity : m_entities.getEntities(EntityTags::SpecialBullet)) {
      bool collision = CollisionHelpers::calculateCollisionBetweenEntities(
          smallEnemyEntity, specialBulletEntity);
      if (collision) {
        playSound(SoundType::ENEMY_SHOOT);
        specialBulletEntity->destroy();
        smallEnemyEntity->destroy();
        m_score = m_score + 10;
        spawnSmallEnemies(smallEnemyEntity);
      }
      std::bitset<4> collidesWithBoundary =
          CollisionHelpers::detectOutOfBounds(specialBulletEntity, m_window.getSize());

      CollisionHelpers::enforceBulletBounds(specialBulletEntity, collidesWithBoundary);
    }
  }

  for (auto bulletEntity : bulletEntities) {
    std::bitset<4> collidesWithBoundary =
        CollisionHelpers::detectOutOfBounds(bulletEntity, m_window.getSize());

    CollisionHelpers::enforceBulletBounds(bulletEntity, collidesWithBoundary);
  }
}

void Game::spawnPlayer() {
  const PlayerConfig           &m_playerConfig = m_configReader.getPlayerConfig();
  const std::shared_ptr<Entity> player         = m_entities.addEntity(EntityTags::Player);
  float                         mx             = m_window.getSize().x / 2.0f;
  float                         my             = m_window.getSize().y / 2.0f;

  m_player = player;

  const auto cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1, 1), 0);
  const auto cShape     = std::make_shared<CShape>(
      m_playerConfig.SR, m_playerConfig.V,
      sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
      sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
  const auto cInput     = std::make_shared<CInput>();
  const auto cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

  player->cTransform = cTransform;
  player->cShape     = cShape;
  player->cInput     = cInput;
  player->cCollision = cCollision;
}

void Game::spawnEnemy() {
  const EnemyConfig &m_enemyConfig = m_configReader.getEnemyConfig();
  srand(time(NULL));
  // Red, Orange, Yellow, Green, Blue, Indigo, Violet
  const std::array<sf::Color, 7> COLORS = {
      sf::Color::Red,  sf::Color(255, 165, 0), sf::Color::Yellow,        sf::Color::Green,
      sf::Color::Blue, sf::Color(75, 0, 130),  sf::Color(238, 130, 238),
  };
  const int       MAX_VERTICES      = m_enemyConfig.VMAX;
  const int       MIN_VERTICES      = m_enemyConfig.VMIN;
  const int       RADIUS            = m_enemyConfig.SR;
  const int       COLLISION_RADIUS  = m_enemyConfig.CR;
  const int       OUTLINE_THICKNESS = m_enemyConfig.OT;
  const float     RANDOM_ANGLE      = rand() % 360;
  const float     RANDOM_VERTICES   = rand() % MAX_VERTICES + MIN_VERTICES;
  const sf::Color COLOR             = COLORS[rand() % 7];
  const sf::Color OUTLINE = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

  const int MIN_DISTANCE_TO_PLAYER = 100;

  float randomPosX;
  float randomPosY;
  float distanceToPlayer;
  bool  tooCloseToPlayer;

  do {
    randomPosX = rand() % m_window.getSize().x;
    randomPosY = rand() % m_window.getSize().y;

    distanceToPlayer = MathHelpers::pythagoras(randomPosX - m_player->cTransform->pos.x,
                                               randomPosY - m_player->cTransform->pos.y);

    tooCloseToPlayer = distanceToPlayer < MIN_DISTANCE_TO_PLAYER;
  } while (tooCloseToPlayer);

  const Vec2 RANDOM_POS = Vec2(randomPosX, randomPosY);
  const Vec2 RANDOM_VEL = Vec2(rand() % 2 ? -1 : 1, rand() % 2 ? -1 : 1);

  std::shared_ptr<Entity> enemy = m_entities.addEntity(EntityTags::Enemy);

  const auto cTransform = std::make_shared<CTransform>(RANDOM_POS, RANDOM_VEL, RANDOM_ANGLE);
  const auto cShape =
      std::make_shared<CShape>(RADIUS, RANDOM_VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
  const auto cLifespan  = std::make_shared<CLifespan>(m_enemyConfig.L);
  const auto cCollision = std::make_shared<CCollision>(COLLISION_RADIUS);

  enemy->cTransform = cTransform;
  enemy->cShape     = cShape;
  enemy->cLifespan  = cLifespan;
  enemy->cCollision = cCollision;

  m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> parentEntity) {
  const std::shared_ptr<CShape>     &parentCShape     = parentEntity->cShape;
  const std::shared_ptr<CTransform> &parentCTransform = parentEntity->cTransform;
  const std::shared_ptr<CCollision> &parentCCollision = parentEntity->cCollision;
  const std::shared_ptr<CLifespan>  &parentCLifespan  = parentEntity->cLifespan;

  Vec2 &parentPosition      = parentCTransform->pos;
  Vec2  parentPositionCopy  = parentPosition;
  Vec2  normalizedParentPos = parentPositionCopy.normalize();

  const sf::Color &FILL                      = parentCShape->circle.getFillColor();
  const sf::Color &OUTLINE_COLOR             = parentCShape->circle.getOutlineColor();
  const float     &OUTLINE_THICKNESS         = parentCShape->circle.getOutlineThickness();
  Vec2            &VELOCITY                  = parentCTransform->velocity;
  const float      SM_ENEMY_SHAPE_RADIUS     = parentCShape->circle.getRadius() * 0.5f;
  const float      SM_ENEMY_COLLISION_RADIUS = parentEntity->cCollision->radius * 0.5f;
  float            ANGLE                     = 0;
  const size_t     VERTICES                  = parentCShape->circle.getPointCount();

  for (size_t i = 0; i < VERTICES; i += 1) {

    float radians              = MathHelpers::degreesToRadians(ANGLE);
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

    std::shared_ptr<Entity> smallEnemy = m_entities.addEntity(EntityTags::SmallEnemy);

    const auto cShape     = std::make_shared<CShape>(SM_ENEMY_SHAPE_RADIUS, VERTICES, FILL,
                                                     OUTLINE_COLOR, OUTLINE_THICKNESS);
    const auto cTransform = std::make_shared<CTransform>(parentPosition, newVelocity, 0);
    const auto cLifespan  = std::make_shared<CLifespan>(parentCLifespan->total / 10);
    const auto cCollision = std::make_shared<CCollision>(SM_ENEMY_SHAPE_RADIUS);

    cLifespan->remaining = cLifespan->total * percentage_remaining;

    smallEnemy->cTransform = cTransform;
    smallEnemy->cShape     = cShape;
    smallEnemy->cLifespan  = cLifespan;
    smallEnemy->cCollision = cCollision;

    ANGLE += 360 / VERTICES;
  }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos) {
  const BulletConfig &m_bulletConfig = m_configReader.getBulletConfig();
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

  const auto FILL_RED   = m_bulletConfig.FR;
  const auto FILL_GREEN = m_bulletConfig.FG;
  const auto FILL_BLUE  = m_bulletConfig.FB;

  const auto OUTLINE_RED   = m_bulletConfig.OR;
  const auto OUTLINE_GREEN = m_bulletConfig.OG;
  const auto OUTLINE_BLUE  = m_bulletConfig.OB;

  const sf::Color &COLOR             = sf::Color(FILL_RED, FILL_GREEN, FILL_BLUE);
  const sf::Color &OUTLINE           = sf::Color(OUTLINE_RED, OUTLINE_GREEN, OUTLINE_BLUE);
  const int        SHAPE_RADIUS      = m_bulletConfig.SR;
  const int        COLLISION_RADIUS  = m_bulletConfig.CR;
  const int        OUTLINE_THICKNESS = m_bulletConfig.OT;
  const int        VERTICES          = 10;
  const int        ANGLE             = 0;
  const Vec2      &STARTING_POSITION = entity->cTransform->pos;

  const auto cTransform = std::make_shared<CTransform>(STARTING_POSITION, VELOCITY, ANGLE);
  const auto cShape =
      std::make_shared<CShape>(SHAPE_RADIUS, VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
  const auto cLifespan  = std::make_shared<CLifespan>(500);
  const auto cCollision = std::make_shared<CCollision>(COLLISION_RADIUS);

  std::shared_ptr<Entity> bullet = m_entities.addEntity(EntityTags::Bullet);
  bullet->cTransform             = cTransform;
  bullet->cShape                 = cShape;
  bullet->cLifespan              = cLifespan;
  bullet->cCollision             = cCollision;
}
// spawns 10 bullets in a circle around the player, taking into account the player's rotation
void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
  const SpecialWeaponConfig &m_specialWeaponConfig = m_configReader.getSpecialWeaponConfig();
  const Vec2                &START_POSITION        = entity->cTransform->pos;
  const float                PLAYER_ANGLE          = entity->cTransform->angle;

  const auto FILL_RED   = m_specialWeaponConfig.FR;
  const auto FILL_GREEN = m_specialWeaponConfig.FG;
  const auto FILL_BLUE  = m_specialWeaponConfig.FB;

  const auto OUTLINE_RED   = m_specialWeaponConfig.OR;
  const auto OUTLINE_GREEN = m_specialWeaponConfig.OG;
  const auto OUTLINE_BLUE  = m_specialWeaponConfig.OB;

  const sf::Color &COLOR   = sf::Color(FILL_RED, FILL_GREEN, FILL_BLUE);
  const sf::Color &OUTLINE = sf::Color(OUTLINE_RED, OUTLINE_GREEN, OUTLINE_BLUE);

  const int LIFESPAN = m_specialWeaponConfig.L;

  const int SHAPE_RADIUS      = m_specialWeaponConfig.SR / 2;
  const int COLLISION_RADIUS  = m_specialWeaponConfig.CR / 2;
  const int OUTLINE_THICKNESS = m_specialWeaponConfig.OT / 2;
  const int VERTICES          = 10;
  const int ANGLE             = 0;

  const int   NUMBER_OF_BULLETS = 5;
  const float ANGLE_INCREMENT   = 360.0f / static_cast<float>(NUMBER_OF_BULLETS);

  float angle = 0.0f;
  for (int i = 0; i < NUMBER_OF_BULLETS; i++) {
    const float bulletAngle = PLAYER_ANGLE + angle;
    const Vec2  mousePos =
        Vec2(START_POSITION.x + 100 * cos(MathHelpers::degreesToRadians(bulletAngle)),
             START_POSITION.y + 100 * sin(MathHelpers::degreesToRadians(bulletAngle)));

    std::shared_ptr<Entity> specialBullet = m_entities.addEntity(EntityTags::SpecialBullet);

    const Vec2 VELOCITY =
        Vec2(NUMBER_OF_BULLETS * cos(MathHelpers::degreesToRadians(bulletAngle)),
             NUMBER_OF_BULLETS * sin(MathHelpers::degreesToRadians(bulletAngle)));

    const auto cTransform = std::make_shared<CTransform>(START_POSITION, VELOCITY, ANGLE);
    const auto cShape =
        std::make_shared<CShape>(SHAPE_RADIUS, VERTICES, COLOR, OUTLINE, OUTLINE_THICKNESS);
    const auto cLifespan  = std::make_shared<CLifespan>(LIFESPAN);
    const auto cCollision = std::make_shared<CCollision>(COLLISION_RADIUS);

    specialBullet->cTransform = cTransform;
    specialBullet->cShape     = cShape;
    specialBullet->cLifespan  = cLifespan;
    specialBullet->cCollision = cCollision;

    angle += ANGLE_INCREMENT;
  }
}

void Game::playSound(SoundType soundType) {
  switch (soundType) {
    case SoundType::SHOOT: {
      shootSound.setBuffer(shootBuffer);
      shootSound.play();
      break;
    }
    case SoundType::ENEMY_SHOOT: {
      enemyShootSound.setBuffer(enemyShootBuffer);
      enemyShootSound.play();
      break;
    }
    case SoundType::SPECIAL_WEAPON: {
      specialWeaponSound.setBuffer(specialWeaponBuffer);
      specialWeaponSound.play();
      break;
    }
    case SoundType::PLAYER_HIT: {
      playerHitSound.setBuffer(playerHitBuffer);
      playerHitSound.play();
      break;
    }
    default: {
      break;
    }
  }
}