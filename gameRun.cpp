#include "gamePong.h"

void PongGame::run()
{
    // Initialize game objects
    leftPaddle =
    {
        {10, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT},
        0,
        0
    };

    rightPaddle =
    {
        {SCREEN_WIDTH - 10 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT},
        0,
        0
    };

    // Show the main menu first
    if (!showMainMenu())
        return;

    SDL_Event e;
    bool continueGame = true; // control main game loop

    resetBall((rand() % 2 == 0) ? 1 : -1);

    while (continueGame && !quit)
    {
        // Handle events
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                // Left paddle controls
                case SDLK_w:
                    leftPaddle.speed = -PADDLE_SPEED;
                    break;
                case SDLK_s:
                    leftPaddle.speed = +PADDLE_SPEED;
                    break;

                // Right paddle controls (only for player vs player mode)
                case SDLK_UP:
                    if (gameMode == PLAYER_VS_PLAYER)
                        rightPaddle.speed = -PADDLE_SPEED;
                    break;
                case SDLK_DOWN:
                    if (gameMode == PLAYER_VS_PLAYER)
                        rightPaddle.speed = +PADDLE_SPEED;
                    break;

                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
            else if (e.type == SDL_KEYUP && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                // Left paddle controls
                case SDLK_w:
                case SDLK_s:
                    leftPaddle.speed = 0;
                    break;

                // Right paddle controls
                case SDLK_UP:
                    if (gameMode == PLAYER_VS_PLAYER)
                        rightPaddle.speed = 0;
                    break;
                case SDLK_DOWN:
                    if (gameMode == PLAYER_VS_PLAYER)
                        rightPaddle.speed = 0;
                    break;
                }
            }
        }

        // Update game state
        updatePaddle(leftPaddle);

        if (gameMode == PLAYER_VS_PLAYER)
            updatePaddle(rightPaddle);
        else
            updateAI();

        updateBall();

        //End screen
        if ((leftPaddle.score >= MAX_SCORE || rightPaddle.score >= MAX_SCORE)
                && abs(leftPaddle.score - rightPaddle.score) >=2 )
            continueGame = showGameOverScreen(); //Show game over screen and get player's next choice

        renderGame();

        SDL_Delay(12);
    }
}
