#include "Game.h"
int main()
{
    // Init srand
    // std::cout << time(NULL) << std::endl;
    // std::srand(static_cast<unsigned>(time(NULL)));

    // Init Game engine
    // std::cout << "Step 1\n";
    Game game;
    // Game Loop
    while (game.running() && !game.getEndGame())
    {
        // std::cout << "Step 2\n";
        // update
        game.update();
        // std::cout << "Step 3\n";
        // render
        game.render();
    }
}