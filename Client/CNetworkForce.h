#pragma once
#include "CBaseFactory.h"
#include <iostream>

CFactory(CNetworkForce)
{
public:

	CNetworkForce() {};


	
	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
	void OnGameFrame();
};
extern CNetworkForce* g_NetworkForce;

