#include "../includes/Game.h"
#include "../tests/Vec2/Vec2.test.cpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

int main() {

  Game game("config.txt");

  game.run();
  return 0;
}