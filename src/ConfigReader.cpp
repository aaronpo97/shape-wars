#include "../includes/ConfigReader.h"
#include <iostream>
void ConfigReader::read() {
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
}

PlayerConfig ConfigReader::getPlayerConfig() const {
  return m_playerConfig;
}

EnemyConfig ConfigReader::getEnemyConfig() const {
  return m_enemyConfig;
}

BulletConfig ConfigReader::getBulletConfig() const {
  return m_bulletConfig;
}
