#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

const int GRID_SIZE = 10;
int CELL_SIZE = 40;

int SCREEN_WIDTH = 900;
int SCREEN_HEIGHT = 500;

Mix_Chunk* testSound = nullptr;

bool init(SDL_Window*& window, SDL_Renderer*& renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("SDL Mixer Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    testSound = Mix_LoadWAV("gameover.mp3");
    if (!testSound)
    {
        cout << "Failed to load test.wav! Mix_Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

void playTestSound()
{
    if (testSound)
    {
        Mix_PlayChannel(-1, testSound, 0);
    }
}

int main(int argc, char* argv[])
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!init(window, renderer)) return 1;

    bool running = true;

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_SPACE)
                {
                    playTestSound();
                }
            }
        }
    }

    Mix_FreeChunk(testSound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
