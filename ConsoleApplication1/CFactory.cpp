#include "CFactory.h"

namespace CBaseFactoryUtils {
	std::vector<void*> pointers;
	void RunHook() {
		for (auto instance : pointers)
			reinterpret_cast<CBaseFactory<int>*>(instance)->Hook();
	}
	void RunUnhook() {

	}
}