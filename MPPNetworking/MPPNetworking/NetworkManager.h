#pragma once
#include "Game.h"

class NetworkManager
{
private:
	Game* game;
	bool isHost;

public:
	NetworkManager(bool isHost_);
	void InitGame();
	void GameLoop();
	void Cleanup();
};