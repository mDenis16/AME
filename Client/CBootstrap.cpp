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
#include <spdlog/spdlog.h>
#include <TlHelp32.h>
#include "CSocialClub.h"
HANDLE hConOut = 0;

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


BOOL
WINAPI
FreeConsole_HK(
    VOID
) {
    MessageBox(0, "Free Console", "Muie valve si crucea mamii lor", MB_OK);
    return TRUE;
}


HANDLE
WINAPI
GetStdHandle_HK(
    _In_ DWORD nStdHandle
) {

    return hConOut;
}



BOOL
WINAPI
SetStdHandle_HK(
    _In_ DWORD nStdHandle,
    _In_ HANDLE hHandle
)
{
    //MessageBox(0, "SetStdHandle hoook", "MuieValve", MB_OK);
    return TRUE;
}

BOOL(WINAPI* o_PostMessageW)( HWND hWnd,
     UINT Msg,
     WPARAM wParam,
     LPARAM lParam);
BOOL
WINAPI
PostMessageW_HK(
    _In_opt_ HWND hWnd,
    _In_ UINT Msg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam) {

    if (Msg == WM_CLOSE) {
        return TRUE;
    }
    return o_PostMessageW(hWnd, Msg, wParam, lParam);
}


HWND
APIENTRY
GetConsoleWindow_KH(
    VOID
) {
    return 0;
}

BOOL(WINAPI* o_CloseHandle)(_In_ _Post_ptr_invalid_ HANDLE hObject);
BOOL
WINAPI
CloseHandle_HK(
    _In_ _Post_ptr_invalid_ HANDLE hObject
) {
    BOOL result = true;
    __try
    {
        result = o_CloseHandle(hObject);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        printf("Closed invalid handle... \n");
    }
    return result;
}
#pragma pack(push, 1)
struct DbgUiRemoteBreakinPatch
{
    WORD  push_0;
    BYTE  push;
    DWORD CurrentPorcessHandle;
    BYTE  mov_eax;
    DWORD TerminateProcess;
    WORD  call_eax;
};
#pragma pack(pop)
void Patch_DbgUiRemoteBreakin()
{
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll)
        return;

    FARPROC pDbgUiRemoteBreakin = GetProcAddress(hNtdll, "DbgUiRemoteBreakin");
    if (!pDbgUiRemoteBreakin)
        return;

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32)
        return;

    FARPROC pTerminateProcess = GetProcAddress(hKernel32, "TerminateProcess");
    if (!pTerminateProcess)
        return;

    DbgUiRemoteBreakinPatch patch = { 0 };
    patch.push_0 = '\x6A\x00';
    patch.push = '\x68';
    patch.CurrentPorcessHandle = 0xFFFFFFFF;
    patch.mov_eax = '\xB8';
    patch.TerminateProcess = (DWORD)pTerminateProcess;
    patch.call_eax = '\xFF\xD0';

    DWORD dwOldProtect;
    if (!VirtualProtect(pDbgUiRemoteBreakin, sizeof(DbgUiRemoteBreakinPatch), PAGE_READWRITE, &dwOldProtect))
        return;

    ::memcpy_s(pDbgUiRemoteBreakin, sizeof(DbgUiRemoteBreakinPatch),
        &patch, sizeof(DbgUiRemoteBreakinPatch));
    VirtualProtect(pDbgUiRemoteBreakin, sizeof(DbgUiRemoteBreakinPatch), dwOldProtect, &dwOldProtect);
   
}
void Patch_DbgBreakPoint()
{
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll)
        return;

    FARPROC pDbgBreakPoint = GetProcAddress(hNtdll, "DbgBreakPoint");
    if (!pDbgBreakPoint)
        return;

    DWORD dwOldProtect;
    if (!VirtualProtect(pDbgBreakPoint, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        return;

    *(PBYTE)pDbgBreakPoint = (BYTE)0xC3; // ret
}
static char(*mata)(void*, void*);
void MATA(void* a1, void* a2) {
    spdlog::info("Called arxan decrypt.");
}
void ResumeGame() {

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
        if (GetCurrentProcessId() == te32.th32OwnerProcessID) {
            if (NULL == (hT = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID))) {
                return ;
            }

           
            if (-1 == ResumeThread(hT)) {
                CloseHandle(hT);
                return;
            }
            

            CloseHandle(hT);
        }
        if (FALSE == (Thread32Next(hSnapT, &te32)))
            break;
    }
}
typedef BOOL(WINAPI* __CPW)(_In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation);
__CPW CreateProcessW_Original;
BOOL
WINAPI
CreateProcessW_HK(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
   // return FALSE;
    std::wcout << "CreateProcessW_HK " << " app name=" << std::wstring(lpApplicationName) << "command line="  << std::wstring(lpCommandLine);
    return CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

typedef HMODULE(WINAPI* __LLEXW)(_In_ LPCWSTR lpLibFileName,
    _Reserved_ HANDLE hFile,
    _In_ DWORD dwFlags);
__LLEXW LoadLibraryExW_Original;

HMODULE
WINAPI
LoadLibraryExW_HK(
    _In_ LPCWSTR lpLibFileName,
    _Reserved_ HANDLE hFile,
    _In_ DWORD dwFlags
) {
    if (std::wstring(lpLibFileName).find(L"socialclub.dll") != std::wstring::npos) {
        spdlog::info("Skipped loading socialclub.dll ");
        auto hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
        auto sc = new CSocialClub(hModule);
        sc->Hook();
        return hModule;
    }
    return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
}
//C:\Users\adm\Documents\Rockstar Games\Social Club\Profiles\autosignin.dat
typedef HANDLE(WINAPI* __CFW)(_In_ LPCWSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile);
__CFW CreateFileW_Original;
HANDLE
CreateFileW_HK(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile
) {
    
    auto handle = CreateFileW_Original(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    if (std::wstring(lpFileName).find(L"autosignin.dat") != std::wstring::npos) {
        spdlog::info("Found auto sign data");
    }
    return handle;
}
void CBootstrap::Initialize()
{
    
    
    //Patch_DbgBreakPoint();
   // Patch_DbgUiRemoteBreakin();

 /*   MessageBox(0, "Created thread inside GTA5", "MEARSA", MB_OK);*/
   

    if (MH_Initialize() != MH_OK)
    {
        return;
    }
   

   /* if (MH_CreateHookApi(
        L"kernel32.dll", "CreateFileW", &CreateFileW_HK, (LPVOID*)&LoadLibraryExW_Original) != MH_OK)
    {
        return;
    }*/
    
  /*  if (MH_CreateHookApi(
        L"kernel32.dll", "LoadLibraryExW", &LoadLibraryExW_HK, (LPVOID*)&LoadLibraryExW_Original) != MH_OK)
    {
        return;
    }*/
    /*if (MH_CreateHookApi(
        L"kernel32.dll", "CreateProcessW", &CreateProcessW_HK, (LPVOID*)&CreateProcessW_Original) != MH_OK)
    {
        return;
    }*/

    if (MH_CreateHookApi(
        L"kernel32.dll", "GetStdHandle", &GetStdHandle_HK, nullptr) != MH_OK)
    {
        return;
    }

    if (MH_CreateHookApi(
        L"kernel32.dll", "CloseHandle", &CloseHandle_HK, (LPVOID*)&o_CloseHandle) != MH_OK)
    {
        return;
    }
 
    if (MH_CreateHookApi(
        L"kernel32.dll", "GetStartupInfoW", &GetStartupInfoW_HK, nullptr) != MH_OK)
    {
        return;
    }

    if (MH_CreateHookApi(
        L"kernel32.dll", "GetConsoleWindow", &GetConsoleWindow_KH, nullptr) != MH_OK)
    {
        return;
    }
    if (MH_CreateHookApi(
        L"kernel32.dll", "FreeConsole", &FreeConsole_HK, nullptr) != MH_OK)
    {
        return;
    }
    /*if (MH_CreateHookApi(
        L"kernel32.dll", "SetStdHandle", &SetStdHandle_HK, nullptr) != MH_OK)
    {
        return;
    }*/
    
    if (MH_CreateHookApi(
        L"kernel32.dll", "IsDebuggerPresent", &IsDebuggerPresent_HK, nullptr) != MH_OK)
    {
        return;
    }

 
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        return ;
    }
    ResumeGame();
  

}

void CBootstrap::OnAttach() {
 
   
}

void CBootstrap::OnDetach()
{
}
bool g_ranStartupInfo;
static bool ThisIsActuallyLaunchery()
{
    /*MessageBox(0, "CBoostrap::ThisIsActuallyLaunchery", "Info", MB_OK);
    hook::nop(0x7FF6256FE218, 1);
    std::cout << "CBootstrap::ThisIsActuallyLaunchery " << std::endl;*/
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
        
        return CBootstrap::instance->gameWindow;
    }
    
    return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
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

    if (!hConOut) {
        AllocConsole();

        hConOut = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        FILE* fDummy;
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);

        SetStdHandle(STD_OUTPUT_HANDLE, hConOut);

        SetConsoleTitleA("AME-Console");
    }


    hook::set_base();
 
    spdlog::set_level(spdlog::level::debug);

    spdlog::trace("GetStartupInfoW_HK");

    MessageBox(0, "Time for debug.", "Hey!", MB_OK);
    spdlog::info("mata");

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
    
    //MessageBox(0, "crucea", "matii", MB_OK);
  

    CBaseFactoryUtils::RunHook();


    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        MessageBox(0, "Failed hooking ", "Info!", MB_OK);
        return;
    }
}

