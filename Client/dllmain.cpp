#include <Windows.h>
#include <iostream>
#include <thread>
#include "CBootstrap.h"
#include "CVGame.h"



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
   
    CVGame::Bootstrap.Main(hModule, ul_reason_for_call, lpReserved);
    return TRUE;
}

