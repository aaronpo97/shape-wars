#include "../includes/SoundManager.h"

SoundManager::SoundManager() {
  loadSounds();
}

void SoundManager::loadSounds() {
  shootBuffer.loadFromFile("../assets/sounds/shoot.wav");
  enemyHit.loadFromFile("../assets/sounds/enemy-hit.wav");
  specialWeaponBuffer.loadFromFile("../assets/sounds/special-weapon.wav");
  playerHitBuffer.loadFromFile("../assets/sounds/player-hit.wav");
}

void SoundManager::playSound(SoundType soundType) {
  switch (soundType) {
    case SoundType::SHOOT: {
      shootSound.setBuffer(shootBuffer);
      shootSound.play();
      break;
    }
    case SoundType::ENEMY_SHOOT: {
      enemyShootSound.setBuffer(enemyHit);
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