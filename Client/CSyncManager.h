#pragma once
#include "CBaseFactory.h"
#include <iostream>

#include "GTA.h"

CFactory(CSyncManager)
{
public:

	CSyncManager() {};


	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
	void OnFrame();
	static CNetGamePlayer* GetLocalPlayer();
	static CNetGamePlayer* GetPlayerByNetId(uint16_t netId);
	static bool EnsurePlayer31();
	inline static netPlayerMgrBase* g_playerMgr;
	inline static netObjectMgr** g_objectMgr;
	inline static CNetGamePlayer* g_player31;
	inline static netInterface_queryFunctions** g_queryFunctions;
	inline static void* netSynctreeFirstArg;
	inline static CNetGamePlayer* (*g_origAllocateNetPlayer)(void*);
	void RunCoopTest();
	static CNetGamePlayer* AllocateNetPlayer(void* mgr);
	static void OnReceiveCloneCreate(datBitBuffer& buffer);
	static void OnReceiveCloneUpdate(datBitBuffer& buffer);
	static netObject* GetNetworkObject(uint16_t id, bool a3);
	static netObject* GetLocalPlayerPedNetObject();

	static void HandleCloneCreate(datBitBuffer& rlBuffer);
	static void* getPlayerPedForNetPlayer(CNetGamePlayer* player);
	static void** ConstructorMutex;
	static netObject* CreateCloneObject(NetObjEntityType type, uint16_t objectId, uint8_t a2, int a3, int a4);
	static void TryAfterJoinPhyisicalhost();
	void EnsureCreateCloneFuncs();
	netObject* CreateCloneToLocalPlayer();
	void RunCloneTest();
	void RunCloneSync(netObject* clone, datBitBuffer& buffer);
};

class CSyncThread : public ScriptThread {


public:
	CSyncThread();

	void			DoRun() override;
};

inline static const uint32_t g_entityNetObjOffset = 208;
extern CSyncManager* g_SyncManager;

