#pragma once
#include <vector>
#include <iostream>
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b



namespace CBaseFactoryUtils {
	extern std::vector<void*> pointers;
	void RunHook();
	void RunUnhook();
	void OnGameHook();
}

class CBaseFactoryHelp {
public:
	CBaseFactoryHelp() {  }
	virtual void Hook() = 0;
	virtual void Unhook() = 0;
	virtual void OnGameHook() = 0;
};
template<typename T>
class CBaseFactory
{
public:

	static inline CBaseFactory<T>* instance;

	CBaseFactory() { instance = this; CBaseFactoryUtils::pointers.push_back(this); }

	virtual void Hook() = 0;
	virtual void Unhook() = 0;
	virtual void OnGameHook() = 0;
public:
	static T& Get() {
		return *(T*)(instance);
	}
};
#define CFactory(T) class T : CBaseFactory<T>
#define CEnd(T) inline const T* CONCAT(factoryInstance, __COUNTER__) = new T();



