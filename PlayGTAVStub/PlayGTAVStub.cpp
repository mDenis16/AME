#include "pch.h"

#include <MinHook.h>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <Windows.h>
#include <cstdio>
#include <winternl.h>
#include <tlhelp32.h>

#include "PlayGTAVStub.h"
void LoadDll(std::string clientDll, DWORD dwProcessId) {
    HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    if (!Process) std::cout << "Failed to open process " << std::endl;

    // Allocate space in the process for our DLL 
    LPVOID Memory = LPVOID(VirtualAllocEx(Process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!Memory) std::cout << "Virtual alloc error " << std::endl;
    // Write the string name of our DLL in the memory allocated 
    if (!WriteProcessMemory(Process, Memory, clientDll.c_str(), clientDll.size(), nullptr))  std::cout << "WriteProcessMemory " << std::endl;

    // Load our DLL
    HANDLE hThread = CreateRemoteThread(Process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), Memory, NULL, nullptr);
    if (!hThread)  std::cout << "CreateRemoteThread" << std::endl;

    //Let the program regain control of itself.
    CloseHandle(Process);

    //Free the allocated memory.
    VirtualFreeEx(Process, LPVOID(Memory), 0, MEM_RELEASE);
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
static int call_count = 0;
static DWORD last_thread = 0;
static HANDLE MainThreadHandle = 0;
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

    //MessageBox(0, "PlayGTAV", "mata", MB_OK);
    if (lpApplicationName) {

        auto hHandle =  CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);

        if (hHandle)
        {
            auto launchPath = PlayGTAVStub::GetLaunchPath("Software\\AME");
            auto exeInj = launchPath.append("\\RoSLauncherInjector.exe");

            STARTUPINFO si;
            PROCESS_INFORMATION pi;

            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));
            std::wstring wsTmp(exeInj.begin(), exeInj.end());

            auto hInjector = CreateProcessW_Original(wsTmp.c_str(), NULL,
                NULL,
                FALSE,
                0,
                CREATE_NO_WINDOW,
                NULL,
                0,
                (LPSTARTUPINFOW)&si,
                &pi);

            auto ret = WaitForSingleObject(pi.hProcess, INFINITE);
            printf("WaitForSingleObject ret = %x\n", ret);
            if (ret == WAIT_OBJECT_0)
            {
                printf("WaitForSingleObject ret ret == WAIT_OBJECT_0\n");
            }
           // MessageBox(0, "PlayGTAVStub32::ResumeThread_HK", "Means that should start injector x64.", MB_OK);
            return hInjector;
        }

    }
    else {
        return CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }
  
}
typedef BOOL(WINAPI* __ResumeThreadTypedef)(_In_ HANDLE hThread);
__ResumeThreadTypedef ResumeThread_Original;
DWORD
WINAPI
ResumeThread_HK(
    _In_ HANDLE hThread
) {


    if (hThread == MainThreadHandle) {
       /* auto launchPath = PlayGTAVStub::GetLaunchPath("Software\\AME");
        auto exeInj = launchPath.append("\\RoSLauncherInjector.exe");

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        std::wstring wsTmp(exeInj.begin(), exeInj.end());

        auto hHandle = CreateProcessW(wsTmp.c_str(), NULL,
            NULL,
            FALSE,
            0,
            CREATE_NO_WINDOW,
            NULL,
            0,
            (LPSTARTUPINFOW)&si,
            &pi);

        auto ret = WaitForSingleObject(pi.hProcess, INFINITE);
        printf("WaitForSingleObject ret = %x\n", ret);
        if (ret == WAIT_OBJECT_0)
        {
            printf("WaitForSingleObject ret ret == WAIT_OBJECT_0\n");
        }
        MessageBox(0, "PlayGTAVStub32::ResumeThread_HK", "Means that should start injector x64.", MB_OK);*/
      
    }
    //

    return ResumeThread_Original(hThread);
 /*   auto launchPath = PlayGTAVStub::GetLaunchPath("Software\\AME");
    auto exeInj = launchPath.append("\\RoSLauncherInjector.exe");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    std::wstring wsTmp(exeInj.begin(), exeInj.end());

    CreateProcessW(wsTmp.c_str(), NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        0,
        (LPSTARTUPINFOW)&si,
        &pi);*/

}

std::string PlayGTAVStub::GetLaunchPath(std::string path) {
    std::string ret = std::string();

    HKEY   hKeySet;
    wchar_t mPath[256];

    DWORD val;
    DWORD dataSize = sizeof(val);
    //::
    LONG nError = RegOpenKeyExA(HKEY_CURRENT_USER,
        path.data(), NULL, KEY_ALL_ACCESS, &hKeySet);
    if (nError)
    {
        std::cout << "Could not open registry key, error: " << nError << std::endl;
        return ret;
    }
    else
    {
        LONG nError;
        //::Set value

        char path[MAX_PATH];
        try
        {

            DWORD dwBufSize = 0;
            LONG lRetVal = RegGetValue(hKeySet,
                NULL,
                "PATH",
                RRF_RT_REG_SZ,
                NULL,
                NULL,
                &dwBufSize);

            if (dwBufSize) {
                CHAR* ppBuf = new CHAR[dwBufSize];
                if (NULL == *ppBuf)
                    return std::string("");
                lRetVal = RegGetValue(hKeySet,
                    NULL,
                    "PATH",
                    RRF_RT_REG_SZ,
                    NULL,
                    ppBuf,
                    &dwBufSize);

                std::stringstream ss;
                ss << ppBuf;

                ret = ss.str();
            }



        }
        catch (...)
        {
            return ret;
        }


    }
    RegCloseKey(hKeySet);
    return ret;
}
void PlayGTAVStub::Hook() {

    if (MH_Initialize() != MH_OK)
    {
        return;
    }

    if (MH_CreateHookApi(
        L"kernel32.dll", "CreateProcessW", &CreateProcessW_HK, (LPVOID*)&CreateProcessW_Original) != MH_OK)
    {
        return;
    }

    if (MH_CreateHookApi(
        L"kernel32.dll", "ResumeThread", &ResumeThread_HK, (LPVOID*)&ResumeThread_Original) != MH_OK)
    {
        return;
    }
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        return;
    }

}
void PlayGTAVStub::Unhook() {

  /*  HMODULE hModule;
    LPVOID  pTarget;

    hModule = GetModuleHandleW(L"kernel32.dll");
    if (hModule == NULL)
        return;

    pTarget = (LPVOID)GetProcAddress(hModule, "ResumeThread");
    if (pTarget == NULL)
        return;

    MH_DisableHook(pTarget);
    MH_RemoveHook(pTarget);
    MH_Uninitialize();*/
}
void PlayGTAVStub::Initialize(HMODULE inst)
{
    instance = inst;
    Hook();
}

void PlayGTAVStub::Release()
{
    Unhook();
}
