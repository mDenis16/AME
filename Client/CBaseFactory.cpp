#include "CBaseFactory.h"
#include <Windows.h>
namespace CBaseFactoryUtils {
	std::vector<void*> pointers;
	void RunHook() {
		MessageBox(0, "Run hook", "as", MB_OK);
		for (auto& instance : pointers)
			reinterpret_cast<CBaseFactoryHelp*>(instance)->Hook();
	}
	void RunUnhook() {
		for (auto instance : pointers)
			reinterpret_cast<CBaseFactoryHelp*>(instance)->Unhook();
	}
	void OnGameHook() {
		for (auto instance : pointers)
			reinterpret_cast<CBaseFactoryHelp*>(instance)->OnGameHook();
	}
}