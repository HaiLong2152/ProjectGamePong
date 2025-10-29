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
    ballResetDirection(0),

    player1Name("Player 1"),
    player2Name("Player 2"),
    isEnteringName(true),
    isPlayer1Turn(true),
    currentInput(""),


    maxScore(10),
    isMatchPointPause(false),
    matchPointPauseTime(0),
    isMatchPoint(false),
    matchPointPlayer(0)

{
}

PongGame::~PongGame()
{
}

bool PongGame::initialize()
{
    return init() && loadMedia();
}


