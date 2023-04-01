#include "CLevelLoader.h"
#include "Hooking.h"
#include <xlocbuf>

#include <MinHook.h>
#include "CBootstrap.h"
#include <spdlog/spdlog.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <winternl.h>

static std::string g_overrideNextLoadedLevel;
static std::string g_nextLevelPath;

static bool g_wasLastLevelCustom;
static bool g_gameUnloaded = false;

int ReturnFalse()
{
	return 0;
}

static bool WaitWithWait(HANDLE handle)
{
	return WaitForSingleObject(handle, 500) == WAIT_OBJECT_0;
}

void CLevelLoader::Hook()
{
	spdlog::info("CLevelLoader::Hook");
	
	char* location = hook::get_pattern<char>("0F 57 C9 48 2B C1 48 8D 0D", 9);
	uint32_t* legalScreenTime = (uint32_t*)(location + *(int32_t*)location + 4);

	*legalScreenTime = 0;

	{
		char* p = hook::pattern("BA 08 00 00 00 8D 41 FC 83 F8 01").count(1).get(0).get<char>(14);

		char* varPtr = p + 2;
		g_initState = (int*)(varPtr + *(int32_t*)varPtr + 4);
	}

	// stop the ros sdk input blocker
	hook::jump(hook::get_pattern("48 8B 01 FF 50 10 84 C0 74 05 BB 01 00 00 00 8A", -0x14), ReturnFalse);



	hook::put<uint32_t>(hook::get_pattern("84 C0 74 36 48 8B 0D ? ? ? ? 48 85 C9", -13), 0x90C301B0);

	char* p = hook::pattern("BA 08 00 00 00 8D 41 FC 83 F8 01").count(1).get(0).get<char>(14);

	char* varPtr = p + 2;
	g_initState = (int*)(varPtr + *(int32_t*)varPtr + 4);
	
	//{

	//	auto location = hook::get_pattern<char>("0F 95 05 ? ? ? ? E8 ? ? ? ? 48 85 C0");
	//	auto addy = hook::get_address<char*>(location + 3);
	//	hook::put<char>(addy, 0);
	//	hook::nop(location, 7);

	//	hook::put<uint32_t>(hook::get_pattern("48 81 C1 00 00 00 12 48 89 0D", 3), 0x36000000);
	//	hook::return_function(hook::get_pattern("45 33 E4 83 39 02 4C 8B FA 45 8D 6C", -0x1C));
	//}
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
	
	

	return g_origWaitUntilGameStarts();
}
typedef NTSTATUS(WINAPI* pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);
#define ThreadQuerySetWin32StartAddress 9

#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)

DWORD WINAPI GetThreadStartAddress(HANDLE hThread)
{
	NTSTATUS ntStatus;
	HANDLE hDupHandle;
	DWORD dwStartAddress;

	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread");

	if (NtQueryInformationThread == NULL)
		return 0;

	HANDLE hCurrentProcess = GetCurrentProcess();
	if (!DuplicateHandle(hCurrentProcess, hThread, hCurrentProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0)) {
		SetLastError(ERROR_ACCESS_DENIED);

		return 0;
	}

	ntStatus = NtQueryInformationThread(hDupHandle, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);
	CloseHandle(hDupHandle);
	if (ntStatus != STATUS_SUCCESS)
		return 0;

	return dwStartAddress;

}
void SuspendSocialClubThreads() {
	return;
	HANDLE hSnapT;
	HANDLE hT;
	THREADENTRY32 te32;

	if (INVALID_HANDLE_VALUE == (hSnapT = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)))
		return;

	te32.dwSize = sizeof(THREADENTRY32);
	if (FALSE == Thread32First(hSnapT, &te32)) {
		return;
	}

	while (TRUE) {
	
		hook::executable_meta social_club(GetModuleHandle("socialclub.dll"));

		if (GetCurrentProcessId() == te32.th32OwnerProcessID) {
			hT = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
			if (hT) {
				//auto startAddress = (std::uintptr_t)GetThreadStartAddress(hT);

				/*if (Tinfo >= social_club.begin() && Tinfo <= social_club.end()) {
					SuspendThread(hT);
					spdlog::info("suspended thread");
				}*/
				ULONG64 Tinfo = 0;
				ULONG retlen = 0;
				NTSTATUS ntqitret = NtQueryInformationThread(hT, (THREADINFOCLASS)9,
					&Tinfo, sizeof(PVOID), &retlen);
				//printf("START  = %I64x\nret    = %x\nretlen = %x\n", Tinfo, ntqitret, retlen);
				//spdlog::info("thread {} start address {:X}", te32.th32ThreadID, startAddress );
				if (Tinfo >= social_club.begin() && Tinfo <= social_club.end()) {
					SuspendThread(hT);
					spdlog::info("suspended thread");
				}
				CloseHandle(hT);
			}
		}
		if (FALSE == (Thread32Next(hSnapT, &te32)))
			break;
	}
}
void CLevelLoader::OnFrame() {
	if (CBaseFactory<CLevelLoader>::Get().ShouldLoad) {
		CBaseFactory<CLevelLoader>::Get().ShouldLoad = false;
		*CBaseFactory<CLevelLoader>::Get().g_initState = 7;
		CBaseFactory<CLevelLoader>::Get().state = GAME_STATE::IN_LOADING;
		return;
	}
	if (CBaseFactory<CLevelLoader>::Get().state == GAME_STATE::IN_LOADING && *CBaseFactory<CLevelLoader>::Get().g_initState == 0)
	{
		CBaseFactory<CLevelLoader>::Get().state = GAME_STATE::IN_GAME;
		SuspendSocialClubThreads();
		//ShowWindow(CBootstrap::instance->gameWindow, SW_SHOWDEFAULT);
	}


	static bool Requested = false;

	if (!Requested && *CBaseFactory<CLevelLoader>::Get().g_initState == 22) {
		CBaseFactory<CLevelLoader>::Get().RequestLoad();
		Requested = true;
	}

}
void CLevelLoader::OnGameHook() {

	

	//char* WaitUntilGameCaller = hook::pattern("E8 ? ? ? ? 8B CF E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ?").count(1).get(0).get<char>();
	//void* Waitcalll = hook::pattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? E8 ? ? ? ? 33 F6 48 8D").count(1).get(0).get<void>();

	////printf("Waitcalll: %i \n", (std::uintptr_t)(Waitcalll));


	//if (MH_CreateHook(Waitcalll, &WaitUntilGameStarts, reinterpret_cast<void**>(&g_origWaitUntilGameStarts)) != MH_OK) {
	//	std::cout << "Failed hooking run script threads " << std::endl;
	//}

	//if (MH_EnableHook(Waitcalll) != MH_OK)
	//	MessageBox(0, "Failed hooking Waitcalll thread. ", "CScriptVM::Hook", MB_ICONERROR);

} CLevelLoader* g_LevelLoader = new CLevelLoader();