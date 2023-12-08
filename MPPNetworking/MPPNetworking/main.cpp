#include <iostream>


//#include "SDL.h"
#include <SDL2/SDL.h>
#include "Game.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

void StartGame(Game* game_) {
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

    int initGame = game_->startGame();
    if (initGame != 0) {
        std::cout << "Game could not be initialized!" << std::endl;
    }
}

bool Setup() {

    std::cout << "H for host, C for client" << std::endl;
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

    StartGame(game);

    return 1;
}