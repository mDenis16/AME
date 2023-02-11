#include <Windows.h>
#include <iostream>
#include <thread>
#include "CBootstrap.h"
#include "CVGame.h"
#include "IATHook.h"
#include <memory>
#include <functional>
#include <thread>
#include <MinHook.h>
#include "Hooking.h"
#include <fstream>
#include <winternl.h>
#include "CBaseFactory.h"

int CBootstrap::Main(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        bootThread = new std::thread(std::bind(&CBootstrap::Initialize, this));
    }
        break;
    case DLL_PROCESS_DETACH:
        OnDetach();
        break;
    }
    return 0;
}

void CBootstrap::InitializeConsole() {
    
}
LPVOID(WINAPI* OldHeapAlloc)(HANDLE, DWORD, SIZE_T);
void (CBootstrap::* memfun)(void);
static int WINAPI NoWindowsHookExA(int, HOOKPROC, HINSTANCE, DWORD)
{
    
    return 1;
}

BOOL
WINAPI
IsDebuggerPresent_HK(
    VOID
) {
    return FALSE;
}


void CBootstrap::Initialize()
{
    
    if (MH_Initialize() != MH_OK)
    {
        return;
    }
   
    if (MH_CreateHookApi(
        L"kernel32.dll", "GetStartupInfoW", &GetStartupInfoW_HK, nullptr) != MH_OK)
    {
        return;
    }
    if (MH_CreateHookApi(
        L"kernel32.dll", "IsDebuggerPresent", &IsDebuggerPresent_HK, nullptr) != MH_OK)
    {
        return;
    }

 
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        return ;
    }
  

}

void CBootstrap::OnAttach() {
 
   
}

void CBootstrap::OnDetach()
{
}
bool g_ranStartupInfo;
static bool ThisIsActuallyLaunchery()
{
  //  MessageBox(0, "CBoostrap::ThisIsActuallyLaunchery", "Info", MB_OK);
  //  hook::nop(0x7FF6256FE218, 1);
    //std::cout << "CBootstrap::ThisIsActuallyLaunchery " << std::endl;
    return true;
}
static void* DeleteVideo(void*, char* videoName)
{
    DWORD oldProtect;
    VirtualProtect(videoName, 4, PAGE_READWRITE, &oldProtect);
    strcpy(videoName, "nah");
    return nullptr;
}

typedef HWND(WINAPI* CreateWindowExW_t)(_In_ DWORD dwExStyle,
    _In_opt_ LPCWSTR lpClassName,
    _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam);
CreateWindowExW_t CreateWindowExW_Original;
HWND
WINAPI
CreateWindowExW_HK(
    _In_ DWORD dwExStyle,
    _In_opt_ LPCWSTR lpClassName,
    _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam)
{

    if (lstrcmpW(lpWindowName, L"Grand Theft Auto V") == 0) {
        CBootstrap::instance->gameWindow = CreateWindowExW_Original(dwExStyle, lpClassName, L"AME-V", WS_OVERLAPPEDWINDOW, X, Y, 800, 600, hWndParent, hMenu, hInstance, lpParam);
        static bool consoleCreated = false;
        if (!consoleCreated) {
            consoleCreated = true;
            AllocConsole();

            FILE* file = nullptr;
            freopen_s(&file, "conin$", "r", stdin);
            freopen_s(&file, "conout$", "w", stdout);
            freopen_s(&file, "conout$", "w", stderr);

            SetConsoleTitleA("VEngine Console");
        }
        return CBootstrap::instance->gameWindow;
    }
    
    return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL
WINAPI
FreeConsole_HK(
    VOID
) {
    return TRUE;
}
BOOL
WINAPI
AttachConsole_HK(
    _In_ DWORD dwProcessId
) {
    return TRUE;
}


typedef HMODULE(WINAPI* LoadLibraryA_t)(_In_ LPCSTR lpLibFileName);
LoadLibraryA_t LoadLibraryA_Original;
HMODULE
WINAPI
LoadLibraryA_HK(
    _In_ LPCSTR lpLibFileName
) {
    //auto str = std::string(lpLibFileName);
    if (lstrcmp(lpLibFileName, "DiscordHook64.dll") == 0) 
        return NULL;
    if (lstrcmp(lpLibFileName, "GameOverlayRenderer64.dll") == 0)
        return NULL;
  //  if (lstrcmp(lpLibFileName, "steam_api64.dll") == 0)
      //  return NULL;

    return LoadLibraryA_Original(lpLibFileName);
}

typedef HMODULE(WINAPI* LoadLibraryW_t)(_In_ LPCWSTR lpLibFileName);
LoadLibraryW_t LoadLibraryW_Original;
HMODULE
WINAPI
LoadLibraryW_HK(
    _In_ LPCWSTR lpLibFileName
) {
    if (lstrcmpW(lpLibFileName, L"DiscordHook64.dll") == 0)
        return NULL;
    if (lstrcmpW(lpLibFileName, L"GameOverlayRenderer64.dll") == 0)
        return NULL;
   // if (lstrcmpW(lpLibFileName, L"steam_api64.dll") == 0)
      //  return NULL;
    return LoadLibraryW_Original(lpLibFileName);
}

void CBootstrap::GetStartupInfoW_HK(LPSTARTUPINFOW lpStartupInfo) {
  
    
    if (g_ranStartupInfo) return;
    g_ranStartupInfo = true;

    MessageBox(0, "Time for debug.", "Hey!", MB_OK);


    PPEB peb = (PPEB)__readgsqword(0x60);
    peb->BeingDebugged = false;

    // set GlobalFlags
    *(DWORD*)((char*)peb + 0xBC) &= ~0x70;

    hook::set_base();

    hook::call(hook::pattern("84 C0 75 0C B2 01 B9 2F A9 C2 F4").count(1).get(0).get<void>(-5), ThisIsActuallyLaunchery);
    
    auto pattern = hook::pattern("FF 15 ? ? ? ? 84 C0 74 0C B2 01 B9 91 32 25");// 31 E8");
    if (pattern.size() > 0)
    {
        hook::nop(pattern.get(0).get<void>(0), 6);
        hook::put<uint8_t>(pattern.get(0).get<void>(8), 0xEB);
    }
  
  

    //std::cout << "hooked launchery \n";
    hook::call(hook::get_pattern("8B F8 48 85 C0 74 47 48 8B C8 E8 ? ? ? ? 4C", -6), DeleteVideo);

    // draw loading screen even if 'not' enabled
    hook::nop(hook::get_pattern("85 DB 0F 84 ? ? ? ? 8B 05 ? ? ? ? A8 01 75 15 83", 2), 6);

    if (MH_CreateHookApi(
        L"user32.dll", "SetWindowsHookExA", &NoWindowsHookExA, nullptr) != MH_OK)
    {
        return;
    }

    if (MH_CreateHookApi(
        L"user32.dll", "CreateWindowExW", &CreateWindowExW_HK, (LPVOID*)&CreateWindowExW_Original) != MH_OK)
    {
        return;
    }




    CBaseFactoryUtils::RunHook();


    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        MessageBox(0, "Failed hooking ", "Info!", MB_OK);
        return;
    }
}

