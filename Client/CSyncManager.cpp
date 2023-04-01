#include "Hooking.h"
#include <xlocbuf>

#include <MinHook.h>
#include "CBootstrap.h"
#include <spdlog/spdlog.h>
#include "atPool.h"
#include <boost/mpl/at.hpp>
#include <boost/function_types/parameter_types.hpp>
#include "CSyncManager.h"
#include <sstream>
#include "CScrEngine.h"

#define ONESYNC_CLONING_NATIVES

#define EASTL_ASSERT_ENABLED 0 
#include <map>
#include <EASTL/bitvector.h>
#include <EASTL/bitset.h>
#include "CPoolManagement.h"
#include "ida.h"
static void (*fwSceneUpdate__AddToSceneUpdate)(void*, uint32_t);
static void (*fwSceneUpdate__RemoveFromSceneUpdate)(void*, uint32_t, bool);
	template<typename T>
	union vector2
	{
		T data[2];
		struct { T x, y; };
	};

	template<typename T>
	union vector3
	{
		T data[3];
		struct { T x, y, z; };
	};

	template<typename T>
	union vector4
	{
		T data[4];
		struct { T x, y, z, w; };
	};

	template<typename T>
	union matrix34
	{
		T data[3][4];
		struct { struct { T x, y, z, w; } rows[3]; };
	};

	template<typename T>
	union matrix44
	{
		T data[4][4];
		struct { struct { T x, y, z, w; } rows[4]; };
	};

	typedef vector2<float> fvector2;
	typedef vector3<float> fvector3;
	typedef vector4<float> fvector4;
	typedef matrix34<float> fmatrix34;
	typedef matrix44<float> fmatrix44;

	//
	struct WriteTreeState
	{
		netObject* object;
		int flags;
		int objFlags;
		datBitBuffer* buffer;
		netLogStub* logger;
		uint32_t time;
		bool wroteAny;
		uint32_t* lastChangeTimePtr;
		int pass = 0;
	};

	struct NetObjectNodeData
	{
		std::tuple<std::array<uint8_t, 1024>, int> lastData;
		std::tuple<std::array<uint8_t, 1024>, int> currentData;
		uint32_t lastChange;
		uint32_t lastAck;
		uint32_t lastResend;
		bool manuallyDirtied = false;

		NetObjectNodeData()
		{
			std::array<uint8_t, 1024> dummyData;
			memset(dummyData.data(), 0, dummyData.size());

			lastData = { dummyData, 0 };
			currentData = { dummyData, 0 };

			lastChange = 0;
			lastAck = 0;
			lastResend = 0;
		}
	};

	struct NetObjectData
	{
		std::array<NetObjectNodeData, 200> nodes;
	};
	int g_physIdx = 42;
	//
	inline size_t GET_NIDX(netSyncTree* tree, void* node)
	{
		return *((uint8_t*)node + 66);
	}
	void* g_tempRemotePlayer;
	//
class CPedFactory
{
public:
	enum class PedCreateFlags
	{
		IS_NETWORKED = (1 << 0),
		IS_PLAYER = (1 << 1)
	};

	virtual ~CPedFactory() = default;
	virtual void* CreatePed(std::uint8_t flags, std::uint16_t model_index, fmatrix44* matrix, bool default_component_variation, bool register_network_object, bool give_default_loadout, bool, bool) = 0; // 0x08
	virtual void* CreateClone(std::uint8_t* flags, std::uint16_t* model_index, fmatrix44* matrix, bool default_component_variation, bool, bool register_network_object, bool) = 0;                          // 0x10
	virtual void* ClonePed(void* ped, bool register_network_object, bool link_blends, bool clone_compressed_damage) = 0;                                                                                          // 0x18
	virtual void* ClonePedToTarget(void* source, void* target, bool clone_compressed_damage) = 0;                                                                                                                 // 0x20
	virtual void* CreatePlayer(std::uint8_t flags, std::uint16_t model_index, fmatrix44* matrix, void* player_info) = 0;                                                                            // 0x28
	virtual void DestroyPed(void* ped) = 0;                                                                                                                                                                       // 0x30

	void* m_local_ped; //0x0008
}; //Size: 0x0010s
static CPedFactory** g_pedFactory;
using TCreateCloneObjFn = netObject* (*)(uint16_t objectId, uint8_t, int, int);
using TPoolPtr = atPoolBase**;

static void netObjectMgrBase__RegisterNetworkObject(netObjectMgr* manager, netObject* object);
static void(*g_orig_netObjectMgrBase__RegisterNetworkObject)(netObjectMgr*, netObject*);
static TCreateCloneObjFn createCloneFuncs[(int)NetObjEntityType::Max];
static TPoolPtr validatePools[(int)NetObjEntityType::Max];
static CNetGamePlayer* (*g_origGetOwnerNetPlayer)(netObject*);
CNetGamePlayer* netObject__GetPlayerOwner(netObject* object);
static bool mD0Stub(netSyncTree* tree, int a2);
std::map<int, std::map<void*, std::tuple<int, uint32_t>>> g_netObjectNodeMapping;
std::array<std::unique_ptr<NetObjectData>, 65536> g_syncData;
static netObject* g_curNetObject;
CNetGamePlayer* g_players[256];
static void* (*g_AllocateNetBlender)(void* netObject, void* a2);
static uint32_t* rage__s_NetworkTimeLastFrameStart;
static uint32_t* rage__s_NetworkTimeThisFrameStart;
static char MATA_BUFFER[2000];
static size_t MATA_BUFFER_SIZE = 0;

//

static CNetGamePlayer* g_playerListRemote[256];
static int g_playerListCountRemote;
std::unordered_map<uint16_t, CNetGamePlayer*> g_playersByNetId;
std::unordered_map<CNetGamePlayer*, uint16_t> g_netIdsByPlayer;
static int g_playerListCount;

//
static CNetGamePlayer* g_playerList[256];
//
void CalledAfterInitSession();
template<typename T>
static bool TraverseTreeInternal(netSyncNodeBase* node, T& state, const std::function<bool(T&, netSyncNodeBase*, const std::function<bool()>&)>& cb)
{
	return cb(state, node, [&node, &state, &cb]()
		{
			bool val = false;

	if (node->IsParentNode())
	{
		for (auto child = node->firstChild; child; child = child->nextSibling)
		{
			if (TraverseTreeInternal(child, state, cb))
			{
				val = true;
			}
		}
	}

	return val;
		});
}

template<typename T>
static void TraverseTree(netSyncTree* tree, T& state, const std::function<bool(T&, netSyncNodeBase*, const std::function<bool()>&)>& cb)
{
	TraverseTreeInternal(tree->syncNode, state, cb);
}

static void InitTree(netSyncTree* tree)
{
	// unused padding in GTA5/RDR3
#ifdef GTA_FIVE
	auto didStuff = (uint16_t*)((char*)tree + 1222);
#elif IS_RDR3
	auto didStuff = (uint16_t*)((char*)tree + 1990);
#endif

	if (*didStuff != 0xCFCF)
	{
		size_t idx = 1;
		TraverseTree<size_t>(tree, idx, [](size_t& idx, netSyncNodeBase* node, const std::function<bool()>& cb) -> bool
			{
				if (node->IsParentNode())
				{
					cb();
				}
				else if (node->IsDataNode())
				{
#ifdef GTA_FIVE
					* ((uint8_t*)node + 66) = idx++;
#elif IS_RDR3
					* ((uint8_t*)node + 68) = idx++;
#endif
				}

		return true;
			});


		*didStuff = 0xCFCF;
	}
}
void* __fastcall AllocateNetBlender(void* netObject, void* a2) {
	spdlog::info("AllocateNetBlender {} {}", netObject, a2);

	return g_AllocateNetBlender(netObject, a2);
}
static bool(*g_orig_netBuffer_WriteUnsigned)(void* a1, uint32_t a2, int length, int a4);
static bool _netBuffer_WriteUnsigned(void* a1, uint32_t a2, int length, int a4)
{
	length = 16;
	return g_orig_netBuffer_WriteUnsigned(a1, a2, length, a4);
}
static void(*g_orig_netBuffer_ReadUnsigned)(void* a1, uint32_t* a2, int length, int a4);
static void _netBuffer_ReadUnsigned(void* a1, uint32_t* a2, int length, int a4)
{
	length = 16;

	return g_orig_netBuffer_ReadUnsigned(a1, a2, length, a4);
}
static void(*g_orig_netBuffer_BumpWriteCursor)(datBitBuffer* a1, int length);
static void _netBuffer_BumpWriteCursor(datBitBuffer* a1, int length)
{
	length = 16;
	return g_orig_netBuffer_BumpWriteCursor(a1, length);
}
static void(*g_orig_netBuffer_BumpReadWriteCursor)(datBitBuffer* a1, int length);
static void _netBuffer_BumpReadWriteCursor(datBitBuffer* a1, int length)
{
	length = 16;
	return g_orig_netBuffer_BumpReadWriteCursor(a1, length);
}
static char(*g_origReadDataNode)(void* node, uint32_t flags, void* mA0, datBitBuffer* buffer, netObject* object);
#pragma pack(push, 8)
class netSyncDataNode : public netSyncNodeBase
{
public:
	uint32_t flags; //0x40
	uint32_t pad3; //0x44
	uint64_t pad4; //0x48

	netSyncDataNode* parentData; //0x50
	uint32_t childCount; //0x58
	netSyncDataNode* children[8]; //0x5C
	uint8_t syncFrequencies[8]; //0x9C
	void* commonDataOpsVFT; //0xA8 wtf
};
static_assert(sizeof(netSyncDataNode) == 0xB0);
#pragma pack(pop)
class NodeCommonDataOperations
{
public:
	virtual ~NodeCommonDataOperations() = default;
	virtual void ReadFromBuffer(netSyncDataNode* node) {};            // 0x08
	virtual void WriteToBuffer(netSyncDataNode* node) {};             // 0x10
	virtual void Unk() {};                                                  // 0x18
	virtual int CalculateSize(netSyncDataNode* node) { return 0; };   // 0x20
	virtual int CalculateSize2(netSyncDataNode* node) { return 0; };  // 0x28
	virtual void LogSyncData(netSyncDataNode* node) {};               // 0x30
	virtual void LogSyncData2(netSyncDataNode* node) {};              // 0x38

	datBitBuffer* m_buffer; // 0x8
};
class CProjectBaseSyncDataNode : public netSyncDataNode
{
public:
	virtual bool IsSyncNode() { return false; }                                 // 0x50
	virtual bool _0x58() { return false; }                                      // 0x58
	virtual bool IsGlobalFlags() { return false; }                              // 0x60
	virtual void DoPreCache(void* data) {}                         // 0x68
	virtual std::uint8_t GetSyncFrequency(int index) { return 0; }              // 0x70
	virtual int GetSyncInterval(int index) { return 0; }                        // 0x78
	virtual int GetBandwidthForPlayer(int player) { return 200; }               // 0x80 (should always return 200)
	virtual void _0x88(void*) {}                                                // 0x88
	virtual bool _0x90(void*, void*, int, int, int) { return false; }           // 0x90
	virtual int CalculateSize() { return 0; }                                   // 0x98 need to verify later
	virtual bool IsPreCacheDisabled() { return false; }                         // 0xA0
	virtual bool CanApply(netObject* object) { return false; }            // 0xA8
	virtual int GetPlayersInScope() { return -1; }                              // 0xB0
	virtual void DeserializeImpl() {}                                           // 0xB8 need to verify later
	virtual void SerializeImpl() {}                                             // 0xC0 need to verify later
	virtual int CalculateSize2() { return 0; }                                  // 0xC8
	virtual int _0xD0() { return 0; }                                           // 0xD0 calls NodeCommonDataOperations::Unk()
	virtual void Log() {}                                                       // 0xD8
	virtual bool CanPreCache(int) { return false; }                             // 0xE0 arg is always zero afaik
	virtual bool CanBeEmpty() { return false; }                                 // 0xE8
	virtual bool IsEmpty() { return false; }                                    // 0xF0 returns true if all data is default
	virtual void SetEmpty() {}                                                  // 0xF8 sets all data to their default values
	virtual void Log2() {}                                                      // 0x100
	virtual void ResetScriptData() {}                                           // 0x108
	virtual bool _0x110() { return false; }                                     // 0x110

private:
	NodeCommonDataOperations m_common_data_operations; // 0xB0 this is generally invalidated by MoveCommonDataOpsVFT()
};
#pragma pack(push,4)
class CPlayerCreationDataNode : CProjectBaseSyncDataNode
{
public:
	uint32_t m_model; //0x00C0
	uint32_t m_num_scars; //0x00C4
	char unk_struct_0xC8[192]; //0x00C8
	uint32_t unk_0188; //0x0188
	char pad_018C[4]; //0x018C
	char m_scar_struct[176]; //0x0190
	bool unk_0240; //0x0240
	char pad_0241[19]; //0x0241
}; //Size: 0x0254
static_assert(sizeof(CPlayerCreationDataNode) == 0x254);
#pragma pack(pop)
#pragma pack(push,4)
class CPedCreationDataNode : CProjectBaseSyncDataNode
{
public:
	uint32_t m_pop_type;  //0x00C0
	uint32_t m_model; //0x00C4
	uint32_t m_random_seed; //0x00C8
	uint32_t m_max_health; //0x00CC
	bool m_in_vehicle; //0x00D0
	char pad_0xD1[1]; //0x00D1
	uint16_t m_vehicle_id; //0x00D2
	uint32_t m_vehicle_seat; //0x00D4
	bool m_has_prop; //0x00D8
	char pad_0xD9[3];  //0x00D9
	uint32_t m_prop_model; //0x00DC
	bool m_is_standing; //0x00E0
	bool m_is_respawn_object_id; //0x00E1
	bool m_is_respawn_flagged_for_removal; //0x00E2
	bool m_has_attr_damage_to_player; //0x00E3
	uint8_t m_attribute_damage_to_player; //0x00E4
	uint32_t m_voice_hash; //0x00E8
}; //Size: 0x00EC
static_assert(sizeof(CPedCreationDataNode) == 0xEC);
#pragma pack(pop)
static bool ReadDataNodeStub(void* node, uint32_t flags, void* mA0, datBitBuffer* buffer, netObject* object)
{
	
	
	// enable this for boundary checks
	/*if (flags == 1 || flags == 2)
	{
		uint32_t in = 0;
		buffer->ReadInteger(&in, 8);
		assert(in == 0x5A);
	}*/

	bool didRead = g_origReadDataNode(node, flags, mA0, buffer, g_curNetObject);

	if (didRead && g_curNetObject)
	{
		g_netObjectNodeMapping[g_curNetObject->GetObjectId()][node] = { 0, (*CSyncManager::g_queryFunctions)->GetTimestamp()};
		CPlayerCreationDataNode* nd = (CPlayerCreationDataNode*)(node);
		spdlog::info("read");
	}
	spdlog::info("ReadDataNodeStub {} {} {} didRead {}", node, flags, buffer->m_curBit, didRead);
	return didRead;
}
static char(*g_origWriteDataNode)(void* node, uint32_t flags, void* mA0, netObject* object, datBitBuffer* buffer, int time, void* playerObj, char playerId, void* unk, void* unk2545);

static bool WriteDataNodeStub(void* node, uint32_t flags, void* mA0, netObject* object, datBitBuffer* buffer, int time, void* playerObj, char playerId, void* unk, void* unk2545)
{
	spdlog::info("WriteDataNodeStub node:{} flags:{} playerId:{} time:{}", node, flags, (int)playerId, time);

	//if (playerId != 31 || flags == 4)
	{
		return g_origWriteDataNode(node, flags, mA0, object, buffer, time, playerObj, playerId, unk, unk2545);
	}
	//else
	{
		// save position and write a placeholder length frame
		uint32_t position = buffer->GetPosition();
		buffer->WriteBit(false);
		buffer->WriteUns(0, 11);

		bool rv = g_origWriteDataNode(node, flags, mA0, object, buffer, time, playerObj, playerId, unk, unk2545);

		// write the actual length on top of the position
		uint32_t endPosition = buffer->GetPosition();
		auto length = endPosition - position - 11 - 1;

		if (length > 1 || flags == 1)
		{
			buffer->Seek(position);

			buffer->WriteBit(true);
			buffer->WriteUns(length, 11);
			buffer->Seek(endPosition);

			if (g_curNetObject)
			{
				g_netObjectNodeMapping[g_curNetObject->GetObjectId()][node] = { 1, (*CSyncManager::g_queryFunctions)->GetTimestamp()};
			}

			spdlog::info("actually wrote {}", node);
		}
		else
		{
			buffer->Seek(position + 1);
		}

		return rv;
	}
}
static void(*g_origCallSkip)(void* a1, void* a2, void* a3, void* a4, void* a5);

static void SkipCopyIf1s(void* a1, void* a2, void* a3, void* a4, void* a5)
{
	//if (!icgi->OneSyncEnabled)
	{
	//	g_origCallSkip(a1, a2, a3, a4, a5);
	}
}
static void SetupLocalPlayer(CNetGamePlayer* player)
{
	if (player != CSyncManager::g_playerMgr->localPlayer)
	{
		return;
	}

	spdlog::info("onesync: Assigning physical player index for the local player.");

	auto clientId = 15;
	auto idx = 13;


	player->physicalPlayerIndex() = idx;

	g_players[idx] = player;

	g_playersByNetId[clientId] = player;
	g_netIdsByPlayer[player] = clientId;

	// add to sequential list
	g_playerList[g_playerListCount] = player;
	g_playerListCount++;


	// don't add to g_playerListRemote(!)
}

static void(*g_origJoinBubble)(void* bubbleMgr, CNetGamePlayer* player);

static void JoinPhysicalPlayerOnHost(void* bubbleMgr, CNetGamePlayer* player)
{
	g_origJoinBubble(bubbleMgr, player);

	
	//SetupLocalPlayer(player);
	spdlog::info("JoinPhysicalPlayerOnHost ");
	
}
void CSyncManager::TryAfterJoinPhyisicalhost() {
	
}
static CNetGamePlayer* (*g_origGetPlayerByIndex)(uint8_t);

static CNetGamePlayer* __fastcall GetPlayerByIndex(uint8_t index)
{


	if (index < 0 || index >= 256)
	{
		return nullptr;
	}

	return g_players[index];
}
static CNetGamePlayer* (*g_origGetPlayerByIndexNet)(int);
void DirtyNode(netObject* object, netSyncDataNodeBase* node)
{
	auto tree = object->GetSyncTree();
	InitTree(tree);

	size_t nodeIdx = GET_NIDX(tree, node);
	const auto& sd = g_syncData[((netObject*)object)->GetObjectId()];

	if (!sd)
	{
		return;
	}

	auto& nodeData = sd->nodes[nodeIdx];
	nodeData.lastChange = 0;
	nodeData.lastAck = 0;
	nodeData.lastResend = 0;
	nodeData.manuallyDirtied = true;
}
static CNetGamePlayer* GetPlayerByIndexNet(int index)
{
	

	// todo: check network game flag
	return GetPlayerByIndex(index);
}

static bool(*g_origIsNetworkPlayerActive)(int);

static bool IsNetworkPlayerActive(int index)
{
	

	return (GetPlayerByIndex(index) != nullptr);
}

static bool(*g_origIsNetworkPlayerConnected)(int);

static bool IsNetworkPlayerConnected(int index)
{
	

	return (GetPlayerByIndex(index) != nullptr);
}
static void(*g_origManuallyDirtyNode)(void* node, void* object);

static void ManuallyDirtyNodeStub(netSyncDataNodeBase* node, netObject* object)
{
	spdlog::info("ManuallyDirtyNodeStub called");
	DirtyNode(object, node);
}
static void(*g_orig_netSyncDataNode_ForceSend)(void* node, int actFlag1, int actFlag2, netObject* object);

static void netSyncDataNode_ForceSendStub(netSyncDataNodeBase* node, int actFlag1, int actFlag2, netObject* object)
{
	spdlog::info("netSyncDataNode_ForceSendStub called");
	// maybe needs to read act flags?
	DirtyNode(object, node);
}
static void(*g_orig_netSyncDataNode_ForceSendToPlayer)(void* node, int player, int actFlag1, int actFlag2, netObject* object);

static void netSyncDataNode_ForceSendToPlayerStub(netSyncDataNodeBase* node, int player, int actFlag1, int actFlag2, netObject* object)
{
	spdlog::info("netSyncDataNode_ForceSendToPlayerStub called");
	// maybe needs to read act flags?
	DirtyNode(object, node);
}
void CalledAfterInitSession() {
	
	
}
static void fwSceneUpdate__AddToSceneUpdate_Track(void* entity, uint32_t what);
static void fwSceneUpdate__RemoveFromSceneUpdate_Track(void* entity, uint32_t what, bool ok);
static bool (*g_origNetGamePlayerIsVisibleToPlayer)(CNetGamePlayer*, CNetGamePlayer*, char);

static bool NetGamePlayerIsVisibleToPlayer(CNetGamePlayer* player, CNetGamePlayer* target, char flags)
{
	
	// So we make sure that target player is still have physical entity, before returning true.

	bool result = g_origNetGamePlayerIsVisibleToPlayer(player, target, flags);

	// don't continue when original function returned false or target is player 31
	if (target->physicalPlayerIndex() == 31)
	{
		return false;
	}
	return true;
}
void AddLog(const char* format, ...)
{
	char dest[1024 * 16];
	va_list argptr;
	va_start(argptr, format);
	vsprintf(dest, format, argptr);
	va_end(argptr);
	spdlog::debug("GTA_DEBUG {}", dest);
	//spdlog::info("GTA_LOG {}", a1, ...);
}
static void (*g_origSendCloneSync)(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6);

static void SendCloneSync(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6)
{
	auto t = *rage__s_NetworkTimeThisFrameStart;
	*rage__s_NetworkTimeThisFrameStart = *rage__s_NetworkTimeLastFrameStart;

	g_origSendCloneSync(a1, a2, a3, a4, a5, a6);

	*rage__s_NetworkTimeThisFrameStart = t;
}

static int(*g_origGetNetworkPlayerListCount)();
static CNetGamePlayer** (*g_origGetNetworkPlayerList)();



static int netInterface_GetNumRemotePhysicalPlayers()
{
	

	return g_playerListCountRemote;
}

static CNetGamePlayer** netInterface_GetRemotePhysicalPlayers()
{
	

	return g_playerListRemote;
}

static int(*g_origGetNetworkPlayerListCount2)();
static CNetGamePlayer** (*g_origGetNetworkPlayerList2)();

static int netInterface_GetNumPhysicalPlayers()
{
	

	return g_playerListCount;
}

static CNetGamePlayer** netInterface_GetAllPhysicalPlayers()
{
	
	return g_playerList;
}
static uint8_t(*g_origGetOwnerPlayerId)(netObject*);

static uint8_t netObject__GetPlayerOwnerId(netObject* object)
{
	

	auto owner = netObject__GetPlayerOwner(object);

	return owner ? owner->physicalPlayerIndex() : 0xFF;
}

void CSyncManager::Hook()
{
	g_pedFactory = hook::get_address<decltype(g_pedFactory)>(hook::get_pattern("E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 58 08 48 8B CB E8", 8));
	rage__s_NetworkTimeThisFrameStart = hook::get_address<uint32_t*>(hook::get_pattern("49 8B 0F 40 8A D6 41 2B C4 44 3B 25", 12));
	rage__s_NetworkTimeLastFrameStart = hook::get_address<uint32_t*>(hook::get_pattern("89 05 ? ? ? ? 48 8B 01 FF 50 10 80 3D", 2));


	// replace joining local net player to bubble
	{
		auto location = hook::get_pattern("48 8B D0 E8 ? ? ? ? E8 ? ? ? ? 83 BB ? ? ? ? 04", 3);

		hook::set_call(&g_origJoinBubble, location);
		hook::call(location, JoinPhysicalPlayerOnHost);
	}

	{
		MH_CreateHook(hook::get_pattern("48 83 79 48 00 48 8B FA 48 8B D9 74 40", -10), ManuallyDirtyNodeStub, (void**)&g_origManuallyDirtyNode);
		MH_CreateHook(hook::get_pattern("85 51 28 0F 84 E4 00 00 00 33 DB", -0x24), netSyncDataNode_ForceSendStub, (void**)&g_orig_netSyncDataNode_ForceSend);
		MH_CreateHook(hook::get_pattern("44 85 41 28 74 73 83 79 30 00", -0x1F), netSyncDataNode_ForceSendToPlayerStub, (void**)&g_orig_netSyncDataNode_ForceSendToPlayer);
	}

	g_playerMgr = *hook::get_address<netPlayerMgrBase**>(hook::get_pattern("40 80 FF 20 72 B3 48 8B 0D", 9));
	g_objectMgr = hook::get_address<netObjectMgr**>(hook::get_pattern("2B C3 3D 88 13 00 00 0F 82 ? ? ? ? 48 8B 05", 16));
	g_queryFunctions = hook::get_address<netInterface_queryFunctions**>(hook::get_pattern("72 23 48 8B 0D ? ? ? ? 48 85 C9 74", 5));


	MH_CreateHook(hook::get_pattern("48 8B F2 0F B7 52 0A 41 B0 01", -0x19), netObjectMgrBase__RegisterNetworkObject, (void**)&g_orig_netObjectMgrBase__RegisterNetworkObject); //

	MH_CreateHook(hook::get_pattern("33 DB 48 8B F9 48 39 99 ? ? 00 00 74 ? 48 81 C1 E0", -10), AllocateNetPlayer, (void**)&g_origAllocateNetPlayer);


	EnsureCreateCloneFuncs();
}

void CSyncManager::Unhook()
{

}

void netBlender::SetTimestamp(uint32_t timestamp)
{
	static auto fn = hook::get_pattern<void(netBlender*, uint32_t timeStamp)>("48 8B D9 39 79 18 74 76 48", -0x13);

	return fn(this, timestamp);
}
CNetGamePlayer* TempHackMakePhysicalPlayerImpl(uint16_t clientId, int idx = -1)
{
	auto npPool = CPoolManagement::GetPoolBase(hashGet("CNonPhysicalPlayerData"));

	// probably shutting down the network subsystem
	if (!npPool)
	{
		return nullptr;
	}

	void* fakeInAddr = calloc(256, 1);
	void* fakeFakeData = calloc(256, 1);

	rlGamerInfo* inAddr = (rlGamerInfo*)fakeInAddr;
	inAddr->peerAddress.localAddr().ip.addr = clientId ^ 0xFEED;
	inAddr->peerAddress.relayAddr().ip.addr = clientId ^ 0xFEED;
	inAddr->peerAddress.publicAddr().ip.addr = clientId ^ 0xFEED;
	inAddr->peerAddress.rockstarAccountId() = clientId;
	inAddr->gamerId = clientId;

	// this has to come from the pool directly as the game will expect to free it
	void* nonPhys = CPoolManagement::PoolAllocate(npPool);
	static auto _npCtor = hook::get_pattern<void(void*)>("C7 41 08 0A 00 00 00 C7 41 0C 20 00 00 00", -0xA);
	_npCtor(nonPhys); // ctor

	void* phys = calloc(1024, 1);

	static auto _pCtor = hook::get_address<void*>(hook::get_pattern<void*>("41 8B E9 4D 8B F0 48 8B DA E8", 9), 1, 5);
	reinterpret_cast<void(__cdecl*)(void*)>(_pCtor)(phys);
	

	auto player = CSyncManager::g_playerMgr->AddPlayer(fakeInAddr, fakeFakeData, nullptr, phys, nonPhys);
	g_tempRemotePlayer = player;
	

	// so we can safely remove (do this *before* assigning physical player index, or the game will add
	// to a lot of lists which aren't >32-safe)
	//g_playerMgr->UpdatePlayerListsForPlayer(player);
	// NOTE: THIS IS NOT SAFE unless array manager/etc. are patched

	if (idx == -1)
	{
		idx = g_physIdx;
		g_physIdx++;
	}

	player->physicalPlayerIndex() = idx;

	g_players[idx] = player;
	
	return player;
}
__int64  FindTaskPrimaryByTypeRE(__int64 a1, int a2, int a3)
{
	__int64 i; // r8

	spdlog::info("from: {}", *(_QWORD*)(a1 + 8i64 * a3 + 32));

	for (i = *(_QWORD*)(a1 + 8i64 * a3 + 32); ; i = *(_QWORD*)(i + 32))
	{
		if (!i)
			return 0i64;
		spdlog::info("active task type {}", (*(__int16*)(i + 52)));
		if (*(__int16*)(i + 52) == a2)
			break;
	}
	return i;
}
__int64 FindTaskActiveByTypeRE(__int64 a1, int a2)
{
	__int64 i; // rcx

	if (*(_DWORD*)(a1 + 16) == -1)
		return 0i64;
	for (i = *(_QWORD*)(a1 + 8i64 * *(int*)(a1 + 16) + 32); i && *(__int16*)(i + 52) != a2; i = *(_QWORD*)(i + 32))
		;
	return i;
}
#include "nativeList.h"
#include "Script.h"
__int64 GetRunningMovementTaskRE(__int64 a1)
{
	__int64 v1; // rdx
	__int64 result; // rax

	if ((*(_BYTE*)(a1 + 48) & 1) == 0)
		return 0i64;
	if ((*(_BYTE*)(a1 + 48) & 4) != 0)
		return 0i64;
	v1 = *(_QWORD*)(a1 + 176);
	if (!v1)
		return 0i64;
	result = *(_QWORD*)(*(_QWORD*)(*(_QWORD*)(*(_QWORD*)(a1 + 16) + 4256i64) + 896i64) + 40i64);
	if (!result || *(_WORD*)(result + 52) != *(_WORD*)(v1 + 52))
		return 0i64;
	return result;
}
float RandomFloat(float min, float max)
{
	// this  function assumes max > min, you may want 
	// more robust error checking for a non-debug build
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;

	// generate (in your case) a float between 0 and (4.5-.78)
	// then add .78, giving you a float between .78 and 4.5
	float range = max - min;
	return (random * range) + min;
}
char __fastcall Is_PedRunning_Internal(__int64 pedBlender)
{
	float movmentSpeed; // xmm0_4
	char v2; // dl
	__m128 v3; // xmm1
	float v4; // xmm2_4

	movmentSpeed = *(float*)(pedBlender + 0x58);
	v2 = 1;
	if ((*(_DWORD*)(pedBlender + 0xB0) & 0x40) != 0)
	{
		v3 =  *(__m128*)(pedBlender + 0x54);
		v3.m128_f32[0] = (float)(v3.m128_f32[0] * v3.m128_f32[0]) + (float)(movmentSpeed * movmentSpeed);
		v4 = _mm_sqrt_ps(v3).m128_f32[0];
		if (v4 < 1.5 || v4 >= 2.5)
			return 0;
	}
	else
	{
		if (movmentSpeed < 0.0)
			movmentSpeed = -movmentSpeed;
		if (movmentSpeed < 1.5 || movmentSpeed >= 2.5)
			return 0;
	}
	return v2;
}
// Created with ReClass.NET 1.2 by KN4CK3R

class CBlender
{
public:
	char pad_0000[48]; //0x0000
	float heading; //0x0030
	float heading2; //0x0034
	char pad_0038[32]; //0x0038
	float movement_speed; //0x0058
	char pad_005C[452]; //0x005C
	int8_t state; //0x0220
	char pad_0221[223]; //0x0221
	float time_since_startedmoving; //0x0300
	char pad_0304[932]; //0x0304
	float N00000148; //0x06A8
	char pad_06AC[4]; //0x06AC
	float N00000149; //0x06B0
	char pad_06B4[92]; //0x06B4
	glm::vec3 N00000155; //0x0710
	char pad_071C[3028]; //0x071C
	glm::vec3 N000002D0; //0x12F0
	char pad_12FC[400]; //0x12FC
}; //Size: 0x148C
static_assert(sizeof(CBlender) == 0x148C);
void RunTest() {
	if (NETWORK::NETWORK_IS_IN_SESSION()) {
		if (CSyncManager::EnsurePlayer31()) {
			CalledAfterInitSession();

			auto local = CSyncManager::GetLocalPlayer();
		
			auto local_net = CSyncManager::GetLocalPlayerPedNetObject();
			if (local_net) {
				auto st = local_net->GetSyncTree();
				static char bluh[1000];
				memset(bluh, 0, sizeof(bluh));
	
				datBitBuffer buffer(bluh, sizeof(bluh));

				uint32_t time;
				st->WriteTreeCfx(2, 0, local_net, &buffer, (*CSyncManager::g_queryFunctions)->GetTimestamp(), nullptr, 31, nullptr, &time);

				spdlog::info("Write tree data length {} ", buffer.GetDataLength());
			}
		}
	}
}

static void fwSceneUpdate__AddToSceneUpdate_Track(void* entity, uint32_t what)
{
	//spdlog::info("fwSceneUpdate__AddToSceneUpdate_Track entity-> {}", entity);

	fwSceneUpdate__AddToSceneUpdate(entity, what);
}

static void fwSceneUpdate__RemoveFromSceneUpdate_Track(void* entity, uint32_t what, bool ok)
{
	fwSceneUpdate__RemoveFromSceneUpdate(entity, what, ok);
	///spdlog::info("fwSceneUpdate__RemoveFromSceneUpdate entity-> {}", entity);
	
}

void CSyncManager::RunCloneSync(netObject* clone, datBitBuffer& buffer) {
	
	auto remote_st = netSyncTree::GetForType(NetObjEntityType::Player);
	remote_st->ReadFromBuffer(2, 0, &buffer, nullptr);
	

	if (!remote_st->CanApplyToObject(clone))
	{
		spdlog::info("Couldn't apply object.\n");
		return;
	}

	if (clone->GetBlender())
	{
		clone->GetBlender()->SetTimestamp((*g_queryFunctions)->GetTimestamp());

		if (!clone->syncData.isRemote)
		{
			clone->GetBlender()->m_28();
		}
	}
	remote_st->ApplyToObject(clone, nullptr);

	// call post-apply
	clone->PostSync();
	spdlog::info("post sync for clone {}", (void*)clone);
}




void dmp_sync_tree_ida(__int64 synctree) {
	if (synctree)
	{
		auto v111 = synctree + 0x30;
		unsigned int v112 = 0;   _DWORD* j;
		for (j = (_DWORD*)(synctree + 0x180); (signed int)v112 < *j; v111 += 8i64)
		{
			if (*(_QWORD*)v111)
			{
				bool updated = false;
				if (*(_BYTE*)(*(_QWORD*)v111 + 64i64))
					updated = true;
				spdlog::info("Node {} updated {}", v112, updated ? "true" : "false");
			}
			else
			{
				spdlog::info("Node {} is null.", v112);
			}
			++v112;
		}
	}
	else
	{
		spdlog::info("Tree is null.");
	}
}
__int64 __fastcall sub_7FF7BC9ED0EC(__int64 a1)
{
	unsigned int v1; // r8d
	__int64* v2; // rdx
	__int64 result; // rax

	v1 = 0;
	if (*(_DWORD*)(a1 + 32))
	{
		v2 = (__int64*)(a1 + 48);
		do
		{
			result = *v2;
			++v1;
			++v2;
			*(_WORD*)(result + 64) = 0;
		} while (v1 < *(_DWORD*)(a1 + 32));
	}
	return result;
}
__int64 __fastcall ReadFromBufferRE(__int64 treeCfx, unsigned int a2, unsigned int a3, __int64 a4, __int64 a5)
{
	__int64 result = 0; // rax
	__int64 v9; // rbx
	unsigned int v10; // [rsp+50h] [rbp+18h] BYREF

	v10 = a3;
	
	if (*(_QWORD*)(treeCfx + 16))
	{
		v9 = a5;
		(*(void(__fastcall**)(__int64, _QWORD, unsigned int*, __int64, __int64))(*(_QWORD*)treeCfx + 40i64))(
			treeCfx,
			a2,
			&v10,
			a4,
			a5);
		sub_7FF7BC9ED0EC(treeCfx);
		return (*(__int64(__fastcall**)(_QWORD, _QWORD, _QWORD, __int64, __int64))(**(_QWORD**)(treeCfx + 16) + 56i64))(
			*(_QWORD*)(treeCfx + 16),
			a2,
			v10,
			a4,
			v9);
	}
	return result;
}
class netSyncTreeRE
{
public:
	virtual ~netSyncTreeRE() = default;

	char pad_0008[8]; //0x0008
	netSyncNodeBase* m_next_sync_node; //0x0010
	netSyncNodeBase* m_last_sync_node; //0x0018
	uint32_t m_child_node_count; //0x0020
	uint32_t m_unk_array_count; //0x0024
	char pad_0028[8]; //0x0028
	netSyncNodeBase* m_child_nodes[42]; //0x0030
	uint32_t m_child_node_max_count; //0x0180
	netSyncNodeBase* m_unk_array[32]; //0x0188
	uint32_t m_unk_array_max_count; //0x0288
	char pad_0290[560]; //0x0290
}; //Size: 0x0030

char __fastcall DirtNodeChildren(__int64 syncTree, __int64 object)
{
	auto stTree = (netSyncTreeRE*)(syncTree);
	char v4; // bl
	int v5; // edi
	_QWORD* i; // rsi
	spdlog::info("syncTree m_child_node_count {}", *(_DWORD*)(syncTree + 0x20));
	v4 = 1;
	//((void (*)(void))sub_7FF7BCA045DC)();
	if (!(*(unsigned __int8(__fastcall**)(__int64))(*(_QWORD*)object + 160i64))(object)) {
		spdlog::info("returned by unknownfuncitons ");
		return 0;
	}
	v5 = 0;




	if (stTree->m_child_node_count)
	{ //loops through child nodes



		for (size_t i = 0; i < stTree->m_child_node_count; i++)
		{

			auto child_node = stTree->m_child_nodes[i];
			netSyncDataNode_ForceSendStub((netSyncDataNodeBase*)child_node, 1, 0, (netObject*)object);
			netSyncDataNode_ForceSendToPlayerStub((netSyncDataNodeBase*)child_node, 31, 1, 0, (netObject*)object);
			ManuallyDirtyNodeStub((netSyncDataNodeBase*)child_node, (netObject*)object);

			if (i == 18) {
				auto creation_node = (CPlayerCreationDataNode*)(child_node);
				spdlog::info("creation_node");
			}
			bool result = (*(unsigned __int8(__fastcall**)(_QWORD, __int64))(*(_QWORD*)child_node + 168i64))((_QWORD)child_node, object);
			spdlog::info("i = {} child node {} result is {}", i, (void*)child_node, result);

		}
		/*	for (i = (_QWORD*)(syncTree + 0x30);
				!*(_BYTE*)(*i + 64i64);
				++i)
			{
				bool result = (*(unsigned __int8(__fastcall**)(_QWORD, __int64))(*(_QWORD*)*i + 168i64))(*i, object);

				spdlog::info("current_child_addr = {} apply status {} " , (uint64)i , result);
				if ((unsigned int)++v5 >= *(_DWORD*)(syncTree + 0x20))
					return v4;
			}
			spdlog::info("returned 0  from CanApplyToObjectRE current v5 {}", v5);*/
		return 0;
	}
	return 0;
}
char __fastcall CanApplyToObjectRE(__int64 syncTree, __int64 object)
{
	auto stTree = (netSyncTreeRE*)(syncTree);
	char v4; // bl
	int v5; // edi
	_QWORD* i; // rsi
	spdlog::info("syncTree m_child_node_count {}", *(_DWORD*)(syncTree + 0x20));
	v4 = 1;
	//((void (*)(void))sub_7FF7BCA045DC)();
	if (!(*(unsigned __int8(__fastcall**)(__int64))(*(_QWORD*)object + 160i64))(object)) {
		spdlog::info("returned by unknownfuncitons ");
		return 0;
	}
	v5 = 0;



	
	if (stTree->m_child_node_count)
	{ //loops through child nodes
		

		
		for (size_t i = 0; i < stTree->m_child_node_count; i++)
		{
			
			auto child_node = stTree->m_child_nodes[i];
			//netSyncDataNode_ForceSendStub((netSyncDataNodeBase*)child_node, 1, 0, (netObject*)object);
			//netSyncDataNode_ForceSendToPlayerStub((netSyncDataNodeBase*)child_node, 31, 1, 0, (netObject*)object);

			if (i == 0) {
				auto creation_node = (CPlayerCreationDataNode*)(child_node);
			}
			bool result = (*(unsigned __int8(__fastcall**)(_QWORD, __int64))(*(_QWORD*)child_node + 168i64))((_QWORD)child_node, object);
			spdlog::info("i = {} child node {} result is {}", i, (void*)child_node, result);

		}
	
		return 0;
	}
	return 0;
}
void CSyncManager::HandleCloneCreate(datBitBuffer& receivedBuff) {
	


	//auto syncTree = netSyncTree::GetForType(NetObjEntityType::Player);
	
	datBitBuffer buf(receivedBuff.m_data, receivedBuff.GetDataLength());
	buf.m_f1C = 1;


	// owner ID (forced to be remote so we can call ChangeOwner later)
	auto isRemote = true;
	auto owner = 31;

	// create the object
	auto obj = CreateCloneObject(NetObjEntityType::Player, rand(), owner, 0, 32);
	g_curNetObject = obj;
	auto syncTree = netSyncTree::GetForType(NetObjEntityType::Player);

	if (!obj)
	{
		spdlog::info("couldn't create object {} \n", __func__);

		return;
	}
	syncTree->ReadFromBuffer(1, 0, &buf, nullptr);
	if (!CanApplyToObjectRE((long long)syncTree, (long long)obj))
	{
		spdlog::info("couldn't apply object {}", __func__);

	}

//	auto creation_tree = (CPlayerCreationDataNode*)syncTree->GetCreationDataNode();
//	creation_tree->m_model = 0x0D7114C9;
	if (!CanApplyToObjectRE((long long)syncTree, (long long)obj))
	{
		spdlog::info("couldn't apply object {}", __func__);

	}
	if (!syncTree->CanApplyToObject(obj)) {
		spdlog::info("couldn't apply object {}", __func__);
		return;
	}
	syncTree->ApplyToObject(obj, nullptr);

	obj->syncData.isRemote = isRemote;
	obj->syncData.ownerId = owner;

	(*g_objectMgr)->RegisterNetworkObject(obj);

	if (obj->GetBlender())
	{
		obj->GetBlender()->SetTimestamp(((*g_queryFunctions)->GetTimestamp()));

		obj->PostSync();

		obj->GetBlender()->ApplyBlend();
		obj->GetBlender()->m_38();
	}

	obj->PostCreate();
	using pointer_to_handle_t = int32_t(*)(void* ptr);

	spdlog::info("got game object {}", (uintptr_t)obj->GetGameObject());
	auto ped = obj->GetGameObject();
	static auto fn = (pointer_to_handle_t)hook::pattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 8B 15 ? ? ? ? 48 8B F9 48 83 C1 10 33 DB").count(1).get(0).get<void>();

	auto handle = fn(ped);

	spdlog::info("Got player object : handle {}", handle);
	if (handle) {
		auto coors = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
		auto remote_coords = ENTITY::GET_ENTITY_COORDS(handle, true);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(PLAYER::PLAYER_PED_ID(), 0, 0, 0, true, true, true);
		spdlog::info("Remote player coords {} {} {}", remote_coords.x, remote_coords.y, remote_coords.z);
	}
	obj->syncData.creationAckedPlayers = 1;
}

static bool WrittenToBuffer = false;

netObject* CSyncManager::CreateCloneToLocalPlayer() {
	
	static char bluh[1000];
	static size_t writtenSize = 0;
	if (EnsurePlayer31()) {
		auto ts = *rage__s_NetworkTimeLastFrameStart;
		auto local = GetLocalPlayer();
		auto local_ped = getPlayerPedForNetPlayer(g_playerMgr->localPlayer);
		auto local_net = GetLocalPlayerPedNetObject();
		static bool Written = false;
		if (Written) {
			auto st = netSyncTree::GetForType(NetObjEntityType::Player);
			datBitBuffer buffer(bluh, writtenSize);
			if (st->ReadFromBuffer(1, 0, &buffer, nullptr)) {
				CPlayerCreationDataNode* node = (CPlayerCreationDataNode*)st->GetCreationDataNode();
				spdlog::info("remote net model_hash {}", node->m_model);
			}

		}
		else {
		
			memset(bluh, 0, sizeof(bluh));

			datBitBuffer buffer(bluh, sizeof(bluh));
			auto st = local_net->GetSyncTree();
			st->WriteTreeCfx(1, 0, local_net, &buffer, 0, nullptr, 31, nullptr, nullptr);
			CPlayerCreationDataNode* node = (CPlayerCreationDataNode*)st->GetCreationDataNode();
			spdlog::info("local net model_hash {}", node->m_model);
			writtenSize = buffer.GetDataLength();
			Written = true;
		}
		
	}
	return nullptr;
	
}
static std::string GetClassTypeName(void* ptr)
{
	
	std::stringstream ss;
	ss << std::hex << hook::get_unadjusted(*(uint64_t*)ptr);
	return ss.str();
}

static void TraverseSyncNode(netSyncNodeBase* node, netObject* object = nullptr)
{
	std::string objectName = GetClassTypeName(node);

	if (node->IsParentNode())
	{
		spdlog::info("\t%sParentNode<\n");
		spdlog::info("\t\tNodeIds<%d, %d, %d>,\n", node->flags1, node->flags2, node->flags3);


		for (auto child = node->firstChild; child; child = child->nextSibling)
		{
			TraverseSyncNode(child, object);
		}
	}
	else
	{
		auto nodeSize = (node->GetMaximumDataSizeInternal() / 8) + 1;

		spdlog::info("%sNodeWrapper<NodeIds<{}, {}, {}>, {}, {}>{}\n", "\t", node->flags1, node->flags2, node->flags3, objectName, nodeSize, ", ");
	}
}


class CSyncDataNodeFrequent : public CProjectBaseSyncDataNode {};

#pragma pack(push,4)
class CPedMovementDataNode : CSyncDataNodeFrequent
{
public:
	bool m_has_desired_move_blend_ratio_x; //0x00C0
	bool m_has_desired_move_blend_ratio_y; //0x00C1
	bool unk_00C2; //0x00C2
	float m_desired_move_blend_ratio_x; //0x00C4
	float m_desired_move_blend_ratio_y; //0x00C8
	float unk_00CC; //0x00CC
	float m_desired_pitch; //0x00D0
}; //Size: 0x00D4
static_assert(sizeof(CPedMovementDataNode) == 0xD4);
static void DumpSyncNode(netSyncNodeBase* node, std::string indent = "\t", bool last = true)
{
	
	if (node->IsParentNode())
	{
		//printf("%sParentNode<\n", indent);
		//printf("%s\tNodeIds<%d, %d, %d>,\n", indent, node->flags1, node->flags2, node->flags3);
	
		
		for (auto child = node->firstChild; child; child = child->nextSibling)
		{
			DumpSyncNode(child, indent + "\t", (child->nextSibling == nullptr));
		}
		

//
		//printf("%s>%s\n", indent, !last ? "," : "");
	}
	else
	{
		int nodeSize = 1024;

#ifdef GTA_FIVE
		nodeSize = (node->GetMaximumDataSizeInternal() / 8) + 1;
#endif
		
		if (node->flags1 == 87 && node->flags2 == 87 && node->flags3 == 0 && nodeSize == 5) {
			spdlog::info("found CPedMovementDataNode {}", (void*)node);
			auto movement_node = (CPedMovementDataNode*)(node);
			spdlog::info("m_desired_move_blend_ratio x: {} y: {} pitch: {}", movement_node->m_desired_move_blend_ratio_x, movement_node->m_desired_move_blend_ratio_y, movement_node->m_desired_pitch);
		}
		//printf("%sNodeWrapper<NodeIds<%d, %d, %d>, %p, %d>%s\n", indent, node->flags1, node->flags2, node->flags3, node, nodeSize, !last ? "," : "");
	}
}
static void DumpSyncTree(netSyncTree* syncTree)
{
	std::string objectName = GetClassTypeName(syncTree);

	printf("using %p = SyncTree<\n", syncTree);

	DumpSyncNode(syncTree->syncNode);

	printf(">;\n");
}
void CSyncManager::RunCloneTest() {

	//static auto show_debug_info_target = hook::pattern("48 8B C4 48 89 58 08 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 48 8D 0D").count(1).get(0).get<void>();
	////reinterpret_cast<__int64(__fastcall*)(float a1)>(show_debug_info_target)(0.f);
	

	static netObject* remoteClone = nullptr;
	if (GetAsyncKeyState(VK_F3)) {
		static bool run = false;
		if (EnsurePlayer31()) {
			auto local = GetLocalPlayer();

			if (!remoteClone && local) {
				remoteClone = CreateCloneToLocalPlayer();
			}
		}
	}
	


}
CSyncThread::CSyncThread() {
//	CScrEngine::CreateThread(this);
	CScrEngine::AddToList(this);
}

CSyncThread* syncThread = new CSyncThread();
void __fastcall ThisWritesMovementSpeedToCPedBlender(
	__int64 pedblender,
	__int64 a2,
	float real_movement_speed,
	float a4,
	char a5)
{
	float v7; // xmm6_4
	float v9; // xmm2_4
	float v10; // xmm5_4
	float v11; // xmm0_4
	float v12; // xmm1_4
	float v13; // xmm3_4
	float v14; // xmm1_4
	float v15; // xmm3_4
	float v16; // xmm1_4
	float v17; // xmm0_4
	float v18; // xmm0_4
	__m128 v19; // xmm1
	float v20; // xmm1_4
	float v21; // xmm2_4
	float v22; // xmm1_4
	float v23; // xmm0_4
	float v24; // xmm0_4

	auto flag_Shit = (*(_BYTE*)(pedblender + 0xB0) & 0x40);

	printf("flag_Shit %i  \n", flag_Shit);

	if ((*(_BYTE*)(pedblender + 0xB0) & 0x40) != 0)
	{

		v7 = -1.0;
		*(float*)a2 = a4;
		*(float*)(a2 + 4) = real_movement_speed;
		v9 = 0.0;
		v10 = (float)(real_movement_speed * real_movement_speed) + (float)(a4 * a4);
		v11 = *(float*)(pedblender + 0x64);
		if (v11 >= 0.0)
		{
			if (v11 == 0.0)
				v12 = 0.0;
			else
				v12 = 1.0;
		}
		else
		{
			v12 = -1.0;
		}
		if (v10 > (float)((float)(v11 * v11) * v12))
		{
			if (v10 == 0.0)
				v13 = 0.0;
			else
				v13 = 1.0 / std::sqrt(v10);
			v14 = v13;
			v15 = v13 * real_movement_speed;
			v16 = v14 * a4;
			*(float*)(a2 + 4) = v15;
			*(float*)a2 = v16;
			v17 = *(float*)(pedblender + 0x64);
			*(float*)a2 = v16 * v17;
			*(float*)(a2 + 4) = v15 * v17;
		}
		v18 = *(float*)(pedblender + 0x68);
		if (v18 >= 0.0)
		{
			if (v18 == 0.0)
				v7 = 0.0;
			else
				v7 = 1.0;
		}
		if (v10 < (float)((float)(v18 * v18) * v7))
		{
			v19 = *(__m128*)a2;
			v19.m128_f32[0] = (float)(v19.m128_f32[0] * v19.m128_f32[0]) + (float)(*(float*)(a2 + 4) * *(float*)(a2 + 4));
			if (v19.m128_f32[0] != 0.0)
				v9 = 1.0 / _mm_sqrt_ps(v19).m128_f32[0];
			v20 = v9;
			v21 = v9 * *(float*)(a2 + 4);
			v22 = v20 * *(float*)a2;
			*(float*)(a2 + 4) = v21;
			*(float*)a2 = v22;
			v23 = *(float*)(pedblender + 0x68);
			*(float*)a2 = v22 * v23;
			*(float*)(a2 + 4) = v21 * v23;
		}
	}
	else
	{
		*(_DWORD*)a2 = 0;
		if (!a5)
		{
			v24 = *(float*)(pedblender + 0x68);
			if (real_movement_speed >= v24)
			{
				//v24 = *(float*)(pedblender + 0x64);
				//if (real_movement_speed <= v24)
				v24 = real_movement_speed;            // this writes 
			}
			printf("setted real movement speeed \n");
			*(float*)(a2 + 4) = v24;
		}
	}

}
class CPedMovement
{
public:
	char pad_0000[48]; //0x0000
	float N00000192; //0x0030
	float N000001F1; //0x0034
	char pad_0038[32]; //0x0038
	float speed; //0x0058
	float N000001D4; //0x005C
	float N000001D7; //0x0060
	float max_speed; //0x0064
	char pad_0068[152]; //0x0068
}; //Size: 0x0100
static_assert(sizeof(CPedMovement) == 0x100);

class N00000052
{
public:
	char pad_0000[1328]; //0x0000
	class CPedMovement movement; //0x0530
	char pad_0630[824]; //0x0630
}; //Size: 0x0968
static_assert(sizeof(N00000052) == 0x968);
__int64 __fastcall DumpTasks(__int64 a1)
{
	__int64 i; // rcx

	if (*(_DWORD*)(a1 + 0x10) == -1)
		return 0i64;
	spdlog::info("DUMP TREE START->");
	for (i = *(_QWORD*)(a1 + 8i64 * *(int*)(a1 + 0x10) + 0x20);
		i /*&& *(__int16*)(i + 0x34) != type*/;
		i = *(_QWORD*)(i + 32))
	{
		spdlog::info("active task type {}", *(__int16*)(i + 0x34));
	}
	spdlog::info("DUMP TREE END[]");
	//   return i;
}
namespace CTask {
	__int64 __fastcall AllocateCTask(void* a1, __int64 a2, __int64 a3, __int64 a4) {
		static auto fn = hook::pattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8D 99 ? ? ? ? 49 8B F0 48 8B EA 48 83 3B 00 48 8B F9 74 09 48 8B CB FF 15 ? ? ? ? ").count(1).get(0).get<void>();
		return reinterpret_cast<__int64(__fastcall*)(void* a1, __int64 a2, __int64 a3, __int64 a4)>(fn)(a1, a2, a3, a4);
	}
	__int64 __fastcall ConstructCTaskMovePlayer(void* a1) {
		static auto fn = hook::pattern("48 89 5C 24 ? 57 48 83 EC 20 F3 0F 10 0D ? ? ? ? 48 8B D9 E8 ? ? ? ? 33 FF 48 8D 05 ? ? ? ? 48 89 03 48 8D 05 ? ? ? ? C6 83 ? ? ? ? ? 48 89 BB ? ? ? ? 48 89 BB ? ? ? ? 48 89 83 ? ? ? ? F3 0F 10 05 ? ? ? ? F3 0F 11 83 ? ? ? ? B9 ? ? ? ? C7 83 ? ? ? ? ? ? ? ? 89 BB ? ? ? ? 48 C7 83 ? ? ? ? ? ? ? ?").count(1).get(0).get<void>();
		return reinterpret_cast<__int64(__fastcall*)(void* a1)>(fn)(a1);
	}
	void __fastcall AddCTaskToSomeListUnkown(__int64 a1, __int64 ctasklist)
	{
		static auto fn = hook::pattern("48 89 5C 24 ? 57 48 83 EC 20 48 8D 59 28 48 8B FA 48 8B 0B 48 85 C9 74 0B 48 8B 01 BA ? ? ? ? FF 50 30").count(1).get(0).get<void>();
		reinterpret_cast<__int64(__fastcall*)(__int64 a1, __int64 ctasklist)>(fn)(a1, ctasklist);
	}
	__int64 __fastcall ConstructCTaskComplexControlMovement(
		__int64 ctask,
		__int64 task1,
		__int64 task2,
		int a4,
		int a5,
		char a6,
		int a7)
	{
		
	static auto fn = hook::pattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 41 8B E9 49 8B F8 48 8B F2 48 8B D9 E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 8D 93 ? ? ? ? 48 89 03 48 89 32 48 85 F6 74 08 48 8B CE E8 ? ? ? ?").count(1).get(0).get<void>();
		return reinterpret_cast<__int64(__fastcall*)(__int64 ctask,
			__int64 task1,
			__int64 task2,
			int a4,
			int a5,
			char a6,
			int a7)>(fn)(ctask, task1, task2, a4,a5,a6,a7);
	}
}
void CSyncManager::RunCoopTest() {
	static int remotePed = 0;
	static auto get_entity = static_cast<std::uintptr_t(*)(int)>(hook::pattern("83 F9 FF 74 31 4C 8B 0D ? ? ? ? 44 8B C1 49 8B 41 08").count(1).get(0).get<void>(0));
	static void** ConstructorMutex = hook::get_address<decltype(ConstructorMutex)>(hook::get_pattern("48 8B 0D ? ? ? ? 41 B8 ? ? ? ? E8 ? ? ? ? 48 8B D8 48 85 C0 75 0A B9 ? ? ? ?"));
	
	if (GetAsyncKeyState(VK_F1) && !remotePed) {
		STREAMING::REQUEST_MODEL(0x705E61F2);
		if (STREAMING::HAS_MODEL_LOADED(0x705E61F2)) {
			//	script::get_current()->yield();

			GamePed player = PLAYER::PLAYER_PED_ID();
			auto coords = ENTITY::GET_ENTITY_COORDS(player, true);

			remotePed = PED::CREATE_PED(1, 0x705E61F2, coords.x, coords.y, coords.z, 0.f, true, false);


			printf("sa spawnat cacatu \n");
		}

	}
	if (remotePed) {
		auto local_ped = (*g_pedFactory)->m_local_ped;
		__int64 c_ped = (__int64)(get_entity(remotePed));

		if (local_ped && c_ped) {
			ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(remotePed, PLAYER::PLAYER_PED_ID(), true);

			static bool wasMoving = false;
			auto pedBlender = (__int64)local_ped + 0x530;
			float movementSpeed = *(float*)(pedBlender + 0x58);
			auto local_coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
			auto remote_coords = ENTITY::GET_ENTITY_COORDS(remotePed, true);
			auto heading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
			auto velocity_gta = ENTITY::GET_ENTITY_VELOCITY(PLAYER::PLAYER_PED_ID());

			auto pos = glm::vec3(local_coords.x, local_coords.y, local_coords.z);
			auto velocity = glm::vec3(velocity_gta.x, velocity_gta.y, velocity_gta.z);

			

			glm::vec3 posPredict = pos + (velocity) *movementSpeed;
		
			auto local_ang = ENTITY::GET_ENTITY_ROTATION_VELOCITY(PLAYER::PLAYER_PED_ID());

			//ENTITY::SET_ENTITY_HEADING(remotePed, heading);
			//ENTITY::_SET_ENTITY_ANGULAR_VELOCITY(remotePed, local_ang.x, local_ang.y, local_ang.z);
		//	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(remotePed, local_coords.x - 1, local_coords.y + 1, local_coords.z, true, true, true);

			int speed = (int)movementSpeed;
			//BRAIN::SET_PED_DESIRED_MOVE_BLEND_RATIO(remotePed, movementSpeed);
		/*	PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(remotePed, 1.49f);*/
			//auto inteligence = c_ped + 0x10A0;
			//auto tree = inteligence + 0x370;
			//static bool a = false;
			//if (!a) {
			//	a = true;
			//	auto mem = CTask::AllocateCTask(*ConstructorMutex, 216i64, 16i64, 0);

			//	auto  move_player_task = CTask::ConstructCTaskMovePlayer((void*)mem);
			//	
			//	(*(void(__fastcall**)(_QWORD, __int64, __int64))(**(_QWORD**)(*(_QWORD*)(c_ped + 0x10A0) + 0x380i64) + 64i64))(
			//		*(_QWORD*)(*(_QWORD*)(c_ped + 0x10A0) + 0x380i64),
			//		move_player_task,
			//		1i64);
			//	/*CTask::ConstructCTaskComplexControlMovement(mem)
			//	CTask::AddCTaskToSomeListUnkown(c_ped,)*/
			//	spdlog::info("created move_player_task {}", move_player_task);
			//}
		 //   DumpTasks(*(_QWORD*)(*(_QWORD*)(c_ped + 0x10A0) + 0x370i64));

		
			GRAPHICS::DRAW_LINE(local_coords.x, local_coords.y, local_coords.z, posPredict.x, posPredict.y, posPredict.z, 255, 255, 0, 255);
		//	BRAIN::TASK_FORCE_MOTION_STATE(remotePed, Hash.)
			//spdlog::info("movementSpeed {}", movementSpeed);
			/*if (movementSpeed > 0.f)
				BRAIN::TASK_GO_STRAIGHT_TO_COORD(remotePed, posPredict.x, posPredict.y, posPredict.z, movementSpeed, -1, 0.0f, 0.0f);
			else
				BRAIN::TASK_STAND_STILL(remotePed, 10000);*/
		//	BRAIN::TASK_ACHIEVE_HEADING(remotePed, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()), -1);

			PLAYER::SIMULATE_PLAYER_INPUT_GAIT(remotePed, 1.f, 100, 1.f, 1, 0);
			PLAYER::SIMULATE_PLAYER_INPUT_GAIT(PLAYER::GET_PLAYER_INDEX(), 1.f, 100, 1.f, 1, 0);

			ThisWritesMovementSpeedToCPedBlender(c_ped + 0x530, c_ped + 0x584, movementSpeed, 0.f, 0);
		/*	if (movementSpeed > 0.f) {
			
				spdlog::info("updating sync for ped {} speed {}", remotePed, movementSpeed);
				wasMoving = true;
			}
			else {
				if (wasMoving) {
					BRAIN::TASK_STAND_STILL(remotePed, 2000);
					wasMoving = false;
				}
			}*/
		}
	}

}
void CSyncManager::OnFrame() {
	
	if (!g_pedFactory)
		g_pedFactory = hook::get_address<decltype(g_pedFactory)>(hook::get_pattern("E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 58 08 48 8B CB E8", 8));
	
	//mov     rax, cs:g_PedFactory
	/*{
		if (*g_pedFactory)
		{
			std::uintptr_t local_ped = (std::uintptr_t)((*g_pedFactory)->m_local_ped);
			if (local_ped) {
				auto motiondata = local_ped + 0x588;

				spdlog::info("local_ped {} motion_data {}", local_ped, motiondata);
				
			}
		}

	}*/
	

	return;
	

	//if (g_pedFactory) {

	//	auto ped = (*g_pedFactory)->m_local_ped;
	//	static GamePed remotePed = 0;
	//	static bool Spawned = false;
	//	if (!Spawned && GetAsyncKeyState(VK_F2)) {

	//		STREAMING::REQUEST_MODEL(0x705E61F2);
	//		while (!STREAMING::HAS_MODEL_LOADED(0x705E61F2))
	//			script::get_current()->yield();

	//		GamePed player = PLAYER::PLAYER_PED_ID();
	//		auto coords = ENTITY::GET_ENTITY_COORDS(player, true);

	//		remotePed = PED::CREATE_PED(1, 0x705E61F2, coords.x, coords.y, coords.z, 0.f, true, false);
	//		BRAIN::TASK_GO_STRAIGHT_TO_COORD(remotePed, 100.f, 100.f, 100.f, 4.0f, 1000, 0.0f, 0.0f);
	//		Spawned = true;
	//		printf("sa spawnat cacatu \n");
	//		
	//	}
	//	if (ped) {
	//		auto pedBlender = (__int64)ped + 0x530;

	//		auto class_blend = (CBlender*)(pedBlender);

	//		bool running = Is_PedRunning_Internal((__int64)ped + 0x530);

	//		float movementSpeed = *(float*)(pedBlender + 0x58);
	//		
	//		spdlog::info("ped {} idk {}", (void*)ped, (void*)pedBlender);

	//	}

	//	if (ped && remotePed) {
	//		return;
	//	
	//		auto local_handle = PLAYER::PLAYER_PED_ID();
	//		
	//		auto local_coords = ENTITY::GET_ENTITY_COORDS(local_handle, true);

	//	
	//	/*	ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(remotePed, local_handle, false);*/

	//		BRAIN::SET_PED_DESIRED_MOVE_BLEND_RATIO(remotePed, BRAIN::GET_PED_DESIRED_MOVE_BLEND_RATIO(local_handle));
	//	
	//	
	//		//pCtrlMove = reinterpret_cast<void* (__fastcall*)(__int64 a1, int a2, int a3)>(FindTaskPrimaryByType)(*(_QWORD*)(*(_QWORD*)(v0 + 0x10A0) + 0x370), 35, 3);

	//		//auto a1 = *(_QWORD*)(*(_QWORD*)(v0 + 0x10A0) + 0x370);
	//		__int64 pedInteligence; // rdi
	//		__int64 pedTaskManager; // rcx
	//		__int64 v4; // rax
	//		__int64 v5; // rax
	//		using handle_to_pointer_t = int64_t(__fastcall*)(int handle);
	//		static auto HandleToPointer = hook::get_address<handle_to_pointer_t>("83 f9 ff 74 31 4c 8b 0d ? ? ? ? 44 8b c1 49 8b 41 08");

	//		__int64 CurrentNativePlayerTarget = HandleToPointer(local_handle);

	//		spdlog::info("CurrentNativePlayerTarget {} ", CurrentNativePlayerTarget);
	//		return;

	//		/*(*(void(__fastcall**)(_QWORD, __int64, __int64))(**(_QWORD**)(pedInteligence + 0x370) + 64i64))(
	//			*(_QWORD*)(pedInteligence + 0x370),
	//			task,
	//			4i64);
	//		(*(void(__fastcall**)(__int64))(*(_QWORD*)(pedInteligence + 872) + 56i64))(pedInteligence + 872);*///add task as primary??

	//		pedInteligence = *(_QWORD*)(CurrentNativePlayerTarget + 0x10A0);
	//		pedTaskManager = *(_QWORD*)(pedInteligence + 0x370);

	//		auto taskTreeMovement = *(_QWORD*)(pedInteligence + 0x380);

	//		spdlog::info("pedInteligence {} pedTaskManager {} taskTreeMovement {}", pedInteligence, pedTaskManager, taskTreeMovement);
	//		return;

	//		if (*(_DWORD*)(pedTaskManager + 16) != 3)
	//			return;
	//		if (*(_DWORD*)(*(_QWORD*)(pedInteligence + 0x380) + 16i64) != 1)
	//			return;

	//		__int64 pCtrlMove = 0;// FindTaskPrimaryByTypeRE(pedTaskManager, 35, 3);

	//		//auto task = FindTaskPrimaryByTypeRE(pedTaskManager, 35, 3);
	//		//auto task = reinterpret_cast<void* (__fastcall*)(__int64 a1, int a2, int a3)>(FindTaskPrimaryByType)(pedTaskManager, 35, 3);
	//		spdlog::info("pCtrlMove {}", pCtrlMove);
	//		if (pCtrlMove) {


	//			auto RunningMovementTask = GetRunningMovementTaskRE(pCtrlMove);
	//			*(float*)(RunningMovementTask + 0xC4) = local_coords.x;
	//			*(float*)(RunningMovementTask + 0xC8) = local_coords.y;
	//			*(float*)(RunningMovementTask + 0xC0) = local_coords.z;

	//			spdlog::info("goto coords  data from running movement {} {} {}", *(float*)(RunningMovementTask + 0xC4), *(float*)(RunningMovementTask + 0xC8), *(float*)(RunningMovementTask + 0xC0));
	//			//


	//		}

	//	}
	//}
	
}

void CSyncManager::OnGameHook() {

}
CNetGamePlayer* CSyncManager::GetLocalPlayer()
{
	return g_playerMgr->localPlayer;
}
CNetGamePlayer* CSyncManager::GetPlayerByNetId(uint16_t netId)
{
	return g_playersByNetId[netId];
}
bool CSyncManager::EnsurePlayer31()
{
	if (!g_player31)
	{
		g_player31 = AllocateNetPlayer(nullptr);
		g_player31->physicalPlayerIndex() = 31;
		//g_player31 = TempHackMakePhysicalPlayerImpl(55, 31);
	}

	return (g_player31 != nullptr);
}


CNetGamePlayer* CSyncManager::AllocateNetPlayer(void* mgr)
{
	void* plr = malloc(704);

	static auto _netPlayerCtor = hook::get_pattern<CNetGamePlayer* (void*)>("83 8B ? 00 00 00 FF 33 F6", -0x17);
	auto player = _netPlayerCtor(plr);
	spdlog::info("AllocateNetPlayer called ");

	return player;
}
netObject* CSyncManager::GetNetworkObject(uint16_t id, bool a3) {
	static auto fn = hook::get_pattern< netObject* (netObjectMgr*, uint16_t, bool)>("44 38 B1 08 27 00 00 0F 84", -0x23);
	return fn(*g_objectMgr, id, a3);
}

netObject* CSyncManager::GetLocalPlayerPedNetObject() {
	auto ped = getPlayerPedForNetPlayer(g_playerMgr->localPlayer);

	if (ped)
	{
		auto netObj = *(netObject**)((char*)ped + g_entityNetObjOffset);

		return netObj;
	}

	return nullptr;
}
void* CSyncManager::getPlayerPedForNetPlayer(CNetGamePlayer* player) {
	static auto fn = hook::get_call(hook::get_pattern<void* (CNetGamePlayer*)>("84 C0 74 1C 48 8B CF E8 ? ? ? ? 48 8B D8", 7));
	return fn(player);
}
netObject* CSyncManager::CreateCloneObject(NetObjEntityType type, uint16_t objectId, uint8_t a2, int a3, int a4) {
	auto pool = *validatePools[(int)type];

	/*if (pool->GetCountDirect() >= pool->GetSize())
	{
		return nullptr;
	}*/

	/*if (type == NetObjEntityType::Ped || type == NetObjEntityType::Player)
	{
		auto entityPool = CPoolManagement::GetPoolBase("Peds");

		if (entityPool->GetCountDirect() >= (entityPool->GetSize() - 4))
		{
			return nullptr;
		}
	}*/

	return createCloneFuncs[(int)type](objectId, a2, a3, a4);
}
void CSyncManager::EnsureCreateCloneFuncs() {
	auto location = hook::get_pattern<char>("0F 8E 12 03 00 00 41 8A", 22);

	createCloneFuncs[(int)NetObjEntityType::Ped] = (TCreateCloneObjFn)hook::get_call(location);
	createCloneFuncs[(int)NetObjEntityType::Object] = (TCreateCloneObjFn)hook::get_call(location + 0x39);
	createCloneFuncs[(int)NetObjEntityType::Heli] = (TCreateCloneObjFn)hook::get_call(location + 0x72);
	createCloneFuncs[(int)NetObjEntityType::Door] = (TCreateCloneObjFn)hook::get_call(location + 0xAB);
	createCloneFuncs[(int)NetObjEntityType::Boat] = (TCreateCloneObjFn)hook::get_call(location + 0xE4);
	createCloneFuncs[(int)NetObjEntityType::Bike] = (TCreateCloneObjFn)hook::get_call(location + 0x11D);
	createCloneFuncs[(int)NetObjEntityType::Automobile] = (TCreateCloneObjFn)hook::get_call(location + 0x156);
	createCloneFuncs[(int)NetObjEntityType::Pickup] = (TCreateCloneObjFn)hook::get_call(location + 0x18F);
	createCloneFuncs[(int)NetObjEntityType::Train] = (TCreateCloneObjFn)hook::get_call(location + 0x1EF);
	createCloneFuncs[(int)NetObjEntityType::Trailer] = (TCreateCloneObjFn)hook::get_call(location + 0x228);
	createCloneFuncs[(int)NetObjEntityType::Player] = (TCreateCloneObjFn)hook::get_call(location + 0x261);
	createCloneFuncs[(int)NetObjEntityType::Submarine] = (TCreateCloneObjFn)hook::get_call(location + 0x296);
	createCloneFuncs[(int)NetObjEntityType::Plane] = (TCreateCloneObjFn)hook::get_call(location + 0x2C8);
	createCloneFuncs[(int)NetObjEntityType::PickupPlacement] = (TCreateCloneObjFn)hook::get_call(location + 0x2FA);

	validatePools[(int)NetObjEntityType::Ped] = (TPoolPtr)hook::get_address<void*>(location - 42);
	validatePools[(int)NetObjEntityType::Object] = (TPoolPtr)hook::get_address<void*>(location + 13);
	validatePools[(int)NetObjEntityType::Heli] = (TPoolPtr)hook::get_address<void*>(location + 70);
	validatePools[(int)NetObjEntityType::Door] = (TPoolPtr)hook::get_address<void*>(location + 127);
	validatePools[(int)NetObjEntityType::Boat] = (TPoolPtr)hook::get_address<void*>(location + 184);
	validatePools[(int)NetObjEntityType::Bike] = (TPoolPtr)hook::get_address<void*>(location + 241);
	validatePools[(int)NetObjEntityType::Automobile] = (TPoolPtr)hook::get_address<void*>(location + 298);
	validatePools[(int)NetObjEntityType::Pickup] = (TPoolPtr)hook::get_address<void*>(location + 355);
	validatePools[(int)NetObjEntityType::Train] = (TPoolPtr)hook::get_address<void*>(location + 453);
	validatePools[(int)NetObjEntityType::Trailer] = (TPoolPtr)hook::get_address<void*>(location + 508);
	validatePools[(int)NetObjEntityType::Player] = (TPoolPtr)hook::get_address<void*>(location + 565);
	validatePools[(int)NetObjEntityType::Submarine] = (TPoolPtr)hook::get_address<void*>(location + 622);
	validatePools[(int)NetObjEntityType::Plane] = (TPoolPtr)hook::get_address<void*>(location + 672);
	validatePools[(int)NetObjEntityType::PickupPlacement] = (TPoolPtr)hook::get_address<void*>(location + 722);

}
CSyncManager* g_SyncManager = new CSyncManager();

static bool mD0Stub(netSyncTree* tree, int a2)
{
	return false;
}

static void netObjectMgrBase__RegisterNetworkObject(netObjectMgr* manager, netObject* object)
{
	//CD_AllocateSyncData(object->GetObjectId());


	/*if (!CloneObjectMgr->RegisterNetworkObject(object))
	{
		return;
	}*/

	if (!g_syncData[object->GetObjectId()])
	{
		g_syncData[object->GetObjectId()] = std::make_unique<NetObjectData>();
	}

	// create a blender, if not existent
	if (!object->GetBlender())
	{
		object->CreateNetBlender();
	}

	if (!object->syncData.isRemote)
	{
#ifdef GTA_FIVE
		if (object->CanSynchronise(true))
#elif IS_RDR3
		uint32_t reason;
		if (object->CanSynchronise(true, &reason))
#endif
		{
			object->StartSynchronising();
		}
	}


	spdlog::info("g_orig_netObjectMgrBase__RegisterNetworkObject {} mata: {}", object->GetTypeString(), *(std::uintptr_t*)((std::uintptr_t)object + 0x50));
	object->OnRegistered();
}
CNetGamePlayer* netObject__GetPlayerOwner(netObject* object)
{
	//spdlog::info("netObject__GetPlayerOwner called for objectId {}", object->objectId);
	if (object && object->syncData.ownerId == 31)
	{

		CSyncManager::EnsurePlayer31();
		return CSyncManager::g_player31;
	}


	return CSyncManager::g_playerMgr->localPlayer;
}
bool netSyncTree::CanApplyToObject(netObject* object)
{
	static auto fn = hook::get_pattern<bool(netSyncTree* self, netObject* obj)>("49 8B 06 49 8B CE FF 90 A0 00 00 00 84 C0", -0x29);
	return fn(this, object);
}

bool netSyncTree::ReadFromBuffer(int flags, int flags2, datBitBuffer* buffer, void* netLogStub)
{
	static auto fn = hook::get_pattern<bool(netSyncTree* self, int flags, int flags2, datBitBuffer* buffer, void* netLogStub)>("44 89 40 18 57 48 83 EC 30 44 8B 05 ? ? ? ? 65", -15);
	return fn(this, flags, flags2, buffer, netLogStub);
}

netSyncTree* netSyncTree::GetForType(NetObjEntityType type)
{
	static auto fn = hook::get_pattern< netSyncTree* (void*, int)>("0F B7 CA 83 F9 07 7F 5E");
	return fn(CSyncManager::netSynctreeFirstArg, (int)type);
}

inline uint32_t GetDelayForUpdateFrequency(uint8_t updateFrequency)
{
	switch (updateFrequency)
	{
	case 5:
		return 0;
	case 4:
		return 25;
	case 3:
		return 100;
	case 2:
		return 300;
	case 1:
		return 400;
	case 0:
	default:
		return 1000;
	}
}
static void AddNodeAndExternalDependentNodes(netSyncDataNodeBase* node, netSyncDataNodeBase* children[], size_t* childCount, size_t maxChildren)
{
	// if we still have space
	if (*childCount < maxChildren)
	{
		// add self
		children[*childCount] = node;
		(*childCount)++;

		// add children
		for (size_t i = 0; i < node->externalDependencyCount; i++)
		{
			AddNodeAndExternalDependentNodes(node->externalDependencies[i], children, childCount, maxChildren);
		}
	}
}
bool netSyncTree::WriteTreeCfx(int flags, int objFlags, netObject* object, datBitBuffer* buffer, uint32_t time, void* logger, uint8_t targetPlayer, void* outNull, uint32_t* lastChangeTime)
{
	auto syncData = g_syncData[object->GetObjectId()].get();

	if (!syncData)
	{
		return false;
	}

	InitTree(this);

	WriteTreeState state;
	state.object = object;
	state.flags = flags;
	state.objFlags = objFlags;
	state.buffer = buffer;
	state.logger = (netLogStub*)logger;
	state.time = time;
	state.wroteAny = false;
	state.lastChangeTimePtr = lastChangeTime;

	if (lastChangeTime)
	{
		*lastChangeTime = 0;
	}

	if (flags == 2 || flags == 4)
	{
		// mA0 bit
		buffer->WriteBit(objFlags & 1);
	}

#ifdef IS_RDR3
	buffer->WriteBit(0);
#endif

	// #NETVER: 2018-12-27 17:41 -> increased maximum packet size to 768 from 256 to account for large CPlayerAppearanceDataNode
	//static auto icgi = Instance<ICoreGameInit>::Get();

	int sizeLength = 13;

	//if (icgi->OneSyncBigIdEnabled)
	{
		sizeLength = 16;
	}
	///else if (icgi->NetProtoVersion < 0x201812271741)
	//{
	//	sizeLength = 11;
	//}

	eastl::bitset<200> processedNodes;

	// callback
	auto nodeWriter = [this, sizeLength, syncData, &processedNodes](WriteTreeState& state, netSyncNodeBase* node, const std::function<bool()>& cb)
	{
		auto buffer = state.buffer;
		bool didWrite = false;

		size_t nodeIdx = GET_NIDX(this, node);

		if (state.flags & node->flags1 && (!node->flags3 || state.objFlags & node->flags3))
		{
			// save position to allow rewinding
			auto startPos = buffer->GetPosition();

			if (state.pass == 2)
			{
				// write presence header placeholder
				if (node->flags2 & state.flags)
				{
					buffer->WriteBit(0);
				}

				// write Cfx length placeholder
				if (node->IsDataNode())
				{
#ifndef ONESYNC_CLONING_NATIVES
					buffer->WriteUns(0, sizeLength);
#endif
				}
			}

			if (node->IsParentNode())
			{
				didWrite = cb();
			}
			else
			{
				// compare last data for the node
				auto nodeData = &syncData->nodes[nodeIdx];
				uint32_t nodeSyncDelay = GetDelayForUpdateFrequency(node->GetUpdateFrequency(UpdateLevel::VERY_HIGH));

				// throttle sends by waiting for the requested node delay
				uint32_t lastChangeDelta = (state.time - nodeData->lastChange);

				if (state.pass == 1 && (lastChangeDelta > nodeSyncDelay || nodeData->manuallyDirtied))
				{
					auto updateNode = [this, syncData, &state, &processedNodes, sizeLength](netSyncDataNodeBase* dataNode, bool force) -> bool
					{
						size_t dataNodeIdx = GET_NIDX(this, dataNode);
						auto nodeData = &syncData->nodes[dataNodeIdx];

						if (processedNodes.test(dataNodeIdx))
						{
							return nodeData->lastChange == state.time || force;
						}

						// calculate node change state
						std::array<uint8_t, 1024> tempData;
						memset(tempData.data(), 0, tempData.size());

#ifdef GTA_FIVE
					//	LoadPlayerAppearanceDataNode(dataNode);
#endif

						datBitBuffer tempBuf(tempData.data(), (sizeLength == 11) ? 256 : tempData.size());
						dataNode->WriteObject(state.object, &tempBuf, state.logger, true);

#ifdef GTA_FIVE
						//StorePlayerAppearanceDataNode(dataNode);
#endif

						if (force || tempBuf.m_curBit != std::get<1>(nodeData->lastData) || memcmp(tempData.data(), std::get<0>(nodeData->lastData).data(), tempData.size()) != 0)
						{
							nodeData->lastResend = 0;
							nodeData->lastChange = state.time;
							nodeData->lastData = { tempData, tempBuf.m_curBit };
							nodeData->currentData = { tempData, tempBuf.m_curBit };
							nodeData->manuallyDirtied = false;

							return true;
						}

						processedNodes.set(dataNodeIdx);

						return false;
					};

					auto dataNode = (netSyncDataNodeBase*)node;

#ifdef GTA_FIVE
					static_assert(offsetof(netSyncDataNodeBase, externalDependentNodeRoot) == 0x50, "parentData off");
#elif IS_RDR3
					static_assert(offsetof(netSyncDataNodeBase, externalDependentNodeRoot) == 0x48, "parentData off");
#endif

					// if we are a data node, we will have to ensure external-dependent nodes are sent as a bundle at all times
					// this means:
					// 1. trace up to the root of the external-dependent node tree.
					// 2. from this point, *recursively* dirty all children, for we are dirty as well.
					//
					// the original implementation here did not take netSyncTree::Update nor AddNodeAndExternalDependentNodes as a reference
					// and wasn't tracing up to the root of the external-dependent node tree, nor did it recursively dirty children.

					if (dataNode->externalDependentNodeRoot || dataNode->externalDependencyCount > 0)
					{
						auto rootNode = dataNode;

						while (rootNode->externalDependentNodeRoot)
						{
							rootNode = rootNode->externalDependentNodeRoot;
						}

						if (rootNode)
						{
							netSyncDataNodeBase* children[16];
							size_t childCount = 0;

							AddNodeAndExternalDependentNodes(rootNode, children, &childCount, std::size(children));

							bool written = false;

							for (int child = 0; child < childCount; child++)
							{
								size_t childIdx = GET_NIDX(this, children[child]);
								auto childData = &syncData->nodes[childIdx];

								written |= updateNode(children[child], nodeData->manuallyDirtied || childData->manuallyDirtied || written);
							}
						}
					}
					else
					{
						updateNode(dataNode, nodeData->manuallyDirtied);
					}
				}

				// resend skipping is broken, perhaps?
				bool isResendSkipped = false;//((state.time - nodeData->lastResend) < 150);

				if (state.pass == 2)
				{
					if (state.lastChangeTimePtr)
					{
						auto oldVal = *state.lastChangeTimePtr;

						if (nodeData->lastChange > oldVal && !isResendSkipped)
						{
							*state.lastChangeTimePtr = nodeData->lastChange;

							nodeData->lastResend = state.time;
						}
					}
				}

				bool shouldWriteNode = false;

				if (!(node->flags2 & state.flags))
				{
					shouldWriteNode = true;
				}

				if (nodeData->lastAck < nodeData->lastChange)
				{
					shouldWriteNode = true;
				}

				if (isResendSkipped)
				{
					shouldWriteNode = false;
				}

				if (shouldWriteNode)
				{
					if (state.pass == 2)
					{
						buffer->WriteBits(std::get<0>(nodeData->currentData).data(), std::get<1>(nodeData->currentData), 0);
					}

					didWrite = true;
				}
			}

			if (!didWrite)
			{
				if (state.pass == 2)
				{
					// set position to just past the 0
					if (node->flags2 & state.flags)
					{
						buffer->Seek(startPos + 1);
					}
					else
					{
						buffer->Seek(startPos);
					}
				}
			}
			else
			{
				state.wroteAny = true;

				if (state.object)
				{
					g_netObjectNodeMapping[state.object->GetObjectId()][node] = { 1, (*CSyncManager::g_queryFunctions)->GetTimestamp()};
				}

				uint32_t endPos = buffer->GetPosition();

				if (state.pass == 2)
				{
					buffer->Seek(startPos);

					if (node->flags2 & state.flags)
					{
						buffer->WriteBit(true);
					}
				}

				if (node->IsDataNode())
				{
					auto length = endPos - startPos - sizeLength;

					if (node->flags2 & state.flags)
					{
						length -= 1;
					}

#if 0
					if (length >= (1 << 13))
					{
						auto extraDumpPath = MakeRelativeCitPath(L"data\\cache\\extra_dump_info.bin");

						auto f = _wfopen(extraDumpPath.c_str(), L"wb");

						if (f)
						{
							fwrite(buffer->m_data, 1, buffer->m_maxBit / 8, f);
							fclose(f);
						}

						trace("Node type: %s\n", typeid(*node).name());
						trace("Start offset: %d\n", startPos);

						FatalError("Tried to write a bad node length of %d bits in a '%s'. There should only ever be 8192 bits. Please report this on https://forum.fivem.net/t/318260 together with the .zip file from 'save information' below.", length, typeid(*node).name());
					}
#endif

					if (state.pass == 2)
					{
#ifndef ONESYNC_CLONING_NATIVES
						buffer->WriteUns(length, sizeLength);
#endif
					}
				}

				if (state.pass == 2)
				{
					buffer->Seek(endPos);
				}
			}
		}

		return didWrite;
	};

	// traverse state and dirty nodes first
	state.pass = 1;
	TraverseTree<WriteTreeState>(this, state, nodeWriter);

	// then traverse again, writing nodes
	state.pass = 2;
	state.wroteAny = false;
	TraverseTree<WriteTreeState>(this, state, nodeWriter);

	return state.wroteAny;
}
bool datBitBuffer::ReadInteger(uint32_t* integer, int bits)
{
	static auto fn = hook::get_pattern<bool(void*, uint32_t*, int)>("8B 44 24 30 8B D6 48 8B CB 89 07 E8", -0x50);
	return fn(this, integer, bits);
}
bool datBitBuffer::WriteInteger(uint32_t integer, int bits)
{
	static auto fn = hook::get_pattern<bool(void*, uint32_t, int)>("48 8B D9 40 84 79 1C 75 6F 8B 49 10", -0x29);
	return fn(this, integer, bits);
}
bool datBitBuffer::WriteUns(uint32_t integer, int bits)
{
	
	// this method only exists inlined in GTA V so we have to manually implement it
	if (m_f1C & 1)
	{
		return false;
	}

	if (m_unkBit + bits > m_maxBit)
	{
		return false;
	}

	if (!(m_f1C & 2))
	{
		_netBuffer_WriteUnsigned(m_data, integer, bits, m_f8 + m_unkBit);
	}

	_netBuffer_BumpReadWriteCursor(this, bits);

	return true;
}


bool datBitBuffer::WriteBit(bool bit)
{
	static auto fn = hook::get_pattern<bool(void*, bool)>("F6 43 1C 01 75 5E", -0xF);
	return fn(this, bit);
}

bool datBitBuffer::WriteBits(const void* src, size_t length, size_t srcOffset)
{
	static auto fn = hook::get_pattern<bool(void*, const void* src, size_t length, size_t srcOffset)>("48 8B D9 75 44 8B 49 10", -0x14);
	return fn(this, src, length, srcOffset);
}


void FindTaskPrimaryByType_IDA(__int64 a1, int a2, int a3)
{
	__int64 i; // r8

	spdlog::info("task tree type {}", a3);

	for (i = *(_QWORD*)(a1 + 8i64 * a3 + 32); ; i = *(_QWORD*)(i + 32))
	{
		if (!i)
			continue;
		__int16 type = *(__int16*)(i + 52);
		spdlog::info("task type id {}", *(__int16*)(i + 52));
		/*if (*(__int16*)(i + 52) == a2)
			break;*/
	}

}
void CSyncThread::DoRun()
{
	
//	spdlog::info("CSyncThread:DoRun");
	if (g_pedFactory) {
		__try {
			auto local_ped = (*g_pedFactory)->m_local_ped;

			auto local_ped_inteligence = (std::uintptr_t)(local_ped)+0x10A0;



			if (local_ped) {
				CBaseFactory<CSyncManager>::Get().RunCloneTest();

			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER){
			spdlog::info("nu mearsa last_error {}", GetLastError());
		}
	}
	/*if (GetAsyncKeyState(VK_F3)) {
		STREAMING::REQUEST_MODEL(0x705E61F2);
		while (!STREAMING::HAS_MODEL_LOADED(0x705E61F2))
			SYSTEM::WAIT(0);

		GamePed player = PLAYER::PLAYER_PED_ID();
		auto coords = ENTITY::GET_ENTITY_COORDS(player, true);

		 PED::CREATE_PED(1, 0x705E61F2, coords.x, coords.y, coords.z, 0.f, true, false);
		 spdlog::info("Created ped");
	}*/
}

