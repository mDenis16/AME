#pragma once
#include <thread>
class CBootstrap
{
public:
	CBootstrap() { instance = this;  };

	int Main(HMODULE hModule,
		DWORD  ul_reason_for_call,
		LPVOID lpReserved);
	void OnAttach();
	void OnDetach();
	void Initialize();
	void InitializeConsole();
	static void GetStartupInfoW_HK(LPSTARTUPINFOW lpStartupInfo);
	HWND gameWindow = NULL;
	inline static CBootstrap* instance;
private:
	std::thread* bootThread = nullptr;
};

