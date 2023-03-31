#include <glm/glm.hpp>
#include "CNativeInvoker.h"
#include "Hooking.h"
#include "CCrossmap.h"
#include <MinHook.h>
#include <fstream>

#include <spdlog/spdlog.h>
extern "C" void	_call_asm(void* context, void* function, void* ret);
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

ScrNativeHandler CNativeInvoker::GetNativeHandler(uint64_t oldHash) {

#if _DEBUG && 0
	static int dumped_native_count = DumpAllNativeHashAndHandlers();
#endif
	
	auto cachePair = foundHashCache.find(oldHash);
	if (cachePair != foundHashCache.end()) {


		uint64_t newHash = 0;

		for (auto const& mapping : CrossMapList)
		{
			if (mapping.first == oldHash)
			{
				newHash = mapping.second;
				break;
			}
		}

		return cachePair->second;
	}
	else {
		uint64_t newHash = 0;

		for (auto const& mapping : CrossMapList)
		{
			if (mapping.first == oldHash)
			{
				newHash = mapping.second;
				break;
			}
		}

		ScrNativeHandler nativeHandler = reinterpret_cast<ScrNativeHandler(__cdecl*)(NativeRegistration**, uint64_t oldHash)>(GetNativeHandleCall)(registrationTable, newHash);

		foundHashCache[oldHash] = nativeHandler;
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

	ScrNativeHandler nativeHandler =  reinterpret_cast<ScrNativeHandler(__cdecl*)(NativeRegistration**, uint64_t oldHash)>(GetNativeHandleCall)(registrationTable, newHash);

	foundHashCache[oldHash] = nativeHandler;*/
}
void CNativeInvoker::BeginCall()
{
	CallContext.Reset();
}bool IsErrorException(PEXCEPTION_POINTERS ep)
{
	return (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && ep->ExceptionRecord->ExceptionInformation[1] == 0xDEED);
}
static LONG ShouldHandleUnwind(PEXCEPTION_POINTERS ep, DWORD exceptionCode, uint64_t identifier)
{
	if (IsErrorException(ep))
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// C++ exceptions?
	if (exceptionCode == 0xE06D7363)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	//// INVOKE_FUNCTION_REFERENCE crashing as top-level is usually related to native state corruption,
	//// we'll likely want to crash on this instead rather than on an assertion down the chain
	//if (identifier == HashString("INVOKE_FUNCTION_REFERENCE"))
	//{
	//	return EXCEPTION_CONTINUE_SEARCH;
	//}

	return EXCEPTION_EXECUTE_HANDLER;
}
void CNativeInvoker::EndCall(ScrNativeHash hash)
{
	
		static void* exceptionAddress;

		auto it = GetNativeHandler(hash);
		static auto mata = hook::get_pattern("FF E3");
		
	
		ScrNativeHandler handler = it;

		if (handler) {


			


			[this, hash, handler]
			{
				__try
				{
					//_call_asm(&m_call_context, handler, g_pointers->m_native_return);
					//_call_asm(&CallContext, handler, mata);
					handler(&CallContext);
					CallContext.SetVectorResults();
					// handler(&m_call_context);
					//g_pointers->m_fix_vectors(&m_call_context);
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					[hash]() { spdlog::critical("Exception caught while trying to call {} native", hash); }();
				}
			}();
		}

}
static void(*g_RegisterNative)(void* a1, void* a2, void* a3);

//17249325974508376352
void CNativeInvoker::RegisterNativeHK(void* a1, void* a2, void* a3) {
	//return g_RegisterNative((void*)a1, a2);
	//MessageBox(0, "Called native ", "RegisterNativeHK::Hook", MB_ICONERROR);

	std::uint64_t nativeHash = (std::uint64_t)a2;
	
	std::uint64_t oldHash;
	for (auto const& mapping : CrossMapList)
	{
		if (mapping.second == nativeHash)
		{
			oldHash = mapping.first;
			break;
		}
	}
	

	CBaseFactory<CNativeInvoker>::Get().foundHashCache[oldHash] = (ScrNativeHandler)(a3);
	spdlog::debug("SCR_REGISTER: table {}, hash {}, handle {}", a1, oldHash, a3);

	return g_RegisterNative((void*)a1, a2, a3);
}


static int callCount = 0;
//__int64(__fastcall*** a1)()
static void(*g_HookThisShit)();

void CNativeInvoker::BeforeRegisteringNatives(void* a1) {
	
	auto RegisterSomeNativeCall = hook::pattern("48 8B C4 48 89 58 08 48 89 68 18 48 89 70 20 48 89 50 10 57 48 83 EC 20 0F B6 C2 49 8B F0 BD ? ? ? ? 48 8D 1C C1 BF ? ? ? ? 48 8B 13 48 85 D2 74 12 48 8D 42 48 8B 48 04 33 C8 8B 00 33 C1 83 F8 07 75 72").count(1).get(0).get<void>();

	if (MH_CreateHook(RegisterSomeNativeCall, &RegisterNativeHK, reinterpret_cast<void**>(&g_RegisterNative)) != MH_OK) {
		MessageBox(0, "Failed hooking RegisterSomeNativeCall1", "Error", MB_ICONERROR);
	}

	if (MH_EnableHook(RegisterSomeNativeCall) != MH_OK)
		MessageBox(0, "Failed enable RegisterSomeNativeCall2", "Error", MB_ICONERROR);

	return g_HookThisShit();
}
void CNativeInvoker::Hook() {
	
	FixVectors = hook::pattern("83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA").count(1).get(0).get<void>();

	auto GetNativeHandlerCaller = hook::pattern("E8 ? ? ? ? 48 85 C0 75 0A 48 8D 05 ? ? ? ? 40 32 F6 48 8B 4B 40 48 89 04 F9").count(1).get(0).get<void>();
	auto registrationTablePattern = hook::pattern("76 32 48 8B 53 40");
	auto location = registrationTablePattern.count(1).get(0).get<char>(9);
	if (location == nullptr) {

		printf("Unable to find registrationTable");
		return;
	}
	registrationTable = reinterpret_cast<decltype(registrationTable)>(location + *(int32_t*)location + 4);


	GetNativeHandleCall = hook::get_call(GetNativeHandlerCaller);

	//MessageBox(0, std::to_string(callCount).c_str(), "native invoker hook callcount", MB_OK);
	//callCount++;
//	auto RegisterNativeTarget = hook::pattern("48 8D 64 24 ? 48 89 14 24 4C 8B 04 24 48 8D 64 24 ? 51 5A 48 8D 0D ? ? ? ? 48 8D 64 24 ? 48 89 2C 24 48 8D 2D ? ? ? ? 48 87 2C 24 48 8D 64 24 ? FF 64 24 F8").count(1).get(0).get<void>();
	//MessageBox(0, "nu prea merge", "test", MB_OK);
	auto hkcaller = hook::pattern("4C 8B DC 48 83 EC 38 48 8B 05 ? ? ? ? 49 83 63 ? ? 48 8B 15 ? ? ? ? 48 8B 0D ? ?").count(1).get(0).get<void>();
	//auto caller = hook::get_call(hkcaller);

	if (MH_CreateHook(hkcaller, &CNativeInvoker::BeforeRegisteringNatives, reinterpret_cast<void**>(&g_HookThisShit)) != MH_OK) {
		std::cout << "Failed hooking caller" << std::endl;
	}

	////if (MH_CreateHook(RegisterNativeTarget, &RegisterNativeHK, reinterpret_cast<void**>(&g_RegisterNative)) != MH_OK) {
	////	std::cout << "Failed hooking RegisterNativeHK" << std::endl;
	////}

	if (MH_EnableHook(hkcaller) != MH_OK)
		MessageBox(0, "Failed hooking caller", "Error", MB_ICONERROR);


}

void CNativeInvoker::Unhook() {
	
}

void CNativeInvoker::DumpTable() {  
	return;
	std::ofstream natives("nativesdmp.txt");

	for (const ScrNativeMapping& mapping : CrossMapList)
	{
		ScrNativeHandler nativeHandler = reinterpret_cast<ScrNativeHandler(__cdecl*)(NativeRegistration**, uint64_t oldHash)>(GetNativeHandleCall)(registrationTable, mapping.second);

		natives << std::hex  << "NATIVE: OLD->" << mapping.first << " NEW->" << mapping.second << " handler->" << (std::uintptr_t)(nativeHandler) << std::endl;
	}


	printf("dumped all natives \n");

	
}
void CNativeInvoker::OnGameHook()
{



	DumpTable();
}
CNativeInvoker* g_NativeInvoker = new CNativeInvoker();