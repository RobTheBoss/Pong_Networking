#include "UDPServer.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

UDPServer::UDPServer()
{
	game = new Game();
}

void UDPServer::InitGame()
{
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

	int initGame = game->initialize();
	if (initGame != 0) {
		std::cout << "Game could not be initialized!" << std::endl;
	}
}

void UDPServer::GameLoop()
{
    SDL_Event e; // Create an SDL event to handle events

   while(game->isRunning){
       Uint32 frameStart = SDL_GetTicks();

       while (SDL_PollEvent(&e) != 0) {
           if (e.type == SDL_QUIT) {
               game->isRunning = false; // Exit the loop if the window is closed
           }
           game->handleEvents(e); // Handle player input
       }

       game->update();
       game->render();

       Uint32 frameTime = SDL_GetTicks() - frameStart;
       if (frameTime < DELAY_TIME) {
           SDL_Delay(DELAY_TIME - frameTime);
       }
   }
}

void UDPServer::Cleanup()
{
    game->cleanup();

    SDL_Quit();
}
