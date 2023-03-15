#include "Game.hpp"
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