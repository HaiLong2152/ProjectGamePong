#include "gamePong.h"

int main(int argc, char* args[])
{
    PongGame game;

    if (!game.initialize())
    {
        std::cout << "Failed to initialize game!" << std::endl;
        return -1;
    }

    game.run();

    return 0;
}
