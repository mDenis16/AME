#pragma once
#include "CBaseFactory.h"
#include <iostream>



CFactory(CScriptHandlerMgr)
{
public:

	CScriptHandlerMgr() {};

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
};
CEnd(CScriptHandlerMgr)
