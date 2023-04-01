// Launcher64NoSC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <filesystem>
#include <iostream>
#include <Windows.h>


void launch_no_sc() {
	auto clientDll = std::filesystem::current_path().string().append("\\Client.dll");


	std::string fileLocation = std::string("D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe");



	LPCSTR lpFileName = fileLocation.data();

	PROCESS_INFORMATION ProcessInformation;
	STARTUPINFOW StartupInfo;
	DWORD cb;

	cb = sizeof(STARTUPINFO);
	ZeroMemory(&StartupInfo, cb);
	StartupInfo.cb = cb;


	//auto result2 = CreateProcessW(L"C:\\Program Files\\Rockstar Games\\Launcher\\RockstarService.exe", NULL, NULL, FALSE, FALSE, CREATE_SUSPENDED, NULL, NULL, &StartupInfo2, &ProcessInformation2);

	wchar_t ProcW[] = L"\"D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe\"";
	//""D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe\" -skipPatcherCheck @args.txt @commandline.txt -enableCrashpad -useSteam -steamAppId=271590 -scCommerceProvider = 4"
	
		//		52	2:44 : 54.491 PM	7	Launcher.exe	CreateProcessW("D:\SteamLibrary\steamapps\common\Grand Theft Auto V\GTA5.exe", ""D:\SteamLibrary\steamapps\common\Grand Theft Auto V\GTA5.exe" -skipPatcherCheck -windowed @args.txt @commandline.txt -enableCrashpad -useSteam -steamAppId=271590 -scCommerceProvider=4 -scUseFQDNAvatars", 0x0000000000000000, 0x0000000000000000, 0, 0, 0x0000000000000000, "D:\SteamLibrary\steamapps\common\Grand Theft Auto V", 0x00000044bfdfef20, 0x00000044bfdfef00)	1		0.0052479

			//auto result = CreateProcessW(L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe", ProcW, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V", (LPSTARTUPINFOW)(&StartupInfo), &ProcessInformation);
			//auto result = CreateProcessW(L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe", ProcW, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT/*CREATE_SUSPENDED*/, NULL, L"D:\\SteamLibrary\\steamapps\\common\\Grand Theft Auto V", (LPSTARTUPINFOW)(&StartupInfo), &ProcessInformation);
		//	#	Time of Day	Thread	Module	API	Return Value	Error	Duration
			//	1	3:00 : 18.089 PM	1	steamclient.dll	CreateProcessW("D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe", ""D:\SteamLibrary\steamapps\common\Grand Theft Auto V\PlayGTAV.exe"", NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT, 0x0804c0a8, "D:\SteamLibrary\steamapps\common\Grand Theft Auto V", 0x0448f5e0, 0x0448f67c)	TRUE		0.0027508
		auto cmdline = R"("D:\SteamLibraryV2\steamapps\common\Grand Theft Auto V\GTA5.exe" @commandline.txt -enableCrashpad -scCommerceProvider=4 -rglLanguage=en-US -scUseFQDNAvatars")";
		
		auto mata = std::string(cmdline);
		std::wstring tactu(mata.begin(), mata.end());

		auto result = CreateProcessW(L"D:\\SteamLibraryV2\\steamapps\\common\\Grand Theft Auto V\\GTA5.exe", tactu.data(), NULL, NULL, FALSE, CREATE_SUSPENDED, 0x0000000000000000, L"D:\\SteamLibraryV2\\steamapps\\common\\Grand Theft Auto V", (LPSTARTUPINFOW)(&StartupInfo), &ProcessInformation);


	if (result == 0)
	{

		DWORD dw = GetLastError();
		std::cout << dw;
	}
	LPVOID Memory = LPVOID(VirtualAllocEx(ProcessInformation.hProcess, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
	if (!Memory) std::cout << "Virtual alloc error " << std::endl;
	// Write the string name of our DLL in the memory allocated 
	if (!WriteProcessMemory(ProcessInformation.hProcess, Memory, clientDll.c_str(), clientDll.size(), nullptr))  std::cout << "WriteProcessMemory " << std::endl;

	// Load our DLL
	HANDLE hThread = CreateRemoteThread(ProcessInformation.hProcess, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), Memory, NULL, nullptr);
	if (!hThread)  std::cout << "CreateRemoteThread" << std::endl;

	
	//Let the program regain control of itself.
	CloseHandle(ProcessInformation.hProcess);

	//Free the allocated memory.
	VirtualFreeEx(ProcessInformation.hProcess, LPVOID(Memory), 0, MEM_RELEASE);
	//OpenThread(THREAD_ALL_ACCESS, FALSE, ProcessInformation.hThread)
//	ResumeThread(ProcessInformation.hThread);
//	CloseHandle(ProcessInformation.hProcess);



}
int main()
{
	launch_no_sc();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
