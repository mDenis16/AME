#pragma once
#include "CBaseFactory.h"
#include <iostream>
#include <map>

CFactory(CPoolManagement)
{
public:
	CPoolManagement() {};
	
	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
	void OnGameFrame();

	static atPoolBase* GetPoolBase(uint32_t hash);
	

	static void* PoolAllocate(atPoolBase* base);
	static void PoolRelease(atPoolBase* pool, void* a1);
	

	inline static void (*o_poolRealease)(atPoolBase* pool, void* a1);
	static void* __fastcall PoolAllocateInternal(atPoolBase* pool);
	inline static void* (*g_origPoolAllocate)(atPoolBase*);

	inline static std::map<uint32_t, atPoolBase*> Pools;

	inline static std::map<atPoolBase*, uint32_t> InversePools;


	static std::uint64_t __fastcall RegisterPoolHook(void* allist, unsigned int hash, int a3);
	inline static atPoolBase*(*g_oRegisterPool)(void* allist, unsigned int hash, int a3);
};
extern CPoolManagement* g_PoolManagement;

