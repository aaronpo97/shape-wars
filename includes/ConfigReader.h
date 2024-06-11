#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <string>

struct WindowConfig {
  int W;  // Width
  int H;  // Height
  int FL; // Framerate limit
  int FS; // Fullscreen
};

struct PlayerConfig {
  int   SR; // Shape radius
  int   CR; // Collision radius
  int   FR; // Fill red
  int   FG; // Fill green
  int   FB; // Fill blue
  int   OR; // Outline red
  int   OG; // Outline green
  int   OB; // Outline blue
  int   OT; // Outline thickness
  int   V;  // Velocity
  float S;  // Speed
};

struct EnemyConfig {
  int   SR;   // Shape radius
  int   CR;   // Collision radius
  int   OR;   // Outline red
  int   OG;   // Outline green
  int   OB;   // Outline blue
  int   OT;   // Outline thickness
  int   VMIN; // Min vertices
  int   VMAX; // Max vertices
  int   L;    // Lifespan
  int   SI;   // Spawn interval
  float SMIN; // Min speed
  float SMAX; // Max speed
};

struct BulletConfig {
  int   SR;   // Shape radius
  int   CR;   // Collision radius
  int   FR;   // Fill red
  int   FG;   // Fill green
  int   FB;   // Fill blue
  int   OR;   // Outline red
  int   OG;   // Outline green
  int   OB;   // Outline blue
  int   OT;   // Outline thickness
  int   V;    // Shape vertices
  int   L;    // Lifespan
  int   SI;   // Spawn interval
  float S;    // Speed
  float SMIN; // Min speed
  float SMAX; // Max speed
};
class ConfigReader {
private:
  std::string  m_path;
  PlayerConfig m_playerConfig;
  EnemyConfig  m_enemyConfig;
  BulletConfig m_bulletConfig;

public:
  ConfigReader(const std::string &path) :
      m_path(path), m_playerConfig(), m_enemyConfig(), m_bulletConfig() {}

  ConfigReader() :
      m_path("./config/config.txt"), m_playerConfig(), m_enemyConfig(), m_bulletConfig() {}

  void         read();
  PlayerConfig getPlayerConfig() const;
  EnemyConfig  getEnemyConfig() const;
  BulletConfig getBulletConfig() const;
};

#endif // CONFIG_READER_H