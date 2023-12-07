#pragma once
#include "Game.h"

class UDPServer
{
private:
	Game* game;

public:
	UDPServer();
	void InitGame();
	void GameLoop();
	void Cleanup();
};