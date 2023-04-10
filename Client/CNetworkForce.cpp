
#include "CNetworkForce.h"
#include "Hooking.h"
#include <MinHook.h>
#include "nativeList.h"
#include "atPool.h"
#include "CPoolManagement.h"
#include <spdlog/spdlog.h>
#include "CSyncManager.h"
#include "ida.h"

static void* HostSessionCall = nullptr;
static bool g_SkipCheckNet = false;
static void ExitCleanly()
{
	ExitProcess(0);

	__debugbreak();
}
static bool tryHosting = false;
static int* g_netNewVal;
static bool* didPresenceStuff;
static void* g_NetworkBail = nullptr;
using networkBail_2372 = void((*)(void*, bool));
static bool* g_isNetGame;
static void* g_isSessionStarted = nullptr;
static void GetNetNewVal()
{
	*g_netNewVal = 0;
}
static void(*g_origMigrateCopy)(void*, void*);
static int ReturnTrue()
{
	return true;
}
bool IsSessionStarted() {
	return reinterpret_cast<bool(__cdecl*)()>(g_isSessionStarted)();
}
void hostGame(int a1, int a2, int a3) {
	//return reinterpret_cast<void(__cdecl*)(int a1, int a2, int a3)>(samp)(a1, a2, a3);
}
void MigrateSessionCopy(char* target, char* source)
{
	g_origMigrateCopy(target, source);

	//auto sessionAddress = reinterpret_cast<rlSessionInfo<Build>*>(target - 16);

	/*std::unique_ptr<net::Buffer> msgBuffer(new net::Buffer(64));

	msgBuffer->Write<uint32_t>((sessionAddress->addr.localAddr().ip.addr & 0xFFFF) ^ 0xFEED);
	msgBuffer->Write<uint32_t>(sessionAddress->addr.unkKey1());*/
	printf("MigrateSessionCopy \n");
	//g_netLibrary->SendReliableCommand("msgHeHost", reinterpret_cast<const char*>(msgBuffer->GetBuffer()), msgBuffer->GetCurOffset());
}
static int* g_clipsetManager_networkState;
static void (*g_orig_fwClipSetManager_StartNetworkSession)();

static void fwClipSetManager_StartNetworkSessionHook()
{
	if (*g_clipsetManager_networkState != 2)
	{
		g_orig_fwClipSetManager_StartNetworkSession();
	}
}


//E8 ? ? ? ? 40 8A C7 4C 8D 9C 24 ? ? ? ? 49 8B 5B 20 49 8B 6B 28 49 8B 73 30 49 8B 7B 38 49 8B E3 

static char (*g_Host_Session)(__int64 a1, unsigned __int16 a2, __int64 a3, unsigned int a4);
char __fastcall Host_SessionHK(__int64 a1, unsigned __int16 a2, __int64 a3, unsigned int a4)
{

	*(DWORD*)(a1 + 0xB27C) = 3;
	auto result = g_Host_Session(a1, a2, a3, a4);


	return result;
}

//E8 ? ? ? ? 84 C0 74 E6 33 D2 8D 4A 01 E8 ? ? ? ? 84 C0 74 D8 85 FF 7E D4 
static char (*g_CheckIfShouldAllowRelatedToHosting)(void* a1, char* a2, char a3);
bool __fastcall CheckIfShouldAllowRelatedToHosting(void* a1, char* a2, char a3) {
	//MessageBox(0, "CheckIfShouldAllowRelatedToHosting", "Mearsa", MB_OK);
	//printf("CheckIfShouldAllowRelatedToHosting hook call %s \n", a2);
	if (strcmp((const char*)a2, "HostSesion")) {
		//printf("Bypassed CheckIfShouldAllowRelatedToHosting to true \n");
		return true;
	}

	bool result = g_CheckIfShouldAllowRelatedToHosting(a1, a2, a3);

	return result;
}

//E8 ? ? ? ? 84 C0 74 D8 85 FF 7E D4 41 BD ? ? ? ? 48 8B CE 41 3B FD 41 0F 4F FD E8 ? ? ? ? 84 C0 74 BB 83 FF 01 75 03
__int64 __fastcall CheckForallowingHosting(__int64 a1, __int64 a2) {
	//printf("CheckForallowingHosting hook call \n");
	return 1;
}

//E8 ? ? ? ? 84 C0 74 BB 83 FF 01 75 03 83 CB 04 80 BE ? ? ? ? ? 74 0B 
bool sub_7FF75CC2EC28HK() {
	//printf("sub_7FF75CC2EC28HK \n");
	return true;
}
static char (*g_Host_SessionInternal)(__int64 sessionManager, DWORD* a2);
char __fastcall Host_SessionInternal(__int64 sessionManager, DWORD* a2) {
	//printf("Host_SessionInternal called\n");
	return g_Host_SessionInternal(sessionManager, a2);
}

//E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 05 ? ? ? ? 4C 8B 0D ? ? ? ? 4C 8B 05 ? ? ? ? 48 8B 15 ? ? ? ? 48 8B 8F ? ? ? ? C6 44 24 ? ? 48 89 44 24 ? 48 8B 05 ? ? ? ? 

static char (*g_CheckForNetGame)(__int64 sessionManager, __int64 a2, DWORD* a3, __int64 a4, int a5);
char __fastcall CheckForNetGame(__int64 a1, __int64 a2, DWORD* a3, __int64 a4, int a5) {
	//printf("Check for net game \n");

	return 1;

	return g_CheckForNetGame(a1, a2, a3, a4, a5);
}
static void* getNetworkManager()
{
	static void** networkMgrPtr = nullptr;

	if (networkMgrPtr == nullptr)
	{
		networkMgrPtr = hook::get_address<void**>(hook::get_pattern("84 C0 74 2E 48 8B 0D ? ? ? ? 48 8D 54 24 20", 7));
	}

	return *networkMgrPtr;
}
char __fastcall isNetworkHost(__int64 a1) {
	return 1;
}
//

static void SetupLocalPlayer(void* player)
{
	//player->physicalPlayerIndex() = 15;

}

//E8 ? ? ? ? 84 C0 0F 95 C0 48 83 C4 28 C3 CC E5 A9 48 83 EC 28 80 3D ? ? ? ? ? 75 11 E8 ? ? ? ?
bool __fastcall CanEnterMultiplayer(void* networkMgr) {
	return true;
}

//E8 ? ? ? ? EB 0E B9 ? ? ? ? E8 ? ? ? ? EB 02 32 C0 84 C0 0F 95 C0 48 83 C4 20 5B
static char (*g_IsSocialBanned)(__int64 a1, int a2);
unsigned __int8 __fastcall IsSocialBanned(__int64 a1, int a2) {
	//printf("Called IsSocialBanned \n");
	if (!tryHosting)
		g_IsSocialBanned(a1, a2);

	return 1;
}

static rlGamerInfo** g_gamerInfo;

static void(*g_origManageHostMigration)(void*);

static void ManageHostMigrationStub(void* a1)
{

}
static void(*g_origUnkBubbleWrap)();

static void UnkBubbleWrap()
{

}
void CNetworkForce::Hook() {

	// exit game on game exit from alt-f4

	hook::call(hook::get_pattern("48 83 F8 04 75 ? 40 88", 6), ExitCleanly);
	//// no netgame jumpouts in alt-f4
	//hook::put<uint8_t>(hook::get_pattern("40 38 35 ? ? ? ? 74 0A 48 8B CF", 7), 0xEB);

	//// fix 'restart' handling to not ask MTL to restart, but relaunch 'ourselves' (eg on settings change)
	//hook::put<uint8_t>(hook::get_pattern("48 85 C9 74 15 40 38 31 74", 3), 0xEB);
	//{
	//	char* getNewNewVal = hook::pattern("41 83 CF FF 33 DB 4C 8D 25").count(1).get(0).get<char>(45);
	//	g_netNewVal = (int*)(*(int32_t*)getNewNewVal + getNewNewVal + 4);

	//	getNewNewVal -= 0x4E;
	//	hook::jump(getNewNewVal, GetNetNewVal);
	//}
	//didPresenceStuff = hook::get_address<bool*>(hook::get_pattern<char>("75 ? 40 38 3D ? ? ? ? 74 ? 48 8D 0D", 5));

	//g_isSessionStarted = hook::get_call(hook::get_pattern("8B 86 D8 08 00 00 C1 E8 05", 13));


	static uintptr_t gamerInfoPtr = hook::get_address<uintptr_t>((char*)hook::get_call(hook::get_pattern("48 8B E8 48 85 C0 74 11 E8", 8)) + 3);
	g_gamerInfo = (decltype(g_gamerInfo))(&gamerInfoPtr);

	auto location = hook::get_pattern<char>("48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 74 5F", 6);
	g_isNetGame = (bool*)(location + *(int32_t*)location + 4 + 1); // 1 as end of instruction is after '00', cmp

	// don't consider ourselves as host for world state reassignment
	hook::put<uint16_t>(hook::get_pattern("EB 02 32 C0 84 C0 0F 84 B4 00", 6), 0xE990);

	// change session count
	// 1604 changed this address to be a bit more specific
	hook::put<uint32_t>(hook::get_pattern("C7 87 ? ? ? 00 18 00 00 00 C7 87", 6), 0x40 >> 1);


	//
	// don't redundantly switch clipset manager to network mode
	// (blocks on a LoadAllObjectsNow after scene has initialized already)
	g_clipsetManager_networkState =
		hook::get_address<int*>(hook::get_pattern("0F 85 6B FF FF FF C7 05", 6), 2, 10);
	MH_CreateHook(hook::get_pattern("83 E1 02 74 34 A8 04 75", -0x36),
		fwClipSetManager_StartNetworkSessionHook, (void**)&g_orig_fwClipSetManager_StartNetworkSession);

	//// don't switch to SP mode either
	hook::return_function(hook::get_pattern("48 8D 2D ? ? ? ? 8B F0 85 C0 0F", -0x15));

	//// start clipset manager off in network mode
	hook::put<uint32_t>(hook::get_pattern("0F 85 6B FF FF FF C7 05", 12), 2); // network state flag
	hook::put<uint8_t>(hook::get_pattern("F6 44 07 04 02 74 7A", 4), 4); // check persistent sp flag -> persistent mp

	MH_CreateHook(hook::get_pattern("01 4F 60 81 7F 60 D0 07 00 00 0F 8E", -0x47), ManageHostMigrationStub, (void**)&g_origManageHostMigration);

	MH_CreateHook(hook::get_pattern("33 F6 33 DB 33 ED 0F 28 80", -0x3A), UnkBubbleWrap, (void**)&g_origUnkBubbleWrap);


	// pretend to have CGameScriptHandlerNetComponent always be host
	// (we now use 'real' network scripts with net component, and compatibility
	// mandates check of netComponent && netComponent->IsHost() always fails)
	hook::jump(hook::get_pattern("33 DB 48 85 C0 74 17 48 8B 48 10 48 85 C9 74 0E", -10), ReturnTrue);

	// always return 'true' from netObjectMgr duplicate script object ID checks
// (this function deletes network objects considered as duplicate)
	hook::jump(hook::get_pattern("49 8B 0C 24 0F B6 51", -0x69), ReturnTrue);

	/*auto HostSession_Call =
	if (MH_CreateHook(HostSession_Call, &Host_SessionHK, reinterpret_cast<void**>(&g_Host_Session)) != MH_OK) {
		std::cout << "Failed hooking run script threads " << std::endl;
	}

	if (MH_EnableHook(HostSession_Call) != MH_OK)
		MessageBox(0, "Failed hooking Waitcalll thread. ", "CScriptVM::Hook", MB_ICONERROR);*/


	auto stringComparer = hook::pattern("E8 ? ? ? ? 84 C0 74 E6 33 D2 8D 4A 01 E8 ? ? ? ? 84 C0 74 D8 85 FF 7E D4").count(1).get(0).get<void>();
	auto stringComparrerCall = hook::get_call(stringComparer);
	if (MH_CreateHook(stringComparrerCall, &CheckIfShouldAllowRelatedToHosting, reinterpret_cast<void**>(&g_CheckIfShouldAllowRelatedToHosting)) != MH_OK) {
		std::cout << "Failed hooking run script threads " << std::endl;
	}

	if (MH_EnableHook(stringComparrerCall) != MH_OK)
		MessageBox(0, "Failed hooking stringComparrerCall. ", "stringComparrerCall", MB_ICONERROR);


	auto HostSessionCaller = hook::pattern("E8 ? ? ? ? 40 8A C7 4C 8D 9C 24 ? ? ? ? 49 8B 5B 20 49 8B 6B 28 49 8B 73 30 49 8B 7B 38 49 8B E3").count(1).get(0).get<void>();
	HostSessionCall = hook::get_call(HostSessionCaller);
	if (MH_CreateHook(HostSessionCall, &Host_SessionHK, reinterpret_cast<void**>(&g_Host_Session)) != MH_OK) {
		std::cout << "Failed hooking HostSessionCall" << std::endl;
	}

	if (MH_EnableHook(HostSessionCall) != MH_OK)
		MessageBox(0, "Failed hookingHostSessionCall. ", "HostSessionCall", MB_ICONERROR);


	auto CheckForAllowingHostingaCaller = hook::pattern("E8 ? ? ? ? 84 C0 74 D8 85 FF 7E D4 41 BD ? ? ? ? 48 8B CE 41 3B FD 41 0F 4F FD E8 ? ? ? ? 84 C0 74 BB 83 FF 01 75 03").count(1).get(0).get<void>();
	auto CheckForAllowingHostingCall = hook::get_call(CheckForAllowingHostingaCaller);

	printf("CheckForAllowingHostingCall %p \n", CheckForAllowingHostingCall);
	if (MH_CreateHook(CheckForAllowingHostingCall, &CheckForallowingHosting, nullptr) != MH_OK) {
		std::cout << "Failed hooking CheckForAllowingHostingCall" << std::endl;
	}

	if (MH_EnableHook(CheckForAllowingHostingCall) != MH_OK)
		MessageBox(0, "Failed CheckForAllowingHostingCall. ", "CheckForAllowingHostingCall", MB_ICONERROR);

	//sub_7FF75CC2EC28HK
	auto sub_7FF75CC2EC28Caller = hook::pattern("E8 ? ? ? ? 84 C0 74 BB 83 FF 01 75 03 83 CB 04 80 BE ? ? ? ? ? 74 0B").count(1).get(0).get<void>();
	auto sub_7FF75CC2EC28Call = hook::get_call(sub_7FF75CC2EC28Caller);

	printf("sub_7FF75CC2EC28Call %p \n", sub_7FF75CC2EC28Call);
	if (MH_CreateHook(sub_7FF75CC2EC28Call, &sub_7FF75CC2EC28HK, nullptr) != MH_OK) {
		std::cout << "Failed hooking sub_7FF75CC2EC28HK" << std::endl;
	}

	if (MH_EnableHook(sub_7FF75CC2EC28Call) != MH_OK)
		MessageBox(0, "Failed sub_7FF75CC2EC28HK. ", "sub_7FF75CC2EC28HK", MB_ICONERROR);

	//Host_SessionInternal
	//E8 ? ? ? ? 4C 8D 9C 24 ? ? ? ? 49 8B 5B 20 49 8B 73 28 49 8B 7B 30 49 8B E3 41 5E 41 5D 5D
	auto Host_SessionInternalCaller = hook::pattern("E8 ? ? ? ? 4C 8D 9C 24 ? ? ? ? 49 8B 5B 20 49 8B 73 28 49 8B 7B 30 49 8B E3 41 5E 41 5D 5D").count(4).get(3).get<void>();
	auto Host_SessionInternalCall = hook::get_call(Host_SessionInternalCaller);

	printf("Host_SessionInternalCall %p \n", Host_SessionInternalCall);
	if (MH_CreateHook(Host_SessionInternalCall, &Host_SessionInternal, reinterpret_cast<void**>(&g_Host_SessionInternal)) != MH_OK) {
		std::cout << "Failed hooking Host_SessionInternalCall" << std::endl;
	}

	if (MH_EnableHook(Host_SessionInternalCall) != MH_OK)
		MessageBox(0, "Failed Host_SessionInternalCall. ", "Host_SessionInternalCall", MB_ICONERROR);

	//CheckForNetGame
	//E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 05 ? ? ? ? 4C 8B 0D ? ? ? ? 4C 8B 05 ? ? ? ? 48 8B 15 ? ? ? ? 48 8B 8F ? ? ? ? C6 44 24 ? ? 48 89 44 24 ? 48 8B 05 ? ? ? ? 
	auto CheckForNetGameCaller = hook::pattern("E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 05 ? ? ? ? 4C 8B 0D ? ? ? ? 4C 8B 05 ? ? ? ? 48 8B 15 ? ? ? ? 48 8B 8F ? ? ? ? C6 44 24 ? ? 48 89 44 24 ? 48 8B 05 ? ? ? ?").count(1).get(0).get<void>();
	auto CheckForNetGameCall = hook::get_call(CheckForNetGameCaller);

	/*printf("CheckForNetGameCall %p \n", CheckForNetGameCall);
	if (MH_CreateHook(CheckForNetGameCall, &CheckForNetGame, reinterpret_cast<void**>(&g_CheckForNetGame)) != MH_OK) {
		std::cout << "Failed hooking CheckForNetGameCall" << std::endl;
	}

	if (MH_EnableHook(CheckForNetGameCall) != MH_OK)
		MessageBox(0, "Failed CheckForNetGameCall. ", "CheckForNetGameCall", MB_ICONERROR);*/

	auto IsNetworkHostTarget = hook::pattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B F9 48 83 E9 80 E8 ? ? ? ? 33 DB 84 C0 74 1A 48 8D 8F ? ? ? ? E8 ? ? ? ? 48 3B 87 ? ? ? ? 75 05 BB ? ? ? ?").count(1).get(0).get<void>();
	if (MH_CreateHook(IsNetworkHostTarget, &isNetworkHost, /*reinterpret_cast<void**>(&g_CheckForNetGame)*/nullptr) != MH_OK) {
		std::cout << "Failed hooking IsNetworkHostTarget" << std::endl;
	}

	if (MH_EnableHook(IsNetworkHostTarget) != MH_OK)
		MessageBox(0, "Failed IsNetworkHostTarget. ", "IsNetworkHostTarget", MB_ICONERROR);


	auto CanEnterMultiplayerCall = hook::pattern("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 84 C0 75 04 32 C0 EB 26 83 C9 FF E8 ? ? ? ? 84 C0 74 10 48 8B 05 ? ? ? ? 80 B8 ? ? ? ? ? 74 E0").count(1).get(0).get<void>();

	if (MH_CreateHook(CanEnterMultiplayerCall, &CanEnterMultiplayer, /*reinterpret_cast<void**>(&g_CheckForNetGame)*/nullptr) != MH_OK) {
		std::cout << "Failed hooking CanEnterMultiplayer" << std::endl;
	}

	if (MH_EnableHook(CanEnterMultiplayerCall) != MH_OK)
		MessageBox(0, "Failed CanEnterMultiplayer. ", "CanEnterMultiplayer", MB_ICONERROR);


	void* locationT = hook::get_pattern("48 8B D0 E8 ? ? ? ? E8 ? ? ? ? 83 BB ? ? ? ? 04", 3);


	//E8 ? ? ? ? EB 0E B9 ? ? ? ? E8 ? ? ? ? EB 02 32 C0 84 C0 0F 95 C0 48 83 C4 20 5B

	auto IsSocialBannedCaller = hook::pattern("E8 ? ? ? ? EB 0E B9 ? ? ? ? E8 ? ? ? ? EB 02 32 C0 84 C0 0F 95 C0 48 83 C4 20 5B").count(2).get(0).get<void>();
	auto IsSocialBannedCall = hook::get_call(IsSocialBannedCaller);

	if (MH_CreateHook(IsSocialBannedCall, &IsSocialBanned, reinterpret_cast<void**>(&g_IsSocialBanned)) != MH_OK) {
		std::cout << "Failed hooking IsSocialBanned" << std::endl;
	}

	if (MH_EnableHook(IsSocialBannedCall) != MH_OK)
		MessageBox(0, "Failed IsSocialBanned. ", "IsSocialBanned", MB_ICONERROR);
}
void CNetworkForce::Unhook() {

}


void CNetworkForce::OnGameHook() {




}
char __fastcall HostSessionInternal(__int64 sessionMgr, unsigned __int16 a2, __int64 a3, unsigned int a4)
{
	int v5; // edi
	__int64 v9[2]; // [rsp+34h] [rbp-D4h] BYREF
	int v10; // [rsp+44h] [rbp-C4h]
	int v11; // [rsp+4Ch] [rbp-BCh]
	int v12; // [rsp+54h] [rbp-B4h]
	int v13; // [rsp+5Ch] [rbp-ACh]
	int v14; // [rsp+64h] [rbp-A4h]
	int v15; // [rsp+6Ch] [rbp-9Ch]
	int v16; // [rsp+74h] [rbp-94h]
	int v17; // [rsp+7Ch] [rbp-8Ch]
	char v18[336]; // [rsp+88h] [rbp-80h] BYREF

	v5 = a3;
	if (*(int*)(sessionMgr + 0xB27C) >= 4)
		return 0;
	a3 = 1;

	if (v5 > 32)
		v5 = 32;

	if (v5 == 1)
		a4 |= 4u;

	if (*(_BYTE*)(sessionMgr + 0x2254D) && (a4 & 4) != 0)
		++v5;
	static auto initializeSomeDataPassedInsideSessionMgr_CreateSession = hook::pattern("40 53 48 83 EC 20 83 61 08 00 C6 41 0C 00 0F B6 41 0C 0D ? ? ? ? 48 8B D9 89 41 0C 48 83 C8 FF 89 41 10").count(2).get(0).get<void>();
	reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(initializeSomeDataPassedInsideSessionMgr_CreateSession)((__int64)v18);

	v9[1] = 0i64;
	v9[0] = 8;
	v10 = 268435457;
	v11 = 268435475;
	v12 = 268435476;
	v13 = 268435479;
	v14 = 268435480;
	v15 = 268435482;
	v16 = 268435483;
	v17 = 268435484;
	//sub_7FF75CB7D318((int*)(unsigned int)v18, (unsigned int)v9 + 4, a2, 0, v5, (a4 & 1) != 0 ? v5 : 0);
	//
	// 
	static auto sub_7FF75CB7D318 = hook::pattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 48 8B 01 45 0F B7 F1 45 0F B7 F8 48 8B DA 48 8B F9 FF 50 08 C7 47").count(1).get(0).get<void>();
	reinterpret_cast<__int64(__fastcall*)(int* a1,
		__int64 a2,
		unsigned __int16 a3,
		unsigned __int16 a4,
		unsigned int a5,
		unsigned int a6)>(sub_7FF75CB7D318)((int*)(unsigned int)v18, (unsigned int)v9 + 4, a2, 0, v5, (a4 & 1) != 0 ? v5 : 0);
	//
	if (*(_BYTE*)(sessionMgr + 0x1C4D8) == 1)
		a4 |= 0x20u;
	*(_DWORD*)(sessionMgr + 0xB6D0) = 0;

	static auto SessionMgrInternal_CreateSession = hook::pattern("48 89 5C 24 ? 48 89 54 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 41 BE ? ? ? ? 45 8B E0 48 8B FA 4C 8B F9 44 84 B1 ? ? ? ? 0F 84 ? ? ?").count(1).get(0).get<void>();


	return reinterpret_cast<char(__fastcall*)(__int64 a1, __int64 a2, __int64 a3)>(SessionMgrInternal_CreateSession)(sessionMgr, (__int64)v18, a4);

}
void CNetworkForce::OnGameFrame() {

	/*static auto netNoUpnp = hook::get_address<int*>(hook::get_pattern("8A D1 76 02 B2 01 48 39 0D", 9));
	static auto netNoPcp = hook::get_address<int*>(hook::get_pattern("8A D1 EB 02 B2 01 48 39 0D", 9));
	*netNoUpnp = TRUE;
	*netNoPcp = TRUE;*/


	//static auto setgamerInfo = hook::get_pattern("3A D8 0F 95 C3 40 0A DE 40", -0x56);
//	auto networkMgr = getNetworkManager();
	//static bool CAlled = false;
	//if (!CAlled) {
	//	CAlled = true;
	//	// ignore CMsgJoinRequest failure reason '7' (seemingly related to tunables not matching?)
	//	hook::put<uint8_t>(hook::pattern("84 C0 75 0B 41 BC 07 00 00 00").count(1).get(0).get<void>(2), 0xEB);

	//	// also ignore the rarer CMsgJoinRequest failure reason '13' (something related to what seems to be like stats)
	//	hook::put<uint8_t>(hook::pattern("3B ? 74 0B 41 BC 0D 00 00 00").count(1).get(0).get<void>(2), 0xEB);

	//	// ignore CMsgJoinRequest failure reason 15 ('mismatching network timeout')
	//	hook::put<uint8_t>(hook::get_pattern("74 0B 41 BC 0F 00 00 00 E9", 0), 0xEB);

	//}

	if (!NETWORK::NETWORK_IS_IN_SESSION() && GetAsyncKeyState(VK_SHIFT)) {
		//HostSessionCall
		auto networkMgr = getNetworkManager();
		static auto AlsoCreatesSessionWithoutManyChecks = hook::pattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 8B F9 48 8B 89 ? ? ? ? 83 39 03 75 42 48 8D 9F ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8B CB 48 8B").count(1).get(0).get<void>();

		printf("NETWORK_SESSION_HOST \n");
		tryHosting = true;
		//reinterpret_cast<__int64(__fastcall*)(void* networkManager, __int64 a2)>(AlsoCreatesSessionWithoutManyChecks)(networkMgr, 1);
		//HostSessionInternal((__int64)networkMgr, 0, 32, 0x100 | 0x12);
		NETWORK::NETWORK_SESSION_HOST_CLOSED(0, 32);
		tryHosting = false;

		//printf("NETWORK_CAN_ENTER_MULTIPLAYER %s \n", NETWORK::NETWORK_CAN_ENTER_MULTIPLAYER()? "true" : "false");

		// reinterpret_cast<void(__fastcall*)(void* networkManager)>(createsession)(networkMgr);

		//reinterpret_cast<void(__fastcall*)(void* networkManager, unsigned __int16 a2, __int64 a3, unsigned int a4)>(HostSessionCall)(networkMgr, a2, 32, 512);

		//hostGame(0, 32, 0);
	//	printf("g_isNetGame equals to %d \n", *g_isNetGame);
	}


	//auto gi = *g_gamerInfo;
	//printf("gamer id %u \n", gi->gamerId);
	//spdlog::info("CNetworkForce::OnGameFrame");
}

CNetworkForce* g_NetworkForce = new CNetworkForce();