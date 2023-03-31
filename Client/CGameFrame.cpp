#include "CGameFrame.h"
#include "Hooking.h"
#include "CLevelLoader.h"
#include "CNetworkForce.h"

#include "CSyncManager.h"
#include <spdlog/spdlog.h>

static bool(*g_origLookAlive)();
static void (*g_orig_sysService_UpdateClass)(void*);

#include "CBootstrap.h"
#include "nativeList.h"
#include <MinHook.h>
static int* netNoUpnp = 0;
static int* netNoPcp = 0;
bool CGameFrame::OnLookAlive()
{
	
	

	static bool Run = false;
	if (!Run) {
		g_LevelLoader->OnGameHook();
		//g_NativeInvoker->OnGameHook();
		//g_NetworkForce->OnGameHook();
	
		
		Run = true;
	}
	g_LevelLoader->OnFrame();
	
	g_NetworkForce->OnGameFrame();
	
	*netNoUpnp = TRUE;
	*netNoPcp = TRUE;

	static bool Called = false;
	if (!Called && CBaseFactory<CLevelLoader>::Get().state == GAME_STATE::IN_GAME) {
		printf("Called spawn player faster \n");
		GamePed player = PLAYER::PLAYER_PED_ID();

		SCRIPT::SHUTDOWN_LOADING_SCREEN();
		CAM::DO_SCREEN_FADE_IN(0);

		ENTITY::SET_ENTITY_COORDS(player, 293.089f, 180.466f, 104.301f, true, true, true, false);
		Called = true;
	}
	

	return g_origLookAlive();
}
void CGameFrame::Hook()
{
	
	spdlog::info("CGameFrame::Hook");
	void* lookAliveFrameCall = hook::pattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? E8 ? ? ? ? 33 F6 48 8D").count(1).get(0).get<void>();

	if (MH_CreateHook(lookAliveFrameCall, &OnLookAlive, reinterpret_cast<void**>(&g_origLookAlive)) != MH_OK) {
		MessageBox(0, "Failed hooking OnLookAlive", "Error", MB_ICONERROR);
	}

	if (MH_EnableHook(lookAliveFrameCall) != MH_OK)
		MessageBox(0, "Failed enable OnLookAlive", "Error", MB_ICONERROR);

	netNoUpnp = hook::get_address<int*>(hook::get_pattern("8A D1 76 02 B2 01 48 39 0D", 9));
	netNoPcp = hook::get_address<int*>(hook::get_pattern("8A D1 EB 02 B2 01 48 39 0D", 9));
	/*hook::set_call(&g_origLookAlive, lookAliveFrameCall);
	hook::call(lookAliveFrameCall, OnLookAlive);*/
	
	auto p_legalNotice = hook::pattern("72 1F E8 ? ? ? ? 8B 0D").count(1).get(0).get<char>(0);
	memset(p_legalNotice, 0x90, 2);

	// SC eula accepted
	hook::put<uint32_t>(hook::get_pattern("84 C0 74 36 48 8B 0D ? ? ? ? 48 85 C9", -13), 0x90C301B0);

	// no showwindow early
	/*{
		auto location = hook::get_pattern<char>("41 8B D4 48 8B C8 48 8B D8 FF 15", 9);
		hook::nop(location, 6);
		hook::nop(location + 9, 6);
		hook::nop(location + 18, 6);
	}*/
}

void CGameFrame::Unhook()
{

}

void CGameFrame::OnGameHook() {

}

 CGameFrame* g_GameFrame = new CGameFrame();