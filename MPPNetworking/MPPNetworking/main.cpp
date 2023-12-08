#include <iostream>


//#include "SDL.h"
#include <SDL2/SDL.h>
#include "Game.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

void InitGame(Game* game_) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized!" << std::endl
            << "SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init() failed: " << TTF_GetError() << std::endl;
        // Handle initialization failure
        return;
    }

    int initGame = game_->initialize();
    if (initGame != 0) {
        std::cout << "Game could not be initialized!" << std::endl;
    }
}

void GameLoop(Game* game_)
{
    SDL_Event e; // Create an SDL event to handle events

    while (game_->isRunning) {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                game_->isRunning = false; // Exit the loop if the window is closed
            }
            game_->handleEvents(e); // Handle player input
        }

        game_->update();
        game_->render();

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY_TIME) {
            SDL_Delay(DELAY_TIME - frameTime);
        }
    }
}

void Cleanup(Game* game_)
{
    game_->cleanup();

    SDL_Quit();
}

bool Setup() {
    while (1)
    {
        char serverInput[1024];
        memset(serverInput, 0, sizeof(serverInput));

        // Read user input
        if (fgets(serverInput, sizeof(serverInput), stdin) == NULL) {
            break;  // Exit the loop on EOF or error
        }

        // Remove newline character at the end of the input
        size_t len = strlen(serverInput);
        if (len > 0 && serverInput[len - 1] == '\n') {
            serverInput[len - 1] = '\0';
        }

        if (strcmp(serverInput, "H") == 0) {
            printf("You are the host.\n");
            return true;
            break;  // Exit the loop upon valid answer
        }
        else if (strcmp(serverInput, "C") == 0) {
            printf("You are the client.\n");
            return false;
            break;  // Exit the loop upon valid answer
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    bool isHost;
    isHost = Setup();

    Game* game = new Game(isHost);;

    InitGame(game);
    GameLoop(game);
    Cleanup(game);

    return 1;
}