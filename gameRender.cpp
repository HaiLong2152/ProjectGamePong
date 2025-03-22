#include "gamePong.h"

void PongGame::renderGame()
{
    // Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
    SDL_RenderClear(gRenderer);

    // Render background
    if (gBackgroundTexture != nullptr)
    {
        SDL_RenderCopy(gRenderer, gBackgroundTexture, nullptr, nullptr);
    }

    // Render paddles
    if (gLeftPaddleTexture != nullptr)
    {
        SDL_RenderCopy(gRenderer, gLeftPaddleTexture, nullptr, &leftPaddle.rect);
    }

    if (gRightPaddleTexture != nullptr)
    {
        SDL_RenderCopy(gRenderer, gRightPaddleTexture, nullptr, &rightPaddle.rect);
    }

    // Render ball
    if (gBallTexture != nullptr)
    {
        SDL_RenderCopy(gRenderer, gBallTexture, nullptr, &ball.rect);
    }

    // Render scores
    SDL_Color textColorBlack = {0, 0, 0, 0};
    stringstream leftScoreText;
    leftScoreText << leftPaddle.score;
    renderText(leftScoreText.str(), SCREEN_WIDTH / 4, 25, {0, 0, 255, 255}, gFont36);

    stringstream rightScoreText;
    rightScoreText << rightPaddle.score;
    renderText(rightScoreText.str(), 3 * SCREEN_WIDTH / 4, 25, {0, 255, 0, 255}, gFont36);

    // Render game mode
    string modeText = (gameMode == PLAYER_VS_PLAYER) ? "Player vs Player" : "Player vs AI";
    renderText(modeText, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 20, textColorBlack, gFont24);

    // Controls tutorial
    if (gameMode == PLAYER_VS_PLAYER)
    {
        renderText("Player 1: W/S", 70, SCREEN_HEIGHT - 20, textColorBlack, gFont24);
        renderText("Player 2: UP/DOWN", SCREEN_WIDTH - 250, SCREEN_HEIGHT - 20, textColorBlack, gFont24);
    }
    else renderText("W/S", 70, SCREEN_HEIGHT - 20, textColorBlack, gFont24);

    // Update screen
    SDL_RenderPresent(gRenderer);
}

bool PongGame::showMainMenu()
{
    bool modeSelected = false;
    SDL_Event e;

    while (!modeSelected && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_1:
                    gameMode = PLAYER_VS_PLAYER;
                    modeSelected = true;
                    break;
                case SDLK_2:
                    gameMode = PLAYER_VS_AI;
                    modeSelected = true;
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        if (gMenuBackTexture != nullptr)
        {
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);
        }

        // Render menu text
        SDL_Color textColor = {255, 255, 255, 255};
        renderText("PONG GAME", SCREEN_WIDTH / 2 - 100, 150, {255, 100, 255, 255}, gFont36);
        renderText("Press 1. Player vs Player", SCREEN_WIDTH / 2 - 120, 250, textColor, gFont36);
        renderText("Press 2. Player vs AI", SCREEN_WIDTH / 2 - 120, 300, textColor, gFont36);
        renderText("Press ESC to quit", SCREEN_WIDTH / 2 - 120, 400, textColor, gFont36);

        SDL_RenderPresent(gRenderer);
    }

    return modeSelected;
}

bool PongGame::showGameOverScreen()
{
    bool restartGame = false;
    bool backToMenu = false;
    SDL_Event e;

    while (!restartGame && !backToMenu && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_r:
                    restartGame = true;
                    break;
                case SDLK_m:
                    backToMenu = true;
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        if (gMenuBackTexture != nullptr)
        {
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);
        }

        // Render game over text
        SDL_Color textColor = {255, 255, 255, 255};

        if(gameMode == PLAYER_VS_PLAYER)
        {
            if(leftPaddle.score >= MAX_SCORE)
                renderText("Player 1 Wins!", SCREEN_WIDTH / 2 - 100, 150, {0, 0, 255, 255}, gFont36);
            else  renderText("Player 2 Wins!", SCREEN_WIDTH / 2 - 100, 150, {255, 0, 0, 255}, gFont36);
        }
        else
        {
            if(leftPaddle.score >= MAX_SCORE)
                renderText("WIN!", SCREEN_WIDTH / 2 - 30, 150, {0, 0, 255, 255}, gFont36);
            else  renderText("LOSE!", SCREEN_WIDTH / 2 - 30, 150, {255, 0, 0, 255}, gFont36);
        }

        renderText("Press R - Restart Game", SCREEN_WIDTH / 2 - 120, 250, textColor, gFont36);
        renderText("press M - Back to Menu", SCREEN_WIDTH / 2 - 120, 300, textColor, gFont36);
        renderText("press ESC - Quit", SCREEN_WIDTH / 2 - 120, 350, textColor, gFont36);

        SDL_RenderPresent(gRenderer);
    }

    if (restartGame)
    {
        // Reset scores
        leftPaddle.score = 0;
        rightPaddle.score = 0;

        resetBall((rand() % 2 == 0) ? 1 : -1);
        return true;
    }

    if (backToMenu)
    {
        // Reset scores
        leftPaddle.score = 0;
        rightPaddle.score = 0;
        return showMainMenu();
    }

    return false;
}
