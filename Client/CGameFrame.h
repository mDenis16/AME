#pragma once
#include "CBaseFactory.h"
#include <iostream>

CFactory(CGameFrame)
{
public:

	CGameFrame() {};

	static bool OnLookAlive();
	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;

	inline static bool g_shouldSetState = false;
	inline static bool g_triedLoading = false;
};
extern CGameFrame* g_GameFrame;