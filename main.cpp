#include "Game.h"
#include <cstdlib>

int main() {
  // Init srand
  std::srand(static_cast<unsigned>(time(NULL)));

  Game game;
  // Game Loop
  while (game.running()) {
    // update
    game.update();
    // render
    game.render();
  }
}
