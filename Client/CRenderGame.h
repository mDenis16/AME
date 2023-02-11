#pragma once
#include "CBaseFactory.h"
#include <iostream>



CFactory(CRenderGame)
{
public:

	CRenderGame() {};


	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;

};
CEnd(CRenderGame)
