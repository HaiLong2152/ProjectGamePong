#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <deque>


#include "consts.h"
#include "gameStructs.h"

using namespace std;

class PongGame
{
private:

    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    TTF_Font* gFont24;
    TTF_Font* gFont36;
    SDL_Texture* gBackgroundTexture;
    SDL_Texture* gMenuBackTexture;
    SDL_Texture* gLeftPaddleTexture;
    SDL_Texture* gRightPaddleTexture;
    SDL_Texture* gBallTexture;
    Mix_Chunk* gHitSound;
    Mix_Chunk* gScoreSound;
    Mix_Chunk* gCountdownSound;

    GameMode gameMode;
    Paddle leftPaddle;
    Paddle rightPaddle;
    Ball ball;
    bool quit;

    Uint32 ballResetTime;
    bool isPaused;
    bool isBallWaiting;
    int ballResetDirection;

    string player1Name;
    string player2Name;

    // Trạng thái nhập tên
    bool isEnteringName;
    bool isPlayer1Turn;  // true = đang nhập P1, false = P2
    string currentInput; // Chuỗi đang gõ

    deque<SDL_Rect> ballTrail;
    int maxScore;
    bool isMatchPoint;
    int matchPointPlayer;        // 1=left, 2=right
    bool isMatchPointPause;  // Dừng game khi match point
    Uint32 matchPointPauseTime;  // Thời gian bắt đầu pause
    const Uint32 MATCH_POINT_PAUSE = 1500;

    Uint32 gameStartTime;
    int leftHits;
    int rightHits;
    int currentRally;
    int longestRally;

    bool init();
    bool loadMedia();
    void close();

    void renderText(const string &text, int x, int y, SDL_Color color, TTF_Font* gFont);

    void resetBall(int Direct);
    void updatePaddle(Paddle &paddle);
    void updateAI();
    void updateBall();
    void renderGame();

    bool showMainMenu();
    bool showGameOverScreen();
    bool showNameEntryScreen();
    bool showConfigScreen();

public:
    PongGame();
    ~PongGame();
    bool initialize();
    void run();
};

#endif // GAME_H
