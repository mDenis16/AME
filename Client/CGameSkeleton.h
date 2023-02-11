#pragma once
#include "CBaseFactory.h"
#include <iostream>

CFactory(CGameSkeleton)
{
public:

	CGameSkeleton() {};


	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
};
CEnd(CGameSkeleton)
