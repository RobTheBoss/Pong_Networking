#include "ScoreKeeping.h"
#include <stdio.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

void ScoreKeeping::init(const char* host, Uint16 port)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, host, port);

    server = SDLNet_TCP_Open(&ip);

    bool quit = false;
    //std::thread sendThread(&ScoreKeeping::sendData, this, server, std::ref(quit));
    std::thread receiveThread(&ScoreKeeping::receiveData, this, server, std::ref(quit));

    while (!quit)
    {
        
    }

    //sendThread.join();
    receiveThread.join();

    cleanup(server);
}

void ScoreKeeping::cleanup(TCPsocket& server)
{
	SDLNet_TCP_Close(server);
	SDLNet_Quit();
	SDL_Quit();
}

void ScoreKeeping::update(TCPsocket& server)
{
}



void ScoreKeeping::receiveData(TCPsocket server, bool& quit_)
{
    while (!quit_)
    {
        TCPsocket client = SDLNet_TCP_Accept(server);

        if (!client) {
            printf("Searching for connection\n");
            auto currentTime = std::chrono::system_clock::now();

            // Calculate a time point 5 seconds into the future
            auto wakeUpTime = currentTime + std::chrono::seconds(3);

            // Sleep until the specified time
            std::this_thread::sleep_until(wakeUpTime);

            continue;
        }

        //Connection Established create a loop
        printf("Connected\n");

        while (1)
        {
            char buffer[1024];

            int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));

            //data not received
            if (received == sizeof(buffer))
                continue;

            if (received > 0) {
                buffer[received] = '\0';

                deserializeScore(std::string(buffer));

                printf("Player 1 score: %i\nPlayer 2 score: %i\n\n", player1score, player2score);
            }
            else {
                printf("Client Disconnected\n");
                quit_ = true;
                break;
            }
        }

        //Client and Server Connection
        SDLNet_TCP_Close(client);
    }
}

std::string ScoreKeeping::serializeScore()
{
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("score1");
    writer.Int(player1score);
    writer.Key("score2");
    writer.Int(player2score);

    writer.EndObject();

    return s.GetString();
}

bool ScoreKeeping::deserializeScore(const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());

    if (!doc.IsObject()) {
        return false;
    }

    const rapidjson::Value& score1 = doc["score1"];
    const rapidjson::Value& score2 = doc["score2"];

    if (!score1.IsInt() || !score2.IsInt()) {
        return false; //keeps on returning false when parsing
    }

    player1score = (score1.GetInt());
    player2score = (score2.GetInt());

    return true;
}