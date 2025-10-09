#include "gamePong.h"

int main(int argc, char* args[])
{
    PongGame game;

    if (!game.initialize())
    {
        cout << "Failed to initialize game!" << endl;
        return -1;
    }

    game.run();

    return 0;
}
