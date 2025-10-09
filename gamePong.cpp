#include "gamePong.h"

PongGame::PongGame() :
    gWindow(nullptr),
    gRenderer(nullptr),
    gFont24(nullptr),
    gFont36(nullptr),
    gBackgroundTexture(nullptr),
    gMenuBackTexture(nullptr),
    gLeftPaddleTexture(nullptr),
    gRightPaddleTexture(nullptr),
    gBallTexture(nullptr),
    gHitSound(nullptr),
    gCountdownSound(nullptr),

    gameMode(PLAYER_VS_PLAYER),
    quit(false),
    ballResetTime(0),
    isPaused(false),
    isBallWaiting(false),
    ballResetDirection(0)

{
}

PongGame::~PongGame()
{
    close();
}

bool PongGame::initialize()
{
    return init() && loadMedia();
}


