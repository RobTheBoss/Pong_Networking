#include <iostream>


//#include "SDL.h"
#include <SDL2/SDL.h>
#include "Game.h"
#include "UDPServer.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

int main(int argc, char* argv[]) {

    bool isHost = false;

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
            isHost = true;
            printf("You are the host.\n");
            break;  // Exit the loop upon valid answer
        }
        else if (strcmp(serverInput, "C") == 0) {
            isHost = false;
            printf("You are the client.\n");
            break;  // Exit the loop upon valid answer
        }
    }

    if (isHost)
    {
        UDPServer* server = new UDPServer();
        server->InitGame();
        server->GameLoop();
        server->Cleanup();
    }

    return 1;
}



