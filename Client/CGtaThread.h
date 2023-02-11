#pragma once
#include "CBaseFactory.h"
#include <iostream>



CFactory(CGtaThread)
{
public:

	CGtaThread() {};


	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
};
CEnd(CGtaThread)
