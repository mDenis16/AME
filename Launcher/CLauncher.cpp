#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <Windows.h>
#include <cstdio>
#include <winternl.h>
#include <tlhelp32.h>
#include "CLauncher.h"

std::string GetSteamPath() {
	std::string ret = std::string();

	HKEY   hKeySet;
	wchar_t mPath[256];

	DWORD val;
	DWORD dataSize = sizeof(val);
	//::
	LONG nError = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\WOW6432Node\\Valve\\Steam", NULL, KEY_ALL_ACCESS, &hKeySet);
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
					"InstallPath",
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

void CLauncher::Launch() {

	auto clientDll = std::filesystem::current_path().string().append("\\Client.dll");

	
	fileLocation = std::string("D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe");
	
	
	
		LPCSTR lpFileName = fileLocation.data();

		PROCESS_INFORMATION ProcessInformation;
		STARTUPINFOW StartupInfo;
		DWORD cb;

		cb = sizeof(STARTUPINFO);
		ZeroMemory(&StartupInfo, cb);
		StartupInfo.cb = cb;

		PROCESS_INFORMATION ProcessInformation2;
		STARTUPINFOW StartupInfo2;
		DWORD cb2;

		cb2 = sizeof(STARTUPINFO);
		ZeroMemory(&StartupInfo2, cb2);
		StartupInfo.cb = cb2;

		//auto result2 = CreateProcessW(L"C:\\Program Files\\Rockstar Games\\Launcher\\RockstarService.exe", NULL, NULL, FALSE, FALSE, CREATE_SUSPENDED, NULL, NULL, &StartupInfo2, &ProcessInformation2);
	
		wchar_t ProcW[] = L"\"D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe\"";
		//""D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe\" -skipPatcherCheck @args.txt @commandline.txt -enableCrashpad -useSteam -steamAppId=271590 -scCommerceProvider = 4"
		//auto result  = CreateProcessW(L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe", ProcW, NULL, NULL, FALSE, 0/*CREATE_SUSPENDED*/, NULL, L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V", (LPSTARTUPINFOW)(&StartupInfo), &ProcessInformation);
		auto result = CreateProcessW(L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\PlayGTAV.exe", ProcW, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT/*CREATE_SUSPENDED*/, NULL, L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V", (LPSTARTUPINFOW)(&StartupInfo), &ProcessInformation);
	//	#	Time of Day	Thread	Module	API	Return Value	Error	Duration
		//	1	3:00 : 18.089 PM	1	steamclient.dll	CreateProcessW("D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe", ""D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe"", NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT, 0x0804c0a8, "D:\SteamLibrary\steamapps\common\Grand Theft Auto V", 0x0448f5e0, 0x0448f67c)	TRUE		0.0027508

	
		if (result == 0)
		{

			DWORD dw = GetLastError();
			std::cout << dw;
		}
		return;

		HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInformation.dwProcessId);

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
std::string GetLaunchPath(std::string path) {
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
DWORD CLauncher::GetSteam() {
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (std::string(process.szExeFile).compare("steam.exe") == 0)
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);
	
	return pid;
}

void CLauncher::SetLaunchPath(std::string path) {
	HKEY   hKeySet;
	wchar_t mPath[256];


	//::
	LONG nError = RegOpenKeyExA(HKEY_CURRENT_USER,
	 path.data(), NULL, KEY_ALL_ACCESS, &hKeySet);
	if (nError)
	{
		std::cout << "Could not open registry key, error: " << nError << std::endl;
		return;
	}
	else
	{
		LONG nError;
		//::Set value
		auto data = std::filesystem::current_path().string();
		try
		{
			nError = RegSetValueEx(hKeySet, "PATH", 0, REG_SZ, (LPBYTE)(data.c_str()),
				(data.size() + 1) * sizeof(char));
		}
		catch (...)
		{
			return;
		}

		//::Checking Result
		if (nError)
		{
			std::cout << "Set Registry value Error: " << nError << std::endl;
			return;
		}
	}
	RegCloseKey(hKeySet);
}
void CLauncher::StubSteam() {



	SetLaunchPath("Software\\AME");
	//auto launchPath = GetLaunchPath("Software\\AME");

	auto steamProc = GetSteam();

	auto steamStubFile = std::filesystem::canonical("SteamLauncherStub32.dll");
	if (steamProc)
		LoadDll(steamStubFile.string(), steamProc);


	std::string samp("\"");
	//"C:\Program Files (x86)\Steam\steam.exe" steam://rungameid/271590
	auto steamPath = samp + GetSteamPath();


	steamPath.append("\\steam.exe\"");
	steamPath.append(" ");
	steamPath.append("steam://rungameid/271590");	system(steamPath.c_str());
}