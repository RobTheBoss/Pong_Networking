#include <iostream>


//#include "SDL.h"
#include <SDL2/SDL.h>
#include "Game.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

int main(int argc, char* argv[]) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init() failed: " << TTF_GetError() << std::endl;
        // Handle initialization failure
        return 1;
    }

    Game game;

    while (true)
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
            break;  // Exit the loop if the user enters 'quit'
        }
        else if (strcmp(serverInput, "C") == 0) {
            printf("You are the client.\n");
            break;  // Exit the loop if the user enters 'quit'
        }
    }

    int initGame = game.initialize();
    if(initGame != 0){
        std::cout << "Game could not be initialized!" << std::endl;
        return 1;
    }

    SDL_Event e; // Create an SDL event to handle events

    while(game.isRunning){
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                game.isRunning = false; // Exit the loop if the window is closed
            }
            game.handleEvents(e); // Handle player input
        }

        game.update();
        game.render();

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY_TIME) {
            SDL_Delay(DELAY_TIME - frameTime);
        }
    }

    game.cleanup();

    SDL_Quit();

    //wepijfgpoiwerugfaeruipgolhengiluv
}