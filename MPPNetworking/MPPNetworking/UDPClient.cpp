//#include "UDPClient.h"
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_net.h>
//#include <iostream>
//#include <cstring>
//#include <sstream>
//#include <thread>
//
//int squareY;
//int client_ypos;
//
//struct square_pos
//{
//
//    int ypos;
//};
//
//square_pos server_square;
//square_pos client_square;
//
//// Function to parse a UDP packet and extract y values
//bool parseUDPPacket(UDPpacket* packet, int& y) {
//    if (packet && packet->data) {
//        std::string packetData(reinterpret_cast<char*>(packet->data));
//
//        // Find the positions of 'x:' and 'y:' in the packet data
//        
//        size_t yPos = packetData.find("y:");
//
//        // Check if 'x:' and 'y:' are found
//        if (yPos != std::string::npos ) {
//            // Extract the values of 'x' and 'y' using substrings
//           
//            std::string ySubstring = packetData.substr(yPos + 2);
//
//            // Convert the substrings to integers
//            
//            std::istringstream(ySubstring) >> y;
//
//            return true;
//        }
//    }
//
//    return false;
//}
//
//// Function to handle network communications
////void handleNetwork(SDL_Renderer* renderer, UDPsocket udpSocket ) {
//void handleNetwork(UDPsocket udpSocket) {
//    
//    int y = 0;
//
//    UDPpacket* receivePacket = SDLNet_AllocPacket(1024);
//
//    if (!receivePacket) {
//        SDL_Log("SDLNet_AllocPacket error: %s", SDLNet_GetError());
//        SDLNet_UDP_Close(udpSocket);
//        SDL_Quit();
//        return;
//    }
//
//    std::cout << "UDP server is running and listening on port 12345..." << std::endl;
//
//    while (true) {
//
//        
//        auto ypos = std::to_string(squareY);
//
//        auto msg_data =  ",y:" + ypos;
//
//        //RECEIVE
//        if (SDLNet_UDP_Recv(udpSocket, receivePacket)) {
//            IPaddress clientAddress = receivePacket->address;
//            Uint16 clientPort = clientAddress.port;
//            const char* clientHost = SDLNet_ResolveIP(&clientAddress);
//
//            std::cout << "Received from client " << clientHost << ":" << clientPort << ": " << receivePacket->data << std::endl;
//
//            // Deserialize the byte array back into the struct
//            //memcpy(&client_square, receivePacket->data, sizeof(square_pos));
//
//
//
//            if (parseUDPPacket(receivePacket, y)) {
//
//               
//                client_ypos = y;
//
//                //SDL_Rect squareRect_client = { x, y, SQUARE_SIZE, SQUARE_SIZE };
//                //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
//                //SDL_RenderFillRect(renderer, &squareRect_client);
//            }
//
//        }
//
//        //SEND
//        // Create a UDP packet to send the client square position to the server
//        UDPpacket* packet = SDLNet_AllocPacket(sizeof(SDL_Rect));
//        if (packet) {
//            packet->address = receivePacket->address;
//            packet->len = sizeof(SDL_Rect);
//            //memcpy(packet->data, &server_square, sizeof(square_pos)); 
//            memcpy(packet->data, msg_data.c_str(), msg_data.size());
//            //memcpy(packet->data, &squareRect, sizeof(SDL_Rect));
//
//            if (SDLNet_UDP_Send(udpSocket, -1, packet) == 0) {
//                std::cerr << "SDLNet_UDP_Send error: " << SDLNet_GetError() << std::endl;
//            }
//        }
//
//        SDLNet_FreePacket(packet);
//    }
//
//    SDLNet_FreePacket(receivePacket);
//}
//
//
//// Function to handle drawing
//void handleDrawing(SDL_Renderer* renderer) {
//   ;
//   
//
//    bool quit = false;
//    SDL_Event e;
//
//    while (!quit) {
//        while (SDL_PollEvent(&e)) {
//            if (e.type == SDL_QUIT) {
//                quit = true;
//            }
//            else if (e.type == SDL_KEYDOWN) {
//                switch (e.key.keysym.sym) {
//                case SDLK_UP:
//                    squareY -= 10;
//                    break;
//                case SDLK_DOWN:
//                    squareY += 10;
//                    break;
//
//                }
//            }
//        }
//    }
//       
//        server_square.ypos = squareY;
//
//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//        SDL_RenderClear(renderer);
//
//        SDL_Rect squareRect = { squareY, SQUARE_SIZE, SQUARE_SIZE };
//        //SDL_Rect squareRect = {server_square.xpos, server_square.ypos, SQUARE_SIZE, SQUARE_SIZE};
//        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//        SDL_RenderFillRect(renderer, &squareRect);
//
//
//        std::cout << "\nClient Position: y=" << client_ypos << std::endl;
//        if (client_ypos != 0)
//        {
//            //SDL_Rect squareRect_client = { client_square.xpos, client_square.ypos, SQUARE_SIZE, SQUARE_SIZE };
//            SDL_Rect squareRect_client = { client_ypos, SQUARE_SIZE, SQUARE_SIZE };
//
//            SDL_RenderFillRect(renderer, &squareRect_client);
//        }