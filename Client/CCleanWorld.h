#pragma once
#include "CBaseFactory.h"
#include <iostream>

CFactory(CCleanWorld)
{
public:

	CCleanWorld() {};

	

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;

};
extern CCleanWorld* g_CleanWorld;

