#pragma once
#include <SDL2/SDL_net.h>
#include <string>

class ScoreKeeping {
private:
	int player1score;
	int player2score;

	TCPsocket server;
public:
	void init(const char* host, Uint16 port);
	void cleanup(TCPsocket& server);
	void update(TCPsocket& server);

	void sendData(TCPsocket server, bool& quit_);
	void receiveData(TCPsocket server, bool& quit_);


	std::string serializeScore();
	bool deserializeScore(const std::string& json);
};