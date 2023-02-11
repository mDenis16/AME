#pragma once
#include "CBaseFactory.h"
#include <iostream>



CFactory(CScriptVM)
{
public:

	CScriptVM() {};


	static bool VMRun(std::uint32_t ops_to_execute);

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
};
extern CScriptVM* g_ScriptVM;