#pragma once
#include "CBaseFactory.h"
#include <iostream>


CFactory(CGameEvents)
{
public:

	CGameEvents() {};

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;

};
extern CGameEvents* g_GameEvents;
