#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 635;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;
const int BALL_SIZE = 30;
const int PADDLE_SPEED = 10;
const int BALL_MIN_SPEED = 8;
const int BALL_MAX_SPEED = 11;
const int AI_SPEED = 10;
const int MAX_SCORE = 10;



enum GameMode
{
    PLAYER_VS_PLAYER,
    PLAYER_VS_AI
};

struct Paddle
{
    SDL_Rect rect;
    int score;
    int speed;
};

struct Ball
{
    SDL_Rect rect;
    int speedX;
    int speedY;
};

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


    GameMode gameMode;
    Paddle leftPaddle;
    Paddle rightPaddle;
    Ball ball;
    bool quit;

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

public:
    PongGame();
    ~PongGame();
    bool initialize();
    void run();
};

#endif // GAME_H
