#include <iostream>


//#include "SDL.h"
#include <SDL2/SDL.h>
#include "Game.h"
#include "NetworkManager.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

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
    bool isHost = false;
    isHost = Setup();

    NetworkManager* networkManager;

    networkManager = new NetworkManager(isHost);
    networkManager->InitGame();
    networkManager->GameLoop();
    networkManager->Cleanup();

    return 1;
}