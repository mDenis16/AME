#include <glm/glm.hpp>
#include "CNativeInvoker.h"
#include "Hooking.h"
#include "CCrossmap.h"
#include <MinHook.h>
#include <fstream>
#include <string>

NativeCallContext::NativeCallContext()
{
	ReturnVal = &ReturnStack[0];
	Args = &ArgsStack[0];
}
uint64_t CNativeInvoker::GetNewHashFromOldHash(uint64_t oldHash) {

	if (SearchDepth == 0) {
		std::cout << "SearchDepth  is 0 " << std::endl;
		// no need for conversion
		return oldHash;
	}


	for (int i = 0; i < fullHashMapCount; i++) {
		for (int j = 0; j <= SearchDepth; j++) {
			if (fullHashMap[i][j] == oldHash) {
				// found
				for (int k = SearchDepth; k > j; k--) {
					uint64_t newHash = fullHashMap[i][k];
					if (newHash == 0)
						continue;
					return newHash;
				}

				return oldHash;
			}
		}
	}
	return 0;
}

RAGE::ScrNativeHandler CNativeInvoker::GetNativeHandler(uint64_t oldHash) {

#if _DEBUG && 0
	static int dumped_native_count = DumpAllNativeHashAndHandlers();
#endif

	auto cachePair = foundHashCache.find(oldHash);
	if (cachePair != foundHashCache.end()) {
		return cachePair->second;
	}
	/*uint64_t newHash = 0;

	for (auto const& mapping : CrossMapList)
	{
		if (mapping.first == oldHash)
		{
			newHash = mapping.second;
			break;
		}
	}

	RAGE::ScrNativeHandler nativeHandler =  reinterpret_cast<RAGE::ScrNativeHandler(__cdecl*)(NativeRegistration**, uint64_t oldHash)>(GetNativeHandleCall)(registrationTable, newHash);

	foundHashCache[oldHash] = nativeHandler;*/
	return nullptr;
}
void CNativeInvoker::BeginCall()
{
	CallContext.Reset();
}

void CNativeInvoker::EndCall(RAGE::ScrNativeHash hash)
{

	auto it = GetNativeHandler(hash);

		RAGE::ScrNativeHandler handler = it;

		if (handler) {
			handler(&CallContext);

			reinterpret_cast<void(__cdecl*)(NativeCallContext*)>(FixVectors)(&CallContext);
		}

	//else
	//{
	//	printf("Failed to find hash native's handler. \n");
	//	//[hash]() { log(WARNING) << "Failed to find " << HEX_TO_UPPER(hash) << " native's handler."; }();
	//}
}
static void(*g_RegisterNative)(void* a1, void* a2);

//17249325974508376352
void CNativeInvoker::RegisterNativeHK(void* a1, void* a2) {
	//MessageBox(0, "Called native ", "RegisterNativeHK::Hook", MB_ICONERROR);

	std::uint64_t nativeHash = (std::uint64_t)a1;
	
	std::uint64_t oldHash;
	for (auto const& mapping : CrossMapList)
	{
		if (mapping.second == nativeHash)
		{
			oldHash = mapping.first;
			break;
		}
	}
	CBaseFactory<CNativeInvoker>::Get().foundHashCache[oldHash] = (RAGE::ScrNativeHandler)(a2);

	if (nativeHash == 0xA0265306DFF63938) {
		printf("Found SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME handle %p\n", a2);
	}
	//	
	//	return g_RegisterNative((void*)nativeHash, a2);
	//}
	printf("Created native old hash %u new hash %u, handle %p \n", oldHash, nativeHash, a2);

	return g_RegisterNative((void*)nativeHash, a2);
}


static int callCount = 0;
//__int64(__fastcall*** a1)()
static void(*g_HookThisShit)(void* a1);

void __fastcall CNativeInvoker::BeforeRegisteringNatives(void* a1) {
	MessageBox(0, "HookThisShit", "test", MB_OK);
	auto RegisterSomeNativeCaller = hook::pattern("E8 ? ? ? ? 90 E9 ? ? ? ? 83 38 00 8B 50 08 E9 ? ? ? ? E8 ? ? ? ? 90 E9 ? ? ? ?").count(1).get(0).get<void>();

	auto RegisterSomeNativeCall = hook::get_call(RegisterSomeNativeCaller);

	if (MH_CreateHook(RegisterSomeNativeCall, &RegisterNativeHK, reinterpret_cast<void**>(&g_RegisterNative)) != MH_OK) {
		MessageBox(0, "Failed hooking RegisterSomeNativeCall1", "Error", MB_ICONERROR);
	}

	if (MH_EnableHook(RegisterSomeNativeCall) != MH_OK)
		MessageBox(0, "Failed enable RegisterSomeNativeCall2", "Error", MB_ICONERROR);

	return g_HookThisShit(a1);
}
void CNativeInvoker::Hook() {
	



	auto GetNativeHandlerCaller = hook::pattern("E8 ? ? ? ? 48 85 C0 75 0A 48 8D 05 ? ? ? ? 40 32 F6 48 8B 4B 40 48 89 04 F9").count(1).get(0).get<void>();

	GetNativeHandleCall = hook::get_call(GetNativeHandlerCaller);

	//MessageBox(0, std::to_string(callCount).c_str(), "native invoker hook callcount", MB_OK);
	//callCount++;
//	auto RegisterNativeTarget = hook::pattern("48 8D 64 24 ? 48 89 14 24 4C 8B 04 24 48 8D 64 24 ? 51 5A 48 8D 0D ? ? ? ? 48 8D 64 24 ? 48 89 2C 24 48 8D 2D ? ? ? ? 48 87 2C 24 48 8D 64 24 ? FF 64 24 F8").count(1).get(0).get<void>();
	//MessageBox(0, "nu prea merge", "test", MB_OK);
	auto hkcaller = hook::pattern("E8 ? ? ? ? E8 ? ? ? ? 90 E9 ? ? ? ? 48 8B 51 10 48 8D 0D ? ? ? ? 41 B8 ? ? ? ? 48 8B 12 48 89 6C 24 ? 48 8D 64 24 ? 48 8D 2D ? ? ? ? 48 87 2C 24 48 8D 64 24 ? FF 64 24 F8").count(1).get(0).get<void>();
	auto caller = hook::get_call(hkcaller);

	if (MH_CreateHook(caller, &CNativeInvoker::BeforeRegisteringNatives, reinterpret_cast<void**>(&g_HookThisShit)) != MH_OK) {
		std::cout << "Failed hooking caller" << std::endl;
	}

	////if (MH_CreateHook(RegisterNativeTarget, &RegisterNativeHK, reinterpret_cast<void**>(&g_RegisterNative)) != MH_OK) {
	////	std::cout << "Failed hooking RegisterNativeHK" << std::endl;
	////}

	if (MH_EnableHook(caller) != MH_OK)
		MessageBox(0, "Failed hooking caller", "Error", MB_ICONERROR);


}

void CNativeInvoker::Unhook() {
	
}

void CNativeInvoker::DumpTable() {  
	return;
	std::ofstream natives("nativesdmp.txt");

	for (const RAGE::ScrNativeMapping& mapping : CrossMapList)
	{
		RAGE::ScrNativeHandler nativeHandler = reinterpret_cast<RAGE::ScrNativeHandler(__cdecl*)(NativeRegistration**, uint64_t oldHash)>(GetNativeHandleCall)(registrationTable, mapping.second);

		natives << std::hex  << "NATIVE: OLD->" << mapping.first << " NEW->" << mapping.second << " handler->" << (std::uintptr_t)(nativeHandler) << std::endl;
	}


	printf("dumped all natives \n");

	
}
void CNativeInvoker::OnGameHook()
{
	printf("CNativeInvoker::Hook \n");
	//auto RegisterNative = hook::pattern("E8 ? ? ? ? 90 E9 ? ? ? ? 90 48 89 6C 24 ? E9 ? ? ? ?").count(1).get(0).get<char*>();
	//auto RegisterNativeCall = hook::get_call(RegisterNative);


	//if (MH_CreateHook(RegisterNativeCall, &RegisterNativeHK, reinterpret_cast<void**>(&g_RegisterNative)) != MH_OK) {
	//	std::cout << "Failed hooking run script threads " << std::endl;
	//}

	//if (MH_EnableHook(RegisterNativeCall) != MH_OK)
	//	MessageBox(0, "Failed hooking RegisterNativeCall. ", "CScriptVM::Hook", MB_ICONERROR);


	auto registrationTablePattern = hook::pattern("76 32 48 8B 53 40");
	auto location = registrationTablePattern.count(1).get(0).get<char>(9);
	if (location == nullptr) {

		printf("Unable to find registrationTable");
		return;
	}
	registrationTable = reinterpret_cast<decltype(registrationTable)>(location + *(int32_t*)location + 4);

	FixVectors = hook::pattern("83 79 18 00 48 8B D1 74 4A FF 4A 18").count(1).get(0).get<void>();
	DumpTable();
}
CNativeInvoker* g_NativeInvoker = new CNativeInvoker();