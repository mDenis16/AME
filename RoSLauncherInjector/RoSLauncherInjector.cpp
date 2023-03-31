// RoSLauncherInjector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>
#include <TlHelp32.h>

void LoadDll(std::string clientDll, DWORD dwProcessId) {
    HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    if (!Process) MessageBox(0, "Failed to OpenProcess", "RoSLauncherInjector::LoadDll", MB_OK);

    // Allocate space in the process for our DLL 
    LPVOID Memory = LPVOID(VirtualAllocEx(Process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!Memory) MessageBox(0, "Virtual alloc", "RoSLauncherInjector::LoadDll", MB_OK);
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
DWORD GetLauncher() {
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
            if (std::string(process.szExeFile).compare("Launcher.exe") == 0)
            {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    return pid;
}
int main()
{
    //MessageBox(0, "RoSLauncherInjector::Main", "Means that injector started.", MB_OK);


    auto launchPath = GetLaunchPath("Software\\AME");
    launchPath.append("\\RoSLauncherStub.dll");
    DWORD launcher = 0;
    int Tries = 0;
    while (!launcher && Tries < 500) {
        launcher = GetLauncher();
        std::string str{ "launcher " };
        str.append(std::to_string(launcher).c_str());


        if (launcher)
        {
           // MessageBox(0, str.c_str(), "mata", MB_OK);
            LoadDll(launchPath, launcher);
        }
    }
    Tries++;
    Sleep(70);
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
