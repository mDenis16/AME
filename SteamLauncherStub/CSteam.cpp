#include "pch.h"
#include "CSteam.h"
#include <MinHook.h>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <Windows.h>
#include <cstdio>
#include <winternl.h>
#include <tlhelp32.h>
#include <shellapi.h>
void LoadDll(std::string clientDll, DWORD dwProcessId) {
    HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    if (!Process) MessageBox(0, "Failed to OpenProcess", "CSteam::LoadDll", MB_OK);

    // Allocate space in the process for our DLL 
    LPVOID Memory = LPVOID(VirtualAllocEx(Process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!Memory) MessageBox(0, "Virtual alloc", "CSteam::LoadDll", MB_OK);
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

    //#	Time of Day	Thread	Module	API	Return Value	Error	Duration
  //  1	3:11 : 50.315 PM	1	steamclient.dll	CreateProcessW("D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe", ""D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe"", 0x00000000, 0x00000000, 0, 1028, 0x08034078, "D:\SteamLibrary\steamapps\common\Grand Theft Auto V", 0x0448f5e0, 0x0448f67c)	1		0.0032125

    auto str = std::wstring(lpApplicationName);

    auto result =  CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
        lpStartupInfo, lpProcessInformation);
    
    //MessageBox(0, "CSteam::CreateProcessHK", "Info!", MB_OK);
  

    auto str2 = std::string(str.begin(), str.end());



    Sleep(500); //MessageBox(0, "CSteam::CreateProcessHK", str2.data(), MB_OK);

    if (str.find(L"PlayGTAV") != std::wstring::npos)
    {
      /// MessageBox(0, "Found PlayGTAV", "", MB_OK);
        auto launchPath = CSteam::GetLaunchPath("Software\\AME");
        launchPath.append("\\PlayGTAVStub32.dll");
        
        //MessageBox(0, launchPath.c_str(), "CSteam::CreateProcess", MB_OK);

        LoadDll(launchPath, lpProcessInformation->dwProcessId);
    
        Sleep(500);
       
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, CSteam::instance, 0, NULL);
   
       // LoadDll("2", "2", 0);
    }
    

    return result;
}
std::string CSteam::GetSteamPath() {
    std::string ret = std::string();

    HKEY   hKeySet;
    wchar_t mPath[256];

    DWORD val;
    DWORD dataSize = sizeof(val);
    //::
    LONG nError = RegOpenKeyExA(HKEY_CURRENT_USER,
        "Computer\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam", NULL, KEY_ALL_ACCESS, &hKeySet);
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
                "InstallPath",
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
std::string CSteam::GetLaunchPath(std::string path) {
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
void CSteam::Hook() {

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
void CSteam::Unhook() {

    HMODULE hModule;
    LPVOID  pTarget;

    hModule = GetModuleHandleW(L"kernel32.dll");
    if (hModule == NULL)
        return;

    pTarget = (LPVOID)GetProcAddress(hModule, "CreateProcessW");
    if (pTarget == NULL)
        return;

    MH_DisableHook(pTarget);
    MH_RemoveHook(pTarget);
    MH_Uninitialize();
}
void CSteam::Initialize(HMODULE inst)
{
    instance = inst;
    Hook();

    auto steamPath = GetSteamPath();
    auto executeCommand = steamPath.append(" steam:://rungameid/271590");
    ShellExecute(0, 0, executeCommand.c_str(), 0, 0, SW_SHOW);
}

void CSteam::Release()
{
    Unhook();
}
