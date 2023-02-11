#pragma once
#include <vector>
#include <iostream>
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#undef UNIQUE_NAME
#define UNIQUE_NAME(base) CONCAT(base, __COUNTER__)


namespace CBaseFactoryUtils {
	extern std::vector<void*> pointers;
	void RunHook();
	void RunUnhook();
}


template<typename T>
class CBaseFactory 
{
public:

	static inline CBaseFactory<T>* instance;

	CBaseFactory() { instance = this; CBaseFactoryUtils::pointers.push_back(this); }
	
	virtual void Hook() = 0;
	virtual void Unhook() = 0;
public:
	static T& Get() {
		return *(T*)(instance);
	}
};
#define CFactory(T) class T : CBaseFactory<T>
#define CEnd(T) inline static T* UNIQUE_NAME(factoryInstance) = new T();



