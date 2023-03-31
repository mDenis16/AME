#include <Windows.h>
#include "CSocialClub.h"
#include <spdlog/spdlog.h>
#include "Hooking.h"
#include <MinHook.h>

void __fastcall DEFAULT_LOGGER(int log_type, const wchar_t* format, va_list argptr)
{
	spdlog::info("called DEFAULT_LOGGER");
	return;
	wchar_t dest[1024 * 16];
	int bytes_count =  _vsnwprintf(dest, sizeof(dest), format, argptr);

	auto str = std::wstring();
	str.resize(bytes_count);
	memcpy(str.data(), dest, bytes_count);
	const std::string s(str.begin(), str.end());
	spdlog::info("[SocialClub] {} ", s);
}
int __fastcall LOGS_A_LOT(char* Buffer, size_t BufferCount, char* Format, va_list ArgList) {
	char dest[1024 * 16];
	int res = vsprintf(Buffer, Format, ArgList);
	va_end(ArgList);
	spdlog::debug("[SC_DEBUG]S {}", Buffer);
	return res;
}
void AddLog(void* a1, const char* format, ...)
{
	char dest[1024 * 16];
	va_list argptr;
	va_start(argptr, format);
	vsprintf(dest, format, argptr);
	va_end(argptr);
	spdlog::info("[SocialClub] {} ", dest);
}
int __fastcall BigFunction(__int64 a1, unsigned int a2) {
	spdlog::info("called BigFunction ");
	return 0;
}
void CSocialClub::Hook() {
	if (!hModule) return;

	spdlog::info("CSocialClub::Hook");

	/*auto target = hook::module_pattern(hModule, "48 8B C4 48 89 58 18 55 57 41 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 4D 8B F1 49 8B D8 8B FA 0F B6 E9 0F 85 ? ? ? ?").count(1).get(0).get<void>();

	MH_CreateHook(target, InternalLogAdd, nullptr);*/

	auto target = hook::module_pattern(hModule, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 49 8B D9 49 8B F8 48 8B EA 48 8B F1 E8 ? ? ? ? 48 89 5C 24 ? 4C 8B CF 48 83 64 24 ? ? 4C 8B C5 48 8B D6 48 8B 08 48 83 C9 01").count(1).get(0).get<void>();

	MH_CreateHook(target, LOGS_A_LOT, nullptr);
	
	auto big_function_target = hook::module_pattern(hModule, "48 89 5C 24 ? 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 8B DA 4C 8D 35 ? ? ? ? 48 8B F9 48 8D 2D ? ? ? ? 45 33 FF 0F 1F 80 ? ? ? ?").count(1).get(0).get<void>();

	/*MH_CreateHook(big_function_target, BigFunction, nullptr);
	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		return;
	}*/
}