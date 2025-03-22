#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

#include <SDL.h>

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

#endif // GAMESTRUCTS_H
