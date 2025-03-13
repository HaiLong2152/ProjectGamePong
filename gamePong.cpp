#include "gamePong.h"

PongGame::PongGame() :
    gWindow(nullptr),
    gRenderer(nullptr),
    gFont(nullptr),
    gBackgroundTexture(nullptr),
    gPaddleTexture(nullptr),
    gBallTexture(nullptr),
    gHitSound(nullptr),
    gScoreSound(nullptr),
    gameMode(PLAYER_VS_PLAYER),
    quit(false)
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

bool PongGame::init()
{
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cout << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr)
    {
        cout << "Không thể tạo cửa sổ! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Create renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr)
    {
        cout << "Không thể tạo renderer! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Init renderer color
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);

    // Init PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        cout << "SDL_image không thể khởi tạo! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    // Init SDL_ttf
    if (TTF_Init() == -1)
    {
        cout << "SDL_ttf không thể khởi tạo! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    // Init SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer không thể khởi tạo! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

bool PongGame::loadMedia()
{
    // Load font
    gFont = TTF_OpenFont("arialbi.ttf", 28);
    if (gFont == nullptr)
    {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;

    }

    // Load sounds
    gHitSound = Mix_LoadWAV("bounce.wav");
    if (gHitSound == nullptr)
    {
        cout << "Failed to load hit sound effect! SDL_mixer Error: " << Mix_GetError() << endl;

    }

    return true;
}

void PongGame::close()
{
    // Free loaded images
    if (gBackgroundTexture != nullptr)
    {
        SDL_DestroyTexture(gBackgroundTexture);
        gBackgroundTexture = nullptr;
    }
    if (gPaddleTexture != nullptr)
    {
        SDL_DestroyTexture(gPaddleTexture);
        gPaddleTexture = nullptr;
    }
    if (gBallTexture != nullptr)
    {
        SDL_DestroyTexture(gBallTexture);
        gBallTexture = nullptr;
    }

    // Free sounds
    if (gHitSound != nullptr)
    {
        Mix_FreeChunk(gHitSound);
        gHitSound = nullptr;
    }
    if (gScoreSound != nullptr)
    {
        Mix_FreeChunk(gScoreSound);
        gScoreSound = nullptr;
    }

    // Free font
    if (gFont != nullptr)
    {
        TTF_CloseFont(gFont);
        gFont = nullptr;
    }

    // Destroy window and renderer
    if (gRenderer != nullptr)
    {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = nullptr;
    }

    if (gWindow != nullptr)
    {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }

    // Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void PongGame::renderText(const string &text, int x, int y, SDL_Color color)
{
    if (gFont == nullptr)
    {
        return;
    }

    // Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
    if (textSurface == nullptr)
    {
        cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
        return;
    }

    // Create texture from surface pixels
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (textTexture == nullptr)
    {
        cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    // Get dimensions
    int width = textSurface->w;
    int height = textSurface->h;
    SDL_FreeSurface(textSurface);

    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, width, height};
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &renderQuad);

    // Free texture
    SDL_DestroyTexture(textTexture);
}

void PongGame::updatePaddle(Paddle &paddle)
{
    // Move paddle
    paddle.rect.y += paddle.speed;

    // Limit paddle movement to screen bounds
    if (paddle.rect.y < 0)
    {
        paddle.rect.y = 0;
    }
    else if (paddle.rect.y > SCREEN_HEIGHT - paddle.rect.h)
    {
        paddle.rect.y = SCREEN_HEIGHT - paddle.rect.h;
    }
}

void PongGame::updateAI()
{
    // Simple AI to follow the ball
    int AI_DIFFICUTY = 50;
    int paddleCenter = rightPaddle.rect.y + rightPaddle.rect.h / 2;
    int ballCenter = ball.rect.y + ball.rect.h / 2;
    int aimove_speed = AI_SPEED - (rand() % 6);
    // Only move if the ball is moving towards the AI paddle
    if (ball.speedX > 0)
    {
        // Add some difficulty by limiting AI reaction
        if (paddleCenter < ballCenter - AI_DIFFICUTY)
        {
            rightPaddle.rect.y += aimove_speed;
        }
        else if (paddleCenter > ballCenter + AI_DIFFICUTY)
        {
            rightPaddle.rect.y -= aimove_speed;
        }

        // Limit paddle movement to screen bounds
        if (rightPaddle.rect.y < 0)
        {
            rightPaddle.rect.y = 0;
        }
        else if (rightPaddle.rect.y > SCREEN_HEIGHT - rightPaddle.rect.h)
        {
            rightPaddle.rect.y = SCREEN_HEIGHT - rightPaddle.rect.h;
        }
    }
}

void PongGame::resetBall()
{

    ball.rect.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.rect.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.rect.w = BALL_SIZE;
    ball.rect.h = BALL_SIZE;

    ball.speedX = ((rand() % 2) == 0) ? BALL_MAX_SPEED : -BALL_MAX_SPEED;
    ball.speedY = ((rand() % 2) == 0) ? BALL_MAX_SPEED : -BALL_MAX_SPEED;
}

void PongGame::updateBall()
{
    // Move ball
    ball.rect.x += ball.speedX;
    ball.rect.y += ball.speedY;

    // Ball collision with top
    if (ball.rect.y <= 0)
    {
        ball.rect.y = 0;
        ball.speedY = -ball.speedY;
    }
    // Ball collision with bottom
    else if (ball.rect.y + ball.rect.h >= SCREEN_HEIGHT)
    {
        ball.rect.y = SCREEN_HEIGHT - ball.rect.h;
        ball.speedY = -ball.speedY;
    }

    // Ball collision with left paddle
    if (ball.speedX < 0 &&
            ball.rect.x < leftPaddle.rect.x + leftPaddle.rect.w &&
            ball.rect.x + ball.rect.w > leftPaddle.rect.x &&
            ball.rect.y < leftPaddle.rect.y + leftPaddle.rect.h &&
            ball.rect.y + ball.rect.h > leftPaddle.rect.y)
    {

        ball.speedX = -ball.speedX;
        //ball.speedY = randAngle();

        // Play hit sound
        if (gHitSound != nullptr)
        {
            Mix_PlayChannel(-1, gHitSound, 0);
        }
    }

    // Ball collision with right paddle
    if (ball.speedX > 0 &&
            ball.rect.x < rightPaddle.rect.x + rightPaddle.rect.w &&
            ball.rect.x + ball.rect.w > rightPaddle.rect.x &&
            ball.rect.y < rightPaddle.rect.y + rightPaddle.rect.h &&
            ball.rect.y + ball.rect.h > rightPaddle.rect.y)
    {

        ball.speedX = -ball.speedX;
        // Play hit sound
        if (gHitSound != nullptr) {
            Mix_PlayChannel(-1, gHitSound, 0);
        }
    }

    // Ball out of bounds (scoring)
    if (ball.rect.x < 0)
    {
        // Right player scores
        rightPaddle.score++;

        resetBall();
    }
    else if (ball.rect.x > SCREEN_WIDTH)
    {
        // Left player scores
        leftPaddle.score++;

        // Play score sound

        resetBall();
    }
}

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

    // Draw center line
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y += 10)
    {
        SDL_Rect centerLine = {SCREEN_WIDTH / 2 - 2, y, 4, 5};
        SDL_RenderFillRect(gRenderer, &centerLine);
    }

    // Draw paddles
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(gRenderer, &leftPaddle.rect);
    SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(gRenderer, &rightPaddle.rect);

    // Draw ball
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(gRenderer, &ball.rect);

    // Render scores
    SDL_Color textColor = {255, 255, 255, 255};
    stringstream leftScoreText;
    leftScoreText << leftPaddle.score;
    renderText(leftScoreText.str(), SCREEN_WIDTH / 4, 20, textColor);

    stringstream rightScoreText;
    rightScoreText << rightPaddle.score;
    renderText(rightScoreText.str(), 3 * SCREEN_WIDTH / 4, 20, textColor);

    // Render game mode
    string modeText = (gameMode == PLAYER_VS_PLAYER) ? "Player vs Player" : "Player vs AI";
    renderText(modeText, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 40, textColor);

    // Controls reminder
    renderText("Player 1:W/S", 20, SCREEN_HEIGHT - 40, textColor);

    if (gameMode == PLAYER_VS_PLAYER)
    {
        renderText("Player 2:UP/DOWN", SCREEN_WIDTH - 300, SCREEN_HEIGHT - 40, textColor);
    }

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

        // Render menu text
        SDL_Color textColor = {255, 255, 255, 255};
        renderText("PONG GAME", SCREEN_WIDTH / 2 - 100, 150, textColor);
        renderText("Press 1. Player vs Player", SCREEN_WIDTH / 2 - 120, 250, textColor);
        renderText("Press 2. Player vs AI", SCREEN_WIDTH / 2 - 120, 300, textColor);
        renderText("Press ESC to quit", SCREEN_WIDTH / 2 - 120, 400, textColor);

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

        // Render game over text
        SDL_Color textColor = {255, 255, 255, 255};
        string winnerText = leftPaddle.score >= MAX_SCORE ? "Player 1 Wins!" : "Player 2 Wins!";
        renderText(winnerText, SCREEN_WIDTH / 2 - 100, 150, textColor);
        renderText("Press R - Restart Game", SCREEN_WIDTH / 2 - 120, 250, textColor);
        renderText("press M - Back to Menu", SCREEN_WIDTH / 2 - 120, 300, textColor);
        renderText("press ESC - Quit", SCREEN_WIDTH / 2 - 120, 350, textColor);

        SDL_RenderPresent(gRenderer);
    }

    if (restartGame)
    {
        // Reset scores
        leftPaddle.score = 0;
        rightPaddle.score = 0;

        resetBall();
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

void PongGame::run()
{
    // Initialize game objects
    leftPaddle = { {50, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT}, 0, 0 };

    rightPaddle = { {SCREEN_WIDTH - 50 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT}, 0, 0};

    resetBall();

    // Show the main menu first
    if (!showMainMenu())
    {
        return;
    }

    // Main game loop
    SDL_Event e;
    bool continueGame = true;

    while (continueGame && !quit)
    {
        // Handle events
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
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
                    {
                        rightPaddle.speed = -PADDLE_SPEED;
                    }
                    break;
                case SDLK_DOWN:
                    if (gameMode == PLAYER_VS_PLAYER)
                    {
                        rightPaddle.speed = +PADDLE_SPEED;
                    }
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
                    {
                        rightPaddle.speed = 0;
                    }
                    break;
                case SDLK_DOWN:
                    if (gameMode == PLAYER_VS_PLAYER)
                    {
                        rightPaddle.speed = 0;
                    }
                    break;
                }
            }
        }

        // Update game state
        updatePaddle(leftPaddle);

        if (gameMode == PLAYER_VS_PLAYER)
        {
            updatePaddle(rightPaddle);
        }
        else
        {
            updateAI();
        }

        updateBall();

        // Check for game over
        if (leftPaddle.score >= MAX_SCORE || rightPaddle.score >= MAX_SCORE)
        {
            continueGame = showGameOverScreen();
        }

        // Render game
        renderGame();

        SDL_Delay(9);
    }
}

