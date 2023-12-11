#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string>
#include <iostream>
#include <thread>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

const int TARGET_FPS = 60;
const int DELAY_TIME = 1000 / TARGET_FPS;

Ball ball(Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT);

Game::Game(bool isHost_) : window(nullptr), renderer(nullptr), isRunning(false) {
    isHost = isHost_;
}

int Game::init(UDPsocket& udpSocket, IPaddress& serverIP, int port_)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return 1;
    }

    //Create window
    window = SDL_CreateWindow("Pong in SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created!" << std::endl
            << "SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    //Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created!" << std::endl
            << "SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init() failed: " << TTF_GetError() << std::endl;
        // Handle initialization failure
        return 1;
    }

    // Initialize SDL Network
    if (SDLNet_Init() < 0) {
        SDL_Log("SDLNet initialization failed: %s", SDLNet_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (isHost)
        udpSocket = SDLNet_UDP_Open(port_);
    else
        udpSocket = SDLNet_UDP_Open(0);

    if (!udpSocket) {
        SDL_Log("SDLNet_UDP_Open error: %s", SDLNet_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDLNet_Quit();
        SDL_Quit();
        return 1;
    }

    if (!isHost)
    {
        if (SDLNet_ResolveHost(&serverIP, "127.0.0.1", port_) < 0) {
            std::cerr << "SDLNet_ResolveHost error: " << SDLNet_GetError() << std::endl;
            SDLNet_UDP_Close(udpSocket);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    //Load font
    std::string FontPath = "resources/arial.ttf";
    font = TTF_OpenFont(FontPath.c_str(), 50); //Debug mode path

    if (!font)
    {
        FontPath = SDL_GetBasePath() + FontPath;
        font = TTF_OpenFont(FontPath.c_str(), 50); //Exe mode path

        if (!font) {
            std::cout << "TTF_OpenFont() failed: " << TTF_GetError() << std::endl;
            return 1;
        }
    }

    SDL_Color textColor = { 0, 0, 0, 0xFF }; // Adjust the text color as needed
    textSurface = TTF_RenderText_Solid(font, "PONG IN SDL2!", textColor);
    if (!textSurface) {
        std::cout << "TTF_RenderText_Solid() failed: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Create text texture
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cout << "SDL_CreateTextureFromSurface() failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Calculate the centered position for the text horizontally
    int textWidth = textSurface->w;
    int centerX = (SCREEN_WIDTH - textWidth) / 2;
    textRect = { centerX, 0, textWidth, textSurface->h };

    return 0;
}

int Game::startGame() {
    UDPsocket udpSocket;
    IPaddress serverIP;
    int port = 8080;

    init(udpSocket, serverIP, port);

    bool quitApplication = false;

    std::thread sendThread(&Game::SendData, this, udpSocket, serverIP, port, std::ref(quitApplication));
    std::thread receiveThread(&Game::ReceiveData, this, udpSocket, serverIP, port, std::ref(quitApplication));

    if (isHost)
        isRunning = false; //Host starts with game off until client joins
    else
        isRunning = true; //Game starts as soon as client joins so client
                          //should have game running by default


    player1.innit(true);
    player2.innit(false);
    ball.innit();

    SDL_Event e; // Create an SDL event to handle events

    while (!quitApplication)
    {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            handleEvents(e); // Handle player input
            if (e.type == SDL_QUIT) {
                isRunning = false; // Exit the loop if the window is closed
                quitApplication = true;
            }
        }

        render();

        if (!isRunning)
            continue;

        update();

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY_TIME) {
            SDL_Delay(DELAY_TIME - frameTime);
        }
    }

    sendThread.join();
    receiveThread.join();

    cleanup();

    return 0;
}
void Game::handleEvents(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            if (isHost)
            { 
                case SDLK_0:
                    isRunning = true;
                    break;
                case SDLK_UP:
                    if (isHost)
                        player1.move(true);
                    else
                        player2.move(true);
                    break;
                case SDLK_DOWN:
                    if (isHost)
                        player1.move(false);
                    else
                        player2.move(false);
                    break;
            }
        }
    } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (isHost)
                    player1.stopMoving();
                else
                    player2.stopMoving();
                break;
            case SDLK_DOWN:
                if (isHost)
                    player1.stopMoving();
                else
                    player2.stopMoving();
                break;
        }
    }
}
void Game::update() {
    // Update game objects (e.g., player, ball, background)
    player1.update();
    player2.update();

    //Only check for collisions
    //And update ball position/velocity on host
    if (isHost)
    {
        if (checkForPaddleCollision(player1.getPaddle())) {
            int curXVelocity = ball.getVelocity().x;
            ball.setVelocity({ abs(curXVelocity), ball.getVelocity().y });
        }
        else if (checkForPaddleCollision(player2.getPaddle())) {
            int curXVelocity = ball.getVelocity().x;
            ball.setVelocity({ -abs(curXVelocity), ball.getVelocity().y });
        }
        else {
            checkForWallCollision();
        }

        ball.update();
    }
}
void Game::render() {
    // Initialize renderer color black for the background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);

    if (isHost && !isRunning)
        renderStartText();
    
    player1.render(renderer);
    player2.render(renderer);
    ball.render(renderer);
    renderText();
    
    // Update screen
    SDL_RenderPresent(renderer);
}
void Game::cleanup() {
    SDLNet_FreePacket(receivePacket);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    TTF_Quit();
}

bool Game::checkForPaddleCollision(SDL_Rect paddle) {
    //if (!isHost) return false;

    if (ball.getBall().x + ball.getBall().w >= paddle.x &&
        ball.getBall().x <= paddle.x + paddle.w &&
        ball.getBall().y + ball.getBall().h >= paddle.y &&
        ball.getBall().y <= paddle.y + paddle.h) {
        return true;
    }
    return false;
}
void Game::checkForWallCollision() {
    //if (!isHost) return;

    // Check for collision with the top and bottom of the screen
    if (ball.getBall().y <= 0 || ball.getBall().y + ball.getBall().h >= SCREEN_HEIGHT) {
        int curYVelocity = ball.getVelocity().y;
        ball.setVelocity({ball.getVelocity().x, -curYVelocity});
    }

    // Check for collision with the left and right sides of the screen
    if (ball.getBall().x + ball.getBall().w <= 0) {
        // Ball passed the left side of the screen
        // Add points and reset ball position
        int score = Game::getPlayer2Score();
        Game::setPlayer2Score(score + 1); // Add points to player B (or your scoring mechanism)
        ball.innit();   // Reset the ball's position to the center
    } else if (ball.getBall().x >= SCREEN_WIDTH) {
        // Ball passed the right side of the screen
        // Add points and reset ball position
        int score = Game::getPlayer1Score();
        Game::setPlayer1Score(score + 1);
        //addPointsToPlayerA();  // Add points to player A (or your scoring mechanism)
        ball.innit();    // Reset the ball's position to the center
    }
}

void Game::SendData(UDPsocket udpSocket_, IPaddress ip_, int port_, bool& quit)
{
    if (isHost) ///SERVER
    {
        std::cout << "UDP server is running and listening on port " << port_ << std::endl;

        while (!quit) {
            auto msg_data = serializeData();

            // SEND
            UDPpacket* packet = SDLNet_AllocPacket(msg_data.size() + 1); // +1 for null terminator
            if (!packet) {
                SDL_Log("SDLNet_AllocPacket error: %s", SDLNet_GetError());
                SDLNet_UDP_Close(udpSocket_);
                SDL_Quit();
                return;
            }

            packet->address = receivePacket->address;
            packet->len = msg_data.size();

           
            memcpy(packet->data, msg_data.c_str(), msg_data.size());

            if (SDLNet_UDP_Send(udpSocket_, -1, packet) == 0) {
                std::cerr << "SDLNet_UDP_Send error: " << SDLNet_GetError() << std::endl;
            }
            
            SDLNet_FreePacket(packet);
            SDL_Delay(16); // Cap the sending rate to approximately 60 FPS
        }
    }

    else ///CLIENT
    { 
        while (!quit) {
            std::string msg_data = serializeData();

            // Create a UDP packet to send the position to the server
            UDPpacket* packet = SDLNet_AllocPacket(msg_data.size() + 1);
            if (packet) {
                packet->address = ip_;
                packet->len = msg_data.size();
                memcpy(packet->data, msg_data.c_str(), msg_data.size());

                if (SDLNet_UDP_Send(udpSocket_, -1, packet) == 0) {
                    std::cerr << "SDLNet_UDP_Send error: " << SDLNet_GetError() << std::endl;
                }
            }

            SDLNet_FreePacket(packet);
            SDL_Delay(16); // Cap the sending rate to approximately 60 FPS
        }
    }
}
void Game::ReceiveData(UDPsocket udpSocket_, IPaddress ip_, int port_, bool& quit)
{
    receivePacket = SDLNet_AllocPacket(1024);

    if (!receivePacket) {
        SDL_Log("SDLNet_AllocPacket error: %s", SDLNet_GetError());
        SDLNet_UDP_Close(udpSocket_);
        SDL_Quit();
        return;
    }

    if (isHost) ///SERVER
    {
        while (!quit) {
            if (SDLNet_UDP_Recv(udpSocket_, receivePacket)) {
                isRunning = true;
                IPaddress clientAddress = receivePacket->address;
                Uint16 clientPort = clientAddress.port;
                const char* clientHost = SDLNet_ResolveIP(&clientAddress);

                std::cout << "Received from client " << clientHost << ":" << clientPort << ": " << receivePacket->data << std::endl;

                //store packet data in receive buffer
                char* receiveBuffer = reinterpret_cast<char*>(receivePacket->data);
                receiveBuffer[receivePacket->len] = '\0'; //null terminated to prevent crash in deserializer

                deserializeData(std::string(receiveBuffer));
            }
        }
    }

    else ///CLIENT
    {
        while (!quit) {
            // Check for incoming UDP packets (data from the server)
            if (SDLNet_UDP_Recv(udpSocket_, receivePacket)) {
                std::string receiveBuffer = reinterpret_cast<char*>(receivePacket->data);
                receiveBuffer[receivePacket->len] = '\0';

                deserializeData(std::string(receiveBuffer));
            }
        }
    }

    SDLNet_FreePacket(receivePacket);
}

std::string Game::serializeData()
{
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("ballX");
    writer.Int(ball.getBall().x);
    writer.Key("ballY");
    writer.Int(ball.getBall().y);
    writer.Key("playerY");
    if (isHost)
        writer.Int(player1.getPaddle().y);
    else
        writer.Int(player2.getPaddle().y);

    writer.EndObject();

    return s.GetString();
}
bool Game::deserializeData(const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());

    if (!doc.IsObject()) {
        return false;
    }

    const rapidjson::Value& ballXVal = doc["ballX"];
    const rapidjson::Value& ballYVal = doc["ballY"];
    const rapidjson::Value& playerYVal = doc["playerY"];

    if (!ballXVal.IsInt() || !ballYVal.IsInt() || !playerYVal.IsInt()) {
        return false; //keeps on returning false when parsing
    }

    if (!isHost)
        ball.setBallPos(ballXVal.GetInt(), ballYVal.GetInt());

    if (isHost)
        player2.setPosY(playerYVal.GetInt());
    else
        player1.setPosY(playerYVal.GetInt());

    return true;
}

void Game::renderText() {
    // Convert player scores to strings
    std::string player1ScoreStr =  std::to_string(getPlayer1Score());
    std::string player2ScoreStr =  std::to_string(getPlayer2Score());

    // Combine the scores with the main text
    std::string combinedText = player1ScoreStr + " : " + player2ScoreStr;

    // Create a new text surface with the combined text
    SDL_Surface* newTextSurface = TTF_RenderText_Solid(font, combinedText.c_str(), { 0xFF, 0xFF, 0xFF, 0xFF});
    if (!newTextSurface) {
        std::cout << "TTF_RenderText_Solid() failed: " << TTF_GetError() << std::endl;
        return;
    }

    // Create a new texture from the updated surface
    SDL_Texture* newTextTexture = SDL_CreateTextureFromSurface(renderer, newTextSurface);
    if (!newTextTexture) {
        std::cout << "SDL_CreateTextureFromSurface() failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(newTextSurface);
        return;
    }

    // Set the position for the updated text
    int textWidth = newTextSurface->w;
    int centerX = (SCREEN_WIDTH - textWidth) / 2;
    textRect = {centerX, 0, textWidth, newTextSurface->h};

    // Render the updated text
    SDL_RenderCopy(renderer, newTextTexture, NULL, &textRect);

    // Clean up the old text surface and texture
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Update the text surface and texture
    textSurface = newTextSurface;
    textTexture = newTextTexture;
}
void Game::renderStartText()
{
    // Create a new text surface with the desired text
    SDL_Surface* startTextSurface = TTF_RenderText_Solid(font, "Waiting for player...", {0xFF, 0xFF, 0xFF, 0xFF});
    if (!startTextSurface) {
        std::cout << "TTF_RenderText_Solid() failed: " << TTF_GetError() << std::endl;
        return;
    }

    // Create a new texture from the text surface
    SDL_Texture* startTextTexture = SDL_CreateTextureFromSurface(renderer, startTextSurface);
    if (!startTextTexture) {
        std::cout << "SDL_CreateTextureFromSurface() failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(startTextSurface);
        return;
    }

    // Set the position for the start text (centered horizontally and vertically)
    int textWidth = startTextSurface->w;
    int textHeight = startTextSurface->h;
    int centerX = (SCREEN_WIDTH - textWidth) / 2;
    int centerY = (SCREEN_HEIGHT - textHeight) / 2;
    SDL_Rect startTextRect = {centerX, centerY, textWidth, textHeight};

    // Render the start text
    SDL_RenderCopy(renderer, startTextTexture, NULL, &startTextRect);

    // Clean up the text surface and texture
    SDL_FreeSurface(startTextSurface);
    SDL_DestroyTexture(startTextTexture);
}
