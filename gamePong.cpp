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

    // Init PNG
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
