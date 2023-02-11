#include "CLevelLoader.h"
#include "Hooking.h"
#include <xlocbuf>
#include <string>
#include <MinHook.h>
#include "CBootstrap.h"
static std::string g_overrideNextLoadedLevel;
static std::string g_nextLevelPath;

static bool g_wasLastLevelCustom;
static bool g_gameUnloaded = false;





int ReturnFalse()
{
	return 0;
}
//static void(*g_origWaitUntilGameStarts)();
//
//void WaitUntilGameStarts() {
//
//	if (CBaseFactory<CLevelLoader>::Get().ShouldLoad) {
//		CBaseFactory<CLevelLoader>::Get().ShouldLoad = false;
//		*CBaseFactory<CLevelLoader>::Get().g_initState = 7;
//		return;
//	}
//
//	if (CBaseFactory<CLevelLoader>::Get().InLoading && *CBaseFactory<CLevelLoader>::Get().g_initState == 0)
//	{
//		CBaseFactory<CLevelLoader>::Get().InLoading = false;
//		printf("Finished loading level.");
//	}
//	static bool Started = false;
//
//
//	//printf("g_origWaitUntilGameStarts called \n");
//	return g_origWaitUntilGameStarts();
//}

static bool WaitWithWait(HANDLE handle)
{
	return WaitForSingleObject(handle, 500) == WAIT_OBJECT_0;
}

void CLevelLoader::Hook()
{

	

	char* p = hook::pattern("BA 08 00 00 00 8D 41 FC 83 F8 01").count(1).get(0).get<char>(14);

	char* varPtr = p + 2;
	g_initState = (int*)(varPtr + *(int32_t*)varPtr + 4);

	// stop the ros sdk input blocker
//	hook::jump(hook::get_pattern("48 8B 01 FF 50 10 84 C0 74 05 BB 01 00 00 00 8A", -0x14), ReturnFalse);



	hook::put<uint32_t>(hook::get_pattern("84 C0 74 36 48 8B 0D ? ? ? ? 48 85 C9", -13), 0x90C301B0);
	
}

void CLevelLoader::Unhook()
{

}

void CLevelLoader::RequestLoad()
{
	if (CBaseFactory<CLevelLoader>::Get().state == GAME_STATE::IN_LOADING || CBaseFactory<CLevelLoader>::Get().state == GAME_STATE::IN_GAME) return;
	  CBaseFactory<CLevelLoader>::Get().ShouldLoad = true;
}
static void(*g_origWaitUntilGameStarts)();


void WaitUntilGameStarts() {
	if (CBaseFactory<CLevelLoader>::Get().ShouldLoad) {
		CBaseFactory<CLevelLoader>::Get().ShouldLoad = false;
		*CBaseFactory<CLevelLoader>::Get().g_initState = 7;
		CBaseFactory<CLevelLoader>::Get().state = GAME_STATE::IN_LOADING;
		return;
	}
	if (CBaseFactory<CLevelLoader>::Get().state == GAME_STATE::IN_LOADING && *CBaseFactory<CLevelLoader>::Get().g_initState == 0)
	{
		CBaseFactory<CLevelLoader>::Get().state = GAME_STATE::IN_GAME;
		ShowWindow(CBootstrap::instance->gameWindow, SW_SHOWDEFAULT);
		printf("Finished loading level %u \n.", (std::uintptr_t)CBootstrap::instance->gameWindow);
	}

	static bool Requested = false;

	if (!Requested && *CBaseFactory<CLevelLoader>::Get().g_initState == 22) {
		CBaseFactory<CLevelLoader>::Get().RequestLoad();
		Requested = true;
	}

	//printf("g_origWaitUntilGameStarts called  *CBaseFactory<CLevelLoader>::Get().g_initState => %i\n", *CBaseFactory<CLevelLoader>::Get().g_initState);
	return g_origWaitUntilGameStarts();
}

void CLevelLoader::OnGameHook() {

	char* p = hook::pattern("BA 08 00 00 00 8D 41 FC 83 F8 01").count(1).get(0).get<char>(14);

	char* varPtr = p + 2;
	g_initState = (int*)(varPtr + *(int32_t*)varPtr + 4);

	char* WaitUntilGameCaller = hook::pattern("E8 ? ? ? ? 8B CF E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ?").count(1).get(0).get<char>();
	char* Waitcalll = hook::get_call(WaitUntilGameCaller);

	printf("Waitcalll: %i \n", (std::uintptr_t)(Waitcalll));


	if (MH_CreateHook(Waitcalll, &WaitUntilGameStarts, reinterpret_cast<void**>(&g_origWaitUntilGameStarts)) != MH_OK) {
		std::cout << "Failed hooking run script threads " << std::endl;
	}

	if (MH_EnableHook(Waitcalll) != MH_OK)
		MessageBox(0, "Failed hooking Waitcalll thread. ", "CScriptVM::Hook", MB_ICONERROR);

	auto location = hook::get_pattern<char>("0F 95 05 ? ? ? ? E8 ? ? ? ? 48 85 C0");
	auto addy = hook::get_address<char*>(location + 3);
	hook::put<char>(addy, 0);
	hook::nop(location, 7);

	hook::put<uint32_t>(hook::get_pattern("48 81 C1 00 00 00 12 48 89 0D", 3), 0x36000000);
	hook::return_function(hook::get_pattern("45 33 E4 83 39 02 4C 8B FA 45 8D 6C", -0x1C));
} CLevelLoader* g_LevelLoader = new CLevelLoader();