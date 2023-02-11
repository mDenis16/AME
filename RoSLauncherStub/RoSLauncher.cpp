#include "pch.h"


#include <MinHook.h>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <Windows.h>
#include <cstdio>
#include <winternl.h>
#include <tlhelp32.h>
#include "RoSLauncher.h"
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
    BOOL result = CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
        lpStartupInfo, lpProcessInformation);
    auto str = std::wstring(lpApplicationName);

    if (str.find(L"GTA5.exe") != std::wstring::npos) {
          auto launchPath = RoSLauncher::GetLaunchPath("Software\\AME");
          auto dllPath = launchPath.append("\\Client.dll");
          LoadDll(dllPath, lpProcessInformation->dwProcessId);
    }
    
    return result;
    //}
  //  if (lpProcessInformation)
   //   last_thread = lpProcessInformation->dwThreadId;

    // std::string samp = std::string("PlayGTAVStub::CreateProcessHK ").append(str2).append(" Creation flags:").append(std::to_string(dwCreationFlags));
    // MessageBox(0, samp.c_str(), "PlayGTAVStub.cpp", MB_OK);

     //{
     //   // MessageBox(0, "Found GTAV Launcher", "mata", MB_OK);
     //  /*  auto launchPath = PlayGTAVStub::GetLaunchPath("Software\\AME");
     //    auto exeInj = launchPath.append("\\RoSLauncherInjector.exe");

     //    STARTUPINFO si;
     //    PROCESS_INFORMATION pi;

     //    ZeroMemory(&si, sizeof(si));
     //    si.cb = sizeof(si);
     //    ZeroMemory(&pi, sizeof(pi));
     //    std::wstring wsTmp(exeInj.begin(), exeInj.end());*/


     //   // // Sleep(15);
     //   ////  MessageBox(0, "PlayGTAVStub32::CreateProcessW_HK", "Means that should start injector x64.", MB_OK);
     //   // CreateProcessW_Original(wsTmp.c_str(), NULL,           // Process handle not inheritable
     //   //     NULL,           // Thread handle not inheritable
     //   //     FALSE,          // Set handle inheritance to FALSE
     //   //     0,              // No creation flags
     //   //     NULL,           // Use parent's environment block
     //   //     NULL,           // Use parent's starting directory 
     //   //     0,
     //   //     (LPSTARTUPINFOW)&si,            // Pointer to STARTUPINFO structure
     //   //     &pi);      // Pointer to PROCESS_INFORMATION structure);
     //   // //    LoadDll(launchPath, lpProcessInformation->dwProcessId);
     //   //   //  MessageBox(0, "PlayGTAV started injector...", "Detached", MB_OK);


     //} return result;

}

std::string RoSLauncher::GetLaunchPath(std::string path) {
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
void RoSLauncher::Hook() {

    if (MH_Initialize() != MH_OK)
    {
        return;
    }

    if (MH_CreateHookApi(
        L"kernel32.dll", "CreateProcessW", &CreateProcessW_HK, (LPVOID*)&CreateProcessW_Original) != MH_OK)
    {
        return;
    }

 
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        return;
    }

}
void RoSLauncher::Unhook() {

      HMODULE hModule;
      LPVOID  pTarget;

      hModule = GetModuleHandleW(L"kernel32.dll");
      if (hModule == NULL)
          return;

      pTarget = (LPVOID)GetProcAddress(hModule, "RoSLauncher");
      if (pTarget == NULL)
          return;

      MH_DisableHook(pTarget);
      MH_RemoveHook(pTarget);
      MH_Uninitialize();
}
void RoSLauncher::Initialize(HMODULE inst)
{
    instance = inst;
    Hook();
}

void RoSLauncher::Release()
{
    Unhook();
}
