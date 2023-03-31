#pragma once
#include "CBaseFactory.h"
#include <iostream>
#include <mutex>


class script;
CFactory(CScriptVM)
{
public:

	CScriptVM() {};


	static  char __fastcall VMRun(std::uint32_t ops_to_execute);

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;

	void add_script(std::unique_ptr<script> script);
	void remove_all_scripts();
	void tick();
	std::recursive_mutex m_mutex;
	std::vector<std::unique_ptr<script>> m_scripts;
};
extern CScriptVM* g_ScriptVM;