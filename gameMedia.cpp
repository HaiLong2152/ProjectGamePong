#include "gamePong.h"

bool PongGame::loadMedia()
{
    bool success = true;

    // Load font
    gFont24 = TTF_OpenFont("VHMUSTI.ttf", 24);
    if (gFont24 == nullptr)
    {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
        success = false;
    }

    gFont36 = TTF_OpenFont("VHMUSTI.ttf", 36);
    if (gFont36 == nullptr)
    {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
        success = false;
    }

    // Load background image
    gBackgroundTexture = IMG_LoadTexture(gRenderer, "hookeyfield.png");
    if (gBackgroundTexture == nullptr)
    {
        cout << "Failed to load background texture! SDL_image Error: " << IMG_GetError() << endl;
        success = false;
    }
    // Load background menu & end
    gMenuBackTexture = IMG_LoadTexture(gRenderer, "backgroundhockey.png");
    if (gMenuBackTexture == nullptr)
    {
        cout << "Failed to load paddle texture! SDL_image Error: " << IMG_GetError() << endl;
        success = false;
    }
    // Load paddle images
    gLeftPaddleTexture = IMG_LoadTexture(gRenderer, "paddleblue.png");
    if (gLeftPaddleTexture == nullptr)
    {
        cout << "Failed to load paddle texture! SDL_image Error: " << IMG_GetError() << endl;
        success = false;
    }

    gRightPaddleTexture = IMG_LoadTexture(gRenderer, "paddlegreen.png");
    if (gRightPaddleTexture == nullptr)
    {
        cout << "Failed to load paddle texture! SDL_image Error: " << IMG_GetError() << endl;
        success = false;
    }

    // Load ball image
    gBallTexture = IMG_LoadTexture(gRenderer, "puck.png");
    if (gBallTexture == nullptr)
    {
        cout << "Failed to load ball texture! SDL_image Error: " << IMG_GetError() << endl;
        success = false;
    }

    // Load sounds
    gHitSound = Mix_LoadWAV("bounce.wav");
    if (gHitSound == nullptr)
    {
        cout << "Failed to load hit sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        success = false;
    }

    //gScoreSound = Mix_LoadWAV("score.wav");
    // if (gScoreSound == nullptr)
    // {
    //    cout << "Failed to load score sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
    //}

    return success;
}

void PongGame::close()
{
    if (gBackgroundTexture != nullptr)
    {
        SDL_DestroyTexture(gBackgroundTexture);
        gBackgroundTexture = nullptr;
    }
    if (gMenuBackTexture != nullptr)
    {
        SDL_DestroyTexture(gMenuBackTexture);
        gMenuBackTexture = nullptr;
    }
    if (gLeftPaddleTexture != nullptr)
    {
        SDL_DestroyTexture(gLeftPaddleTexture);
        gLeftPaddleTexture = nullptr;
    }
    if (gRightPaddleTexture != nullptr)
    {
        SDL_DestroyTexture(gRightPaddleTexture);
        gRightPaddleTexture = nullptr;
    }
    if (gBallTexture != nullptr)
    {
        SDL_DestroyTexture(gBallTexture);
        gBallTexture = nullptr;
    }

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

    if (gFont24 != nullptr)
    {
        TTF_CloseFont(gFont24);
        gFont24 = nullptr;
    }

    if (gFont36 != nullptr)
    {
        TTF_CloseFont(gFont36);
        gFont36 = nullptr;
    }

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

    // Quit SDL
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void PongGame::renderText(const string &text, int x, int y, SDL_Color color, TTF_Font* gFont)
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

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (textTexture == nullptr)
    {
        cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    int width = textSurface->w;
    int height = textSurface->h;
    SDL_FreeSurface(textSurface);

    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, width, height};
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &renderQuad);

    // Free texture
    SDL_DestroyTexture(textTexture);
}
