
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <Windows.h>
#include <cstdio>
#include <winternl.h>
#include <tlhelp32.h>
#include "CLauncher.h"
int main()
{
    CLauncher launch;
    launch.StubSteam();
}
