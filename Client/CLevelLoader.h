#pragma once
#include "CBaseFactory.h"
#include <iostream>


enum GAME_STATE {
	GAME_SCREEN,
	IN_LOADING,
	IN_GAME
};
CFactory(CLevelLoader)
{
public:

	CLevelLoader() {};

	int* g_initState = 0;

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;

	void RequestLoad();
	GAME_STATE state;

	bool ShouldLoad = false;
};
extern CLevelLoader* g_LevelLoader;

