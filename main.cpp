#include "Game.h"
int main()
{
    // Init Game engine
    Game game;
    // Game Loop
    while (game.running() && !game.getEndGame())
    {
        game.update();
        // render
        game.render();
    }
}