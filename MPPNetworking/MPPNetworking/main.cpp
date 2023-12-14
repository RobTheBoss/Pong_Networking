#include <iostream>


//#include "SDL.h"
#include <SDL2/SDL.h>
#include "Game.h"
#include "ScoreKeeping.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

void StartGame(Game* game_) {
    int initGame = game_->startGame();
    if (initGame != 0) {
        std::cout << "Game could not be initialized!" << std::endl;
    }
}

bool Setup(bool& isHost_, bool& isScoreKeeper) {

    std::cout << "H for host, C for client, S for Scorekeeper" << std::endl;
    while (1)
    {
        char Input[1024];
        memset(Input, 0, sizeof(Input));

        // Read user input
        if (fgets(Input, sizeof(Input), stdin) == NULL) {
            break;  // Exit the loop on EOF or error
        }

        // Remove newline character at the end of the input
        size_t len = strlen(Input);
        if (len > 0 && Input[len - 1] == '\n') {
            Input[len - 1] = '\0';
        }

        if (strcmp(Input, "H") == 0) {
            printf("You are the host.\n");
            isHost_ = true;
            break;  // Exit the loop upon valid answer
        }
        else if (strcmp(Input, "C") == 0) {
            printf("You are the client.\n");
            isHost_ = false;
            break;  // Exit the loop upon valid answer
        }
        else if (strcmp(Input, "S") == 0) {
            printf("Scorekeeper.\n");
            isHost_ = true;
            isScoreKeeper = true;
            break;  // Exit the loop upon valid answer
        }
    }
}

int main(int argc, char* argv[]) {
    bool isHost = false;
    bool isScoreKeeper = false;
    Setup(isHost, isScoreKeeper);

    if (!isScoreKeeper)
    {
        Game* game = new Game(isHost);;
        StartGame(game);
    }
    else
    {
        ScoreKeeping* scoreKeeper = new ScoreKeeping();
        scoreKeeper->init(NULL, 8080);
    }

    return 1;
}