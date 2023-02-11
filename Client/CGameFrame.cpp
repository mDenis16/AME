#include "CGameFrame.h"
#include "Hooking.h"
#include "CGameEvents.h"
#include "CLevelLoader.h"
#include "nativeList.h"
#include "CNetworkForce.h"
#include <MinHook.h>

static bool(*g_origLookAlive)();
static void (*g_orig_sysService_UpdateClass)(void*);


bool CGameFrame::OnLookAlive()
{


	static bool Run = false;
	if (!Run) {
		g_LevelLoader->OnGameHook();
		g_NativeInvoker->OnGameHook();
		g_NetworkForce->OnGameHook();
		Run = true;
	}
	
	g_NetworkForce->OnGameFrame();

	return g_origLookAlive();
}
void CGameFrame::Hook()
{

	void* lookAliveFrameCall = hook::pattern("48 81 EC ? 01 00 00 E8 ? ? ? ? 33 F6 48 8D").count(1).get(0).get<void>(7);

	hook::set_call(&g_origLookAlive, lookAliveFrameCall);
	hook::call(lookAliveFrameCall, OnLookAlive);
	
	auto p_legalNotice = hook::pattern("72 1F E8 ? ? ? ? 8B 0D").count(1).get(0).get<char>(0);
	memset(p_legalNotice, 0x90, 2);

	hook::put<uint32_t>(hook::get_pattern("84 C0 74 36 48 8B 0D ? ? ? ? 48 85 C9", -13), 0x90C301B0);

	auto location = hook::get_pattern<char>("41 8B D4 48 8B C8 48 8B D8 FF 15", 9);
	hook::nop(location, 6);
	hook::nop(location + 9, 6);
	hook::nop(location + 18, 6);
}

void CGameFrame::Unhook()
{

}

void CGameFrame::OnGameHook() {

}

 CGameFrame* g_GameFrame = new CGameFrame();