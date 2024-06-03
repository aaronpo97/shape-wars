#include "../includes/Game.h"
#include "../tests/Vec2/Vec2.test.cpp"
#include <fstream>
#include <iostream>
#include <sfml/Graphics.hpp>

int main() {

  Game game("config.txt");

  game.run();
  return 0;
}