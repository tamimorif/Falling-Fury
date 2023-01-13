#include "Game.h"
int main()
{
    // Init srand
    // std::cout << time(NULL) << std::endl;
    // std::srand(static_cast<unsigned>(time(NULL)));

    // Init Game engine
    Game game;

    // Game Loop
    while (game.running() && !game.getEndGame())
    {
        // update
        game.update();
        // render
        game.render();
    }
}