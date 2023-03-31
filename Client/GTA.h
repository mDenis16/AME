#pragma once
#include "XBRVirtual.h"
#include <iostream>
#include <sstream>
#define GTA_FIVE

enum class NetObjEntityType
{
	Automobile = 0,
	Bike = 1,
	Boat = 2,
	Door = 3,
	Heli = 4,
	Object = 5,
	Ped = 6,
	Pickup = 7,
	PickupPlacement = 8,
	Plane = 9,
	Submarine = 10,
	Player = 11,
	Trailer = 12,
	Train = 13,
	Max = 14
};
inline int MapNetSyncNodeMethod(int offset)
{
#ifdef GTA_FIVE
	
	{
		// in 2545 there's now 2 calculator functions in common operations,
		// we need the old one that was added as a new function, mapping it manually.
		if (offset == 0xB8)
		{
			return 0x98;
		}

		if (offset >= 0x40)
		{
			offset += 0x8;
		}

		if (offset >= 0x88)
		{
			offset += 0x8;
		}
	}
#endif

	return offset;
}

class netLogStub
{
public:
	virtual ~netLogStub() = default;

#ifdef GTA_FIVE
	virtual void m_8() = 0;

	virtual void m_10() = 0;

	virtual void m_18() = 0;

	virtual void m_20() = 0;

	virtual void m_28() = 0;

	virtual void LogString(const char* prefix, const char* fmt, ...) = 0;

	virtual void m_38() = 0;

	virtual void m_40() = 0;
#elif IS_RDR3
	virtual void m_8(const char* prefix, uint64_t value) = 0;

	virtual void m_10(const char* prefix, uint64_t value) = 0;

	virtual void m_18(const char* prefix, uint64_t value) = 0;

	virtual void m_20(const char* prefix, uint64_t value) = 0;

	virtual void m_28(const char* prefix, float value) = 0;

	virtual void m_30(const char* prefix, uint64_t value) = 0;

	virtual void m_38(const char* prefix, int64_t value) = 0;

	virtual void m_40(const char* prefix, uint32_t value) = 0;

	virtual void m_48(const char* prefix, int32_t value) = 0;

	virtual void m_50(const char* prefix, bool value) = 0;

	virtual void LogString(const char* prefix, const char* fmt, ...) = 0;
#endif

	// ?
};

enum class UpdateLevel : uint32_t
{
	VERY_LOW = 0,
	LOW = 1,
	MEDIUM = 2,
	HIGH = 3,
	VERY_HIGH = 4,
};

class netObject;
class datBitBuffer;
class netSyncNodeBase
{
public:
	virtual ~netSyncNodeBase() = default;                                                                                                            // 0x00
	virtual bool IsDataNode() { return false; };                                                                                                     // 0x08
	virtual bool IsParentNode() { return false; };                                                                                                   // 0x10
	virtual void MoveCommonDataOpsVFT() {};                                                                                                          // 0x18
	virtual void ClearChildren() {};                                                                                                                 // 0x20
	virtual void _0x28(void*, void*, void*, int* out_count) {};                                                                                      // 0x28
	virtual bool Serialize(int flags, int flags2, void*, datBitBuffer* buffer, int, void*, bool, int*, int* num_serialized) { return false; }  // 0x30
	virtual bool Deserialize(int flags, int flags2, datBitBuffer* buffer, void*) { return false; }                                             // 0x38
	virtual int CalculateSize(int flags, int flags2, void*) { return 0; }                                                                            // 0x40
	virtual int CalculateSize2(int flags, int flags2, bool) { return 0; }                                                                            // 0x48


private:
	template<typename TMember>
	inline static TMember get_member(void* ptr)
	{
		union member_cast
		{
			TMember function;
			struct
			{
				void* ptr;
				uintptr_t off;
			};
		};

		member_cast cast;
		cast.ptr = ptr;
		cast.off = 0;

		return cast.function;
	}

public:
#undef FORWARD_FUNC
#define FORWARD_FUNC(name, offset, ...)           \
		using TFn = decltype(&netSyncNodeBase::name); \
		void** vtbl = *(void***)(this);               \
		return (this->*(get_member<TFn>(vtbl[MapNetSyncNodeMethod(offset) / 8])))(__VA_ARGS__);

#ifdef GTA_FIVE
	inline uint8_t GetUpdateFrequency(UpdateLevel level)
	{
		FORWARD_FUNC(GetUpdateFrequency, 0x68, level);
	}

	inline void WriteObject(netObject* object, datBitBuffer* buffer, netLogStub* logger, bool readFromObject)
	{
		FORWARD_FUNC(WriteObject, 0xB0, object, buffer, logger, readFromObject);
	}

	inline int GetMaximumDataSizeInternal()
	{
		FORWARD_FUNC(GetMaximumDataSizeInternal, 0xB8);
	}

	inline void LogNode(netLogStub* stub)
	{
		FORWARD_FUNC(LogNode, 0xC8, stub);
	}

	inline void LogObject(netObject* object, netLogStub* stub)
	{
		FORWARD_FUNC(LogObject, 0xF0, object, stub);
	}

#undef FORWARD_FUNC

#elif IS_RDR3
	virtual void m_18() = 0; // InitialiseNode
	virtual void m_20() = 0; // ShutdownNode
	virtual void m_28() = 0;
	virtual void m_30() = 0;
	virtual void m_added1311() = 0;
	virtual void m_38() = 0;
	virtual void m_something() = 0; // calls calculatesize, added in a patch
	virtual void m_40() = 0;
	virtual void m_48() = 0;
	virtual void m_50() = 0;
	virtual void m_58() = 0;
	virtual void m_60() = 0;
	virtual void m_68() = 0;
	virtual void m_70() = 0;
	virtual uint8_t GetUpdateFrequency(UpdateLevel level) = 0;
	virtual void m_80() = 0;
	virtual void m_88() = 0;
	virtual void m_90() = 0;
	virtual void m_98() = 0;
	virtual void m_A0() = 0;
	virtual void m_unk1() = 0;
	virtual void m_unk2() = 0;
	virtual void m_unk3() = 0;
	virtual void m_unk4() = 0;
	virtual void m_unk5() = 0;
	virtual void m_unk6() = 0;
	virtual void m_unk7() = 0;
	virtual void m_unk8() = 0;
	virtual void m_B0() = 0;
	virtual void WriteObject(rage::netObject* object, rage::datBitBuffer* buffer, rage::netLogStub* logger, bool readFromObject) = 0;
	virtual void m_D8() = 0;
	virtual void m_E832() = 0;
	virtual void LogNode(rage::netLogStub* stub) = 0;
	virtual void GetUsesCurrentStateBuffer() = 0;
	virtual void m_E328() = 0;
	virtual void m_E321() = 0;
	virtual void LogObject(rage::netObject* object, rage::netLogStub* stub) = 0;
#endif

	// data node

public:
	netSyncNodeBase* nextSibling;

	uint8_t pad[24];

	uint32_t flags1;
	uint32_t flags2;
	uint32_t flags3;

	uint32_t pad2;

	netSyncNodeBase* firstChild;
};

class netSyncDataNodeBase : public netSyncNodeBase
{
public:
	uint32_t flags;
	uint32_t pad3;

#ifdef GTA_FIVE
	uint64_t pad4;
#endif

	netSyncDataNodeBase* externalDependentNodeRoot; // 0x50
	uint32_t externalDependencyCount;
	netSyncDataNodeBase* externalDependencies[8];
	uint8_t syncFrequencies[8];
	void* nodeBuffer;
};
class datBitBuffer
{
public:
	inline datBitBuffer(void* data, size_t size)
	{
		m_data = data;
		m_f8 = 0;
		m_maxBit = size * 8;
		m_unkBit = 0;
		m_curBit = 0;
		m_unk2Bit = 0;
		m_f1C = 0;
	}

	inline uint32_t GetPosition()
	{
		return m_unkBit;
	}

	inline bool Seek(int bits)
	{
		if (bits >= 0)
		{
			uint32_t length = (m_f1C & 1) ? m_maxBit : m_curBit;

			if (bits <= length)
			{
				m_unkBit = bits;
			}
		}

		return false;
	}

	inline size_t GetDataLength()
	{
		char leftoverBit = (m_curBit % 8) ? 1 : 0;

		return (m_curBit / 8) + leftoverBit;
	}

	bool ReadInteger(uint32_t* integer, int bits);

	// NOTE: SIGNED
	bool WriteInteger(uint32_t integer, int bits);

	bool WriteUns(uint32_t integer, int bits);

	bool WriteBit(bool bit);

	bool WriteBits(const void* src, size_t length, size_t srcOffset);

public:
	void* m_data; // +0
	uint32_t m_f8; // +8
	uint32_t m_maxBit; // +12
	uint32_t m_unkBit; // +16
	uint32_t m_curBit; // +20
	uint32_t m_unk2Bit; // +24
	char pad[12]; // +28
	uint8_t m_f1C; // +40
};
#define DECLARE_PEER_ACCESSOR(x) \
	decltype(Impl2372{}.x)& x()        \
	{                       \
		return  ((Impls*)this)->m2372.x;   \
	} \
	

struct netIpAddress
{
	union
	{
		uint32_t addr;
		uint8_t bytes[4];
	};
};

struct netSocketAddress
{
	netIpAddress ip;
	uint16_t port;
};

struct netPeerId
{
	uint64_t val;
};

struct rlGamerHandle
{
	uint8_t handle[16];
};

struct netPeerAddress
{
public:
	struct Impl505
	{
		uint64_t unkKey1;
		uint64_t unkKey2;
		uint32_t secKeyTime; // added in 393
		netSocketAddress relayAddr;
		netSocketAddress publicAddr;
		netSocketAddress localAddr;
		uint32_t newVal; // added in 372
		uint64_t rockstarAccountId; // 463/505
	};

	struct Impl2060
	{
		uint64_t unkKey1;
		uint64_t unkKey2;
		uint32_t secKeyTime;
		netSocketAddress relayAddr;
		netSocketAddress publicAddr;
		netSocketAddress localAddr;
		netSocketAddress unkAddr; // added in 2060
		uint32_t newVal;
		uint64_t rockstarAccountId;
	};

	// unions used here are to retain accessors for external usage
#pragma pack(push, 4)
	struct Impl2372
	{
		netPeerId peerId;

		// gamer handle + pad
		union
		{
			rlGamerHandle gamerHandle;

			// compat
			struct
			{
				uint64_t rockstarAccountId;
				char pad22[8];
			};
		};

		// peer key
		union
		{
			// compat
			struct
			{
				uint64_t unkKey1;
				uint64_t unkKey2;
				char pad3[16];
				uint32_t secKeyTime;
			};

			// real
			struct
			{
				uint8_t peerKey[32];
				uint8_t hasPeerKey;
			};
		};

		netSocketAddress relayAddr;
		netSocketAddress unkAddr;
		netSocketAddress publicAddr;
		netSocketAddress localAddr;

		uint32_t newVal;
	};
#pragma pack(pop)

	union Impls
	{
		Impl505 m1604;
		Impl2060 m2060;
		Impl2372 m2372;
	};

public:
	DECLARE_PEER_ACCESSOR(unkKey1);
	DECLARE_PEER_ACCESSOR(unkKey2);
	DECLARE_PEER_ACCESSOR(secKeyTime);
	DECLARE_PEER_ACCESSOR(relayAddr);
	DECLARE_PEER_ACCESSOR(publicAddr);
	DECLARE_PEER_ACCESSOR(localAddr);
	DECLARE_PEER_ACCESSOR(newVal);
	DECLARE_PEER_ACCESSOR(rockstarAccountId);
};

#define DECLARE_ACCESSOR(x) \
	decltype(impl.m2372.x)& x()        \
	{                       \
		return impl.m2372.x;   \
	} 
struct netPeerAddressStorage : netPeerAddress
{
	uint8_t data[sizeof(Impl2372)];
};


struct rlGamerInfo
{
	netPeerAddressStorage peerAddress;
	uint64_t systemKey;
	uint64_t gamerId;
};


class netPlayer : XBR_VIRTUAL_BASE_2802(0)
{
public:
	//virtual ~netPlayer() = 0;
	// TODO: real dtors
	XBR_VIRTUAL_METHOD(void, Dtor, ())

		XBR_VIRTUAL_METHOD(void, Reset, ())

		XBR_VIRTUAL_METHOD(void, m_10, ())

		XBR_VIRTUAL_METHOD(const char*, GetName, ())

		XBR_VIRTUAL_METHOD(void, m_20, ())

		XBR_VIRTUAL_METHOD(void, m_28, ())

		XBR_VIRTUAL_METHOD(void*, GetGamerInfo_raw, ())

		
	inline auto GetGamerInfo()
	{
		return (rlGamerInfo*)GetGamerInfo_raw();
	}
};

// using XBRVirt is safe here because it's right below so the counter increments right away
class CNetGamePlayer : public netPlayer
{
public:
	XBR_VIRTUAL_METHOD(void, m_38, ())

private:
	template<int ActiveIndexPad, int PlayerInfoPad, int EndPad>
	struct Impl
	{
		uint8_t pad[8];
		void* nonPhysicalPlayerData;
		uint8_t pad2[8 + ActiveIndexPad];
		uint8_t activePlayerIndex; // 1604: +44, 2060: +52, 2372: +32
		uint8_t physicalPlayerIndex;
		uint8_t pad3[2];
		uint8_t pad4[120 + PlayerInfoPad];
		void* playerInfo; // 1604: +148, 2060: +176, 2372: +160
		char end[EndPad];
	};

	union
	{
		Impl<12, 0, 28> m1604;
		Impl<20, 0, 0> m2060;
		Impl<0, 4, 16> m2372;
	} impl;

public:
	void* GetPlayerInfo()
	{
		return  impl.m2372.playerInfo;
	}

public:
	DECLARE_ACCESSOR(nonPhysicalPlayerData);
	DECLARE_ACCESSOR(activePlayerIndex);
	DECLARE_ACCESSOR(physicalPlayerIndex);
	DECLARE_ACCESSOR(playerInfo);
};

class CNetworkPlayerMgr
{
public:
	static CNetGamePlayer* GetPlayer(int playerIndex);
};
template<int Offset>
inline int MapNetObjectMethod()
{
	int offset = Offset;

	if constexpr (Offset >= 0x8)
	{
		offset += 0x30;
	}

	if constexpr (Offset >= 0x78)
	{
		offset += 0x10;
	}

	if constexpr (Offset >= 0x330)
	{
		offset += 0x8;
	}

	return offset;
}
class CNetworkSyncDataULBase
{
public:
	virtual ~CNetworkSyncDataULBase() = default;

	// dummy functions to satisfy compiler
	inline virtual void m_8() { }

	inline virtual void m_10() { }

	inline virtual void m_18() { }

	inline virtual void m_20() { }

	inline virtual void m_28() { }

	inline virtual void m_30() { }

	inline virtual void m_38() { }

	inline virtual void m_40() { }

	inline virtual void m_48() { }

	inline virtual void m_50() { }

	inline virtual void SetCloningFrequency(int player, int frequency) { }

public:
	uint8_t pad_10h[49];
	uint8_t ownerId;
	uint8_t nextOwnerId;
	uint8_t isRemote;
	uint8_t wantsToDelete : 1; // netobj+76
	uint8_t unk1 : 1;
	uint8_t shouldNotBeDeleted : 1;
	uint8_t pad_4Dh[3];
	uint8_t pad_50h[32];
	uint32_t creationAckedPlayers; // netobj+112
	uint32_t m64;
	uint32_t m68;
	uint32_t m6C;

public:
	inline bool IsCreationAckedByPlayer(int index)
	{
		return (creationAckedPlayers & (1 << index)) != 0;
	}
};

class netBlender
{
public:
	virtual ~netBlender() = 0;

	virtual void m_8() = 0;

	virtual void m_10() = 0;

	virtual void m_18() = 0;

	virtual void m_20() = 0;

	virtual void m_28() = 0;

	virtual void m_30() = 0;

	virtual void m_38() = 0;

	virtual void ApplyBlend() = 0;

	virtual void m_48() = 0;

	virtual void m_50() = 0;

	virtual void m_58() = 0;

public:
	void SetTimestamp(uint32_t timestamp);
};

class netSyncTree;
class netObject
{
public:
	virtual ~netObject() = 0;

public:
	uint16_t objectType;
	uint16_t objectId;
	uint32_t pad;

	CNetworkSyncDataULBase syncData;

	inline netBlender* GetBlender()
	{
		return *(netBlender**)((uintptr_t)this + 88);
	}

private:
	template<typename TMember>
	inline static TMember get_member(void* ptr)
	{
		union member_cast
		{
			TMember function;
			struct
			{
				void* ptr;
				uintptr_t off;
			};
		};

		member_cast cast;
		cast.ptr = ptr;
		cast.off = 0;

		return cast.function;
	}

public:
#undef FORWARD_FUNC
#define FORWARD_FUNC(name, offset, ...)     \
	using TFn = decltype(&netObject::name); \
	void** vtbl = *(void***)(this);         \
	return (this->*(get_member<TFn>(vtbl[MapNetObjectMethod<offset>() / 8])))(__VA_ARGS__);

	inline void* GetSyncData()
	{
		FORWARD_FUNC(GetSyncData, 0x20);
	}

	inline netSyncTree* GetSyncTree()
	{
		FORWARD_FUNC(GetSyncTree, 0x30); //0x70
	}

	inline void* GetGameObject()
	{
		FORWARD_FUNC(GetGameObject, 0x80);
	}

	inline void* CreateNetBlender()
	{
		FORWARD_FUNC(CreateNetBlender, 0xB0);
	}

	//D8
	inline void* AllocateNetBlenderParent(int unk)
	{
		FORWARD_FUNC(AllocateNetBlenderParent, 0xD8, unk);
	}
	inline int GetSyncFrequency()
	{
		FORWARD_FUNC(GetSyncFrequency, 0xD0);
	}

	inline void Update()
	{
		FORWARD_FUNC(Update, 0x100);
	}

	inline void StartSynchronising()
	{
		FORWARD_FUNC(StartSynchronising, 0x110);
	}

	inline bool CanSynchronise(bool unk)
	{
		FORWARD_FUNC(CanSynchronise, 0x130, unk);
	}

	inline bool CanPassControl(CNetGamePlayer* player, int type, int* outReason)
	{
		FORWARD_FUNC(CanPassControl, 0x160, player, type, outReason);
	}

	inline bool CanBlend(int* outReason)
	{
		FORWARD_FUNC(CanBlend, 0x170, outReason);
	}

	inline void ChangeOwner(CNetGamePlayer* player, int migrationType)
	{
		FORWARD_FUNC(ChangeOwner, 0x188, player, migrationType);
	}

	inline void OnRegistered()
	{
		FORWARD_FUNC(OnRegistered, 0x190);
	}

	inline void PostMigrate(int migrationType)
	{
		FORWARD_FUNC(PostMigrate, 0x1E0, migrationType);
	}

	inline void PostCreate()
	{
		FORWARD_FUNC(PostCreate, 0x1C8);
	}

	inline void PostSync()
	{
		FORWARD_FUNC(PostSync, 0x1D8);
	}

	inline const char* GetTypeString()
	{
		FORWARD_FUNC(GetTypeString, 0x1F8);
	}

	inline void UpdatePendingVisibilityChanges()
	{
		FORWARD_FUNC(UpdatePendingVisibilityChanges, 0x330);
	}

#undef FORWARD_FUNC

	inline uint16_t GetObjectId()
	{
		return objectId;
	}

	inline uint16_t GetObjectType()
	{
		return objectType;
	}

	inline std::string ToString()
	{
		std::stringstream ss;
		ss << "[netObj:" << objectId << ":" << GetTypeString() << "]";
		return ss.str();
	}
};
inline int MapNetSyncTreeMethod(int offset)
{
	if (offset >= 0xD0)
	{
		offset += 0x10;
	}

	return offset;
}
class netSyncTree
{
public:
	virtual ~netSyncTree() = 0;

private:
	template<typename TMember>
	inline static TMember get_member(void* ptr)
	{
		union member_cast
		{
			TMember function;
			struct
			{
				void* ptr;
				uintptr_t off;
			};
		};

		member_cast cast;
		cast.ptr = ptr;
		cast.off = 0;

		return cast.function;
	}

public:
#undef FORWARD_FUNC
#define FORWARD_FUNC(name, offset, ...)     \
	using TFn = decltype(&netSyncTree::name); \
	void** vtbl = *(void***)(this);         \
	return (this->*(get_member<TFn>(vtbl[MapNetSyncTreeMethod(offset) / 8])))(__VA_ARGS__);

	inline void WriteTree(int flags, int objFlags, netObject* object, datBitBuffer* buffer, uint32_t time, void* logger, uint8_t targetPlayer, void* outNull)
	{
		FORWARD_FUNC(WriteTree, 0x8, flags, objFlags, object, buffer, time, logger, targetPlayer, outNull);
	}
	inline void* Test()
	{
		void** vtbl = *(void***)(this);         
		return vtbl[MapNetSyncTreeMethod(0x10) / 8];
	}
	inline int TestOffset()
	{
		void** vtbl = *(void***)(this);
		return MapNetSyncTreeMethod(0x10);
	}
	inline void ApplyToObject(netObject* object, void* unk)
	{
		FORWARD_FUNC(ApplyToObject, 0x10, object, unk);
	}

	inline void* GetCreationDataNode()
	{
		FORWARD_FUNC(GetCreationDataNode, 0x50);
	}

	inline bool m_D0(int unk)
	{
		FORWARD_FUNC(m_D0, 0xD0, unk);
	}

#undef FORWARD_FUNC

public:
	bool CanApplyToObject(netObject* object);

	bool ReadFromBuffer(int flags, int flags2, datBitBuffer* buffer, void* netLogStub);

	bool WriteTreeCfx(int flags, int objFlags, netObject* object, datBitBuffer* buffer, uint32_t time, void* logger, uint8_t targetPlayer, void* outNull, uint32_t* lastChangeTime);

	void AckCfx(netObject* object, uint32_t timestamp);

	
public:
	static netSyncTree* GetForType(NetObjEntityType type);

private:
	char pad[8]; // +8

public:
	netSyncNodeBase* syncNode; // +16
};
class netObjectMgr
{
public:
	virtual ~netObjectMgr() = 0;

	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Update() = 0;

	virtual void AddEntity() = 0;
	virtual void m_28() = 0;
	virtual void UnregisterNetworkObject(netObject* object, int reason, bool force1, bool force2) = 0;
	virtual void ChangeOwner(netObject* object, CNetGamePlayer* player, int migrationType) = 0;
	virtual void m_40() = 0;
	virtual void m_48() = 0;
	virtual void m_50() = 0;
	virtual void m_58() = 0;
	virtual void RegisterNetworkObject(netObject* entity) = 0;

private:
	struct atDNetObjectNode
	{
		virtual ~atDNetObjectNode();

		netObject* object;
		atDNetObjectNode* next;
	};

	struct ObjectHolder
	{
		atDNetObjectNode* objects;
		netObject** unk; // might not just be a netObject**
	};

private:
	ObjectHolder m_objects[32];

public:
	template<typename T>
	inline void ForAllNetObjects(int playerId, const T& callback)
	{
		for (auto node = m_objects[playerId].objects; node; node = node->next)
		{
			if (node->object)
			{
				callback(node->object);
			}
		}
	}

	netObject* GetNetworkObject(uint16_t id, bool a3);

	static netObjectMgr* GetInstance();
};
class netPlayerMgrBase
{
public:
	char pad[232 - 8];
	CNetGamePlayer* localPlayer;

public:
	virtual ~netPlayerMgrBase() = 0;

	virtual void Initialize() = 0;

	virtual void Shutdown() = 0;

	virtual void m_18() = 0;

#ifdef GTA_FIVE
private:
	virtual CNetGamePlayer* AddPlayer_raw(void* scInAddr, void* unkNetValue, void* addedIn1290, void* playerData, void* nonPhysicalPlayerData) = 0;

public:
#elif IS_RDR3
	virtual CNetGamePlayer* AddPlayer(void* scInAddr, uint32_t activePlayerIndex, void* playerData, void* playerAccountId) = 0;
#endif

	virtual void RemovePlayer(CNetGamePlayer* player) = 0;

	void UpdatePlayerListsForPlayer(CNetGamePlayer* player);

#ifdef GTA_FIVE
public:
	CNetGamePlayer* AddPlayer(void* scInAddr, void* unkNetValue, void* addedIn1290, void* playerData, void* nonPhysicalPlayerData)
	{
		//if (xbr::IsGameBuildOrGreater<2372>())
		{
			static auto addPlayerFunc = *(uint64_t*)(*(char**)this + 0x20);
			return ((CNetGamePlayer * (*)(void*, void*, void*, void*, void*))(addPlayerFunc))(this, scInAddr, unkNetValue, playerData, nonPhysicalPlayerData);
		}

		return AddPlayer_raw(scInAddr, unkNetValue, addedIn1290, playerData, nonPhysicalPlayerData);
	}
#endif
};
class netInterface_queryFunctions
{
public:
	virtual ~netInterface_queryFunctions() = 0;

	virtual void m_8() = 0;

	virtual uint32_t GetTimestamp() = 0;

public:
	static netInterface_queryFunctions* GetInstance();
};
enum eThreadState {
	ThreadStateIdle,
	ThreadStateRunning,
	ThreadStateKilled,
	ThreadState3,
	ThreadState4,
};

class scrThreadContext {
public:

	uint32_t					m_iThreadId; 		//0x0000 
	uint32_t					m_iScriptHash; 		//0x0004 
	eThreadState				m_State; 			//0x0008 
	uint32_t					m_iIP; 				//0x000C 
	uint32_t					m_iFrameSP; 		//0x0010 
	uint32_t					m_iSP; 				//0x0014 
	uint32_t					m_iTimerA; 			//0x0018 
	uint32_t					m_iTimerB; 			//0x001C 
	uint32_t					m_iTimerC; 			//0x0020 
	uint32_t					m_iUnk1; 			//0x0024 
	uint32_t					m_iUnk2;			//0x0028 
	char _0x002C[52];
	uint32_t					m_iSet1;			//0x0060 
	char _0x0064[68];
};

class scrThread {
protected:

	scrThreadContext			m_Context;			//0x0008
	__int64						m_pStack;			//0x00B0
	char _0x00B8[16];
	char* m_pszExitMessage;	//0x00C8

public:

	virtual ~scrThread() {}
	virtual eThreadState		Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount) = 0;
	virtual eThreadState		Run(uint32_t opsToExecute) = 0;
	virtual eThreadState		Tick(uint32_t opsToExecute) = 0;
	virtual void				Kill() = 0;

	inline scrThreadContext* GetContext() { return &m_Context; }
	inline uint32_t				GetId() { return m_Context.m_iThreadId; }
};
class scriptHandlerMgr
{
public:
	virtual inline ~scriptHandlerMgr() {}

	virtual void m1() = 0;

	virtual void m2() = 0;

	virtual void m3() = 0;

	virtual void m4() = 0;

	virtual void m5() = 0;

	virtual void m6() = 0;

	virtual void m7() = 0;

	virtual void m8() = 0;

	virtual void m9() = 0;

	virtual void AttachScript(scrThread* thread) = 0;

	virtual void DetachScript(scrThread* thread) = 0;
};

class ScriptThread : public scrThread {
private:
	inline uint32_t hashGetMata(const char* str)
	{
		size_t len = strlen(str);
		size_t i = 0;
		uint32_t hash = 0;
		while (i != len) {
			hash += (char)tolower(str[i++]);
			hash += hash << 10;
			hash ^= hash >> 6;
		}
		hash += hash << 3;
		hash ^= hash >> 11;
		hash += hash << 15;
		return hash;
	}
	char scriptName[64];
	void* m_pScriptHandler;	//0x0110 
	char _0x0118[40];
	uint8_t						m_bFlag1;			//0x0140 
	uint8_t						m_bNetworkFlag;		//0x0141 
	char _0x0142[22];

public:

	virtual void				DoRun() = 0;

	virtual eThreadState		Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount);
	virtual eThreadState		Run(uint32_t opsToExecute);
	virtual eThreadState		Tick(uint32_t opsToExecute);
	virtual void				Kill();

	inline void* GetScriptHandler() { return m_pScriptHandler; }
	inline void SetScriptName(const char* name)
	{

		auto scriptHashPtr = reinterpret_cast<uint32_t*>((uint64_t)this + 0xD0);
		auto scriptNamePtr = reinterpret_cast<char*>((uint64_t)this + 0xD4);

		strncpy(scriptNamePtr, name, sizeof(scriptName) - 1);
		scriptNamePtr[sizeof(scriptName) - 1] = '\0';

		*scriptHashPtr = hashGetMata(name);

	}

	inline void SetNetworkFlag(char state)
	{
		bool* thisNetFlag = (bool*)((uintptr_t)this + 0x149); // See GtaThreadInit function in GtaThread::GtaThread() (and extrapolate)
		*thisNetFlag = state;

	}
};