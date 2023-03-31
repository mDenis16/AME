#pragma once
#include "CBaseFactory.h"
#include <iostream>



template<typename T>
class pgCollection;
template<typename T>
class pgPtrCollection;

class ScriptThread;
class scriptHandlerMgr;
class scrThread;

CFactory(CScrEngine)
{
public:
	CScrEngine() {};

	scriptHandlerMgr* GetScriptHandleMgr();
	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
	void OnGameFrame();

	static pgPtrCollection<ScriptThread>* GetThreadCollection();
	static void CreateThread(ScriptThread* thread);
	static void AddToList(ScriptThread* thread);
	static scrThread* GetActiveThread();
	static void SetActiveThread(scrThread* thread);
};
extern CScrEngine* g_ScrEngine;

