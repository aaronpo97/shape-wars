#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "SoundHelpers.h"
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>

enum class SoundType { SHOOT, ENEMY_SHOOT, SPECIAL_WEAPON, PLAYER_HIT };

class SoundManager {
public:
  SoundManager();

  sf::SoundBuffer shootBuffer;
  sf::Sound       shootSound;
  sf::SoundBuffer enemyHit;
  sf::Sound       enemyShootSound;
  sf::SoundBuffer specialWeaponBuffer;
  sf::Sound       specialWeaponSound;
  sf::SoundBuffer playerHitBuffer;
  sf::Sound       playerHitSound;

  void loadSounds();
  void playSound(SoundType soundType);
};

#endif // SOUND_MANAGER_H
