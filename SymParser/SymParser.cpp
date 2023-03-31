// SymParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>

#include "Sym.h"
#include <string>
//uint64_t get_ntoskrnl_function_address(LPCWSTR function_name)
//{
//    SymParser Parser;
//    if (!Parser.IsInitialized())
//        throw std::runtime_error("Unable to initialize DbgHelp!");
//
//    WCHAR system_dir[256];
//    GetWindowsDirectoryW(system_dir, 256);
//
//    std::wstring sz_ntoskrnl = (std::wstring(system_dir) + (L"\\System32\\ntoskrnl.exe"));
//    Parser.LoadModule(sz_ntoskrnl.c_str());
//
//    SymParser::SYM_INFO Info = {};
//    Parser.DumpSymbol(function_name, Info);
//
//    DWORD size;
//    uint64_t base;
//
//    driver().get_kernel_information_ex(&base, &size);
//
//    uint64_t start_base = 0x40000000;
//    uint64_t calc_address = Info.Offset - start_base;
//    uint64_t original_address = calc_address + base;
//
//    return original_address;
//}
int main()
{
    SymParser Parser;
    if (!Parser.IsInitialized())
        throw std::runtime_error("Unable to initialize DbgHelp!");

    // Download PDB and load it to parser:
    Parser.LoadModule(L"C:\\Windows\\System32\\ntdll.dll");

    // Dump you want:
    //SymParser::SYM_INFO Info = {};
    //Parser.DumpSymbol(L"MiInsertVad", Info);

 

    //// Or try to obtain unexported function offset:
    //Parser.DumpSymbol(L"MiInsertVad", Info);

    //uint64_t start_base = 0x40000000;
    //uint64_t calc_address = start_base + Info.Offset;
    //std::wcout << L"MiInsertVad offset = 0x" << std::hex << Info.Offset << std::endl;
    //std::wcout << L"calc_address 0x" << std::hex << calc_address << std::endl;

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
