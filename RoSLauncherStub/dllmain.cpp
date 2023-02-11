// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "RoSLauncher.h"

unsigned long WINAPI initialize(void* instance) {



    RoSLauncher::Initialize((HMODULE)instance);
    return 0;
}

void release() {

}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (auto handle = CreateThread(nullptr, NULL, initialize, hModule, NULL, nullptr))
            CloseHandle(handle);
        break;
    case DLL_PROCESS_DETACH:
        RoSLauncher::Release();
        break;
    }
    return TRUE;
}
