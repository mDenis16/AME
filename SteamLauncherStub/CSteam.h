#pragma once
#include <string>
class CSteam
{
public:
	static void Hook();
	static void Unhook();
	static void Initialize(HMODULE inst);
	static void Release();
	static std::string GetSteamPath();
	static std::string GetLaunchPath(std::string path);
	inline static HMODULE instance;
};

