#include <iostream>
#include "Hooking.h"
#include <MinHook.h>
#include "nativeList.h"
#include "atPool.h"
#include "CPoolManagement.h"
#include <map>
#include <sstream>
#include "jooat.h"
#include <spdlog/spdlog.h>

const int g_netOverlayOffsetX = -30;
const int g_netOverlayOffsetY = -60;
const int g_netOverlayWidth = 400;
const int g_netOverlayHeight = 320;

const int g_netOverlaySampleSize = 200; // milliseconds per sample frame
const int g_netOverlaySampleCount = 150;



class RageHashList
{
public:
	template<int Size>
	RageHashList(const char* (&list)[Size])
	{
		for (int i = 0; i < Size; i++)
		{
			m_lookupList.insert({ hashGet(list[i]), list[i] });
		}
	}

	inline std::string LookupHash(uint32_t hash)
	{
		auto it = m_lookupList.find(hash);

		if (it != m_lookupList.end())
		{
			return std::string(it->second);
		}
		std::stringstream ss;
		ss << std::hex << hash;
		return ss.str();
	}

private:
	std::map<uint32_t, std::string_view> m_lookupList;
};


static const char* poolEntriesTable[] = {
	"AnimatedBuilding",
	"AttachmentExtension",
	"AudioHeap",
	"BlendshapeStore",
	"Building",
	"carrec",
	"CBoatChaseDirector",
	"CVehicleCombatAvoidanceArea",
	"CCargen",
	"CCargenForScenarios",
	"CCombatDirector",
	"CCombatInfo",
	"CCombatSituation",
	"CCoverFinder",
	"CDefaultCrimeInfo",
	"CTacticalAnalysis",
	"CTaskUseScenarioEntityExtension",
	"AnimStore",
	"CGameScriptResource",
	"ClothStore",
	"CombatMeleeManager_Groups",
	"CombatMountedManager_Attacks",
	"CompEntity",
	"CPrioritizedClipSetBucket",
	"CPrioritizedClipSetRequest",
	"CRoadBlock",
	"CStuntJump",
	"CScenarioInfo",
	"CScenarioPointExtraData",
	"CutsceneStore",
	"CScriptEntityExtension",
	"CVehicleChaseDirector",
	"CVehicleClipRequestHelper",
	"CPathNodeRouteSearchHelper",
	"CGrabHelper",
	"CGpsNumNodesStored",
	"CClimbHandHoldDetected",
	"CAmbientLookAt",
	"DecoratorExtension",
	"DrawableStore",
	"Dummy Object",
	"DwdStore",
	"EntityBatch",
	"GrassBatch",
	"ExprDictStore",
	"FrameFilterStore",
	"FragmentStore",
	"GamePlayerBroadcastDataHandler_Remote",
	"InstanceBuffer",
	"InteriorInst",
	"InteriorProxy",
	"IplStore",
	"MaxLoadedInfo",
	"MaxLoadRequestedInfo",
	"ActiveLoadedInfo",
	"ActivePersistentLoadedInfo",
	"Known Refs",
	"LightEntity",
	"MapDataLoadedNode",
	"MapDataStore",
	"MapTypesStore",
	"MetaDataStore",
	"NavMeshes",
	"NetworkDefStore",
	"NetworkCrewDataMgr",
	"Object",
	"OcclusionInteriorInfo",
	"OcclusionPathNode",
	"OcclusionPortalEntity",
	"OcclusionPortalInfo",
	"Peds",
	"CWeapon",
	"phInstGta",
	"PhysicsBounds",
	"CPickup",
	"CPickupPlacement",
	"CPickupPlacementCustomScriptData",
	"CRegenerationInfo",
	"PortalInst",
	"PoseMatcherStore",
	"PMStore",
	"PtFxSortedEntity",
	"PtFxAssetStore",
	"QuadTreeNodes",
	"ScaleformStore",
	"ScaleformMgrArray",
	"ScriptStore",
	"StaticBounds",
	"tcBox",
	"TrafficLightInfos",
	"TxdStore",
	"Vehicles",
	"VehicleStreamRequest",
	"VehicleStreamRender",
	"VehicleStruct",
	"HandlingData",
	"wptrec",
	"fwLodNode",
	"CTask",
	"CEvent",
	"CMoveObject",
	"CMoveAnimatedBuilding",
	"atDScriptObjectNode",
	"fwDynamicArchetypeComponent",
	"fwDynamicEntityComponent",
	"fwEntityContainer",
	"fwMatrixTransform",
	"fwQuaternionTransform",
	"fwSimpleTransform",
	"ScenarioCarGensPerRegion",
	"ScenarioPointsAndEdgesPerRegion",
	"ScenarioPoint",
	"ScenarioPointEntity",
	"ScenarioPointWorld",
	"MaxNonRegionScenarioPointSpatialObjects",
	"ObjectIntelligence",
	"VehicleScenarioAttractors",
	"AircraftFlames",
	"CScenarioPointChainUseInfo",
	"CScenarioClusterSpawnedTrackingData",
	"CSPClusterFSMWrapper",
	"fwArchetypePooledMap",
	"CTaskConversationHelper",
	"SyncedScenes",
	"AnimScenes",
	"CPropManagementHelper",
	"ActionTable_Definitions",
	"ActionTable_Results",
	"ActionTable_Impulses",
	"ActionTable_Interrelations",
	"ActionTable_Homings",
	"ActionTable_Damages",
	"ActionTable_StrikeBones",
	"ActionTable_Rumbles",
	"ActionTable_Branches",
	"ActionTable_StealthKills",
	"ActionTable_Vfx",
	"ActionTable_FacialAnimSets",
	"NetworkEntityAreas",
	"NavMeshRoute",
	"CScriptEntityExtension",
	"AnimStore",
	"CutSceneStore",
	"OcclusionPathNode",
	"OcclusionPortalInfo",
	"CTask",
	"OcclusionPathNode",
	"OcclusionPortalInfo",
#include "gta_vtables.h"
	"Decorator",
};
static RageHashList poolEntries(poolEntriesTable);

__int64 ThisRegistersMaxLoadedInfoLoopSeemsBug() {
	return 0;
}
struct PoolRegistrationData {
	void* allocated_tls;
	std::string name;
	std::uint32_t hash;
	bool finished = false;
};
inline static std::map<std::uintptr_t, PoolRegistrationData> LinkBetweenTlsAllocationAndRegisterPool;

inline static void* (*oSetPoolToTls)(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6);
void* __fastcall SetPoolToTls(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6){
	auto result = oSetPoolToTls(a1, a2, a3, a4, a5, a6);
	auto size = (std::uintptr_t)a2;

	auto callerHash = (std::uintptr_t(_AddressOfReturnAddress()) * size);

	if (LinkBetweenTlsAllocationAndRegisterPool.find(callerHash) != LinkBetweenTlsAllocationAndRegisterPool.end()) {
		PoolRegistrationData& data = LinkBetweenTlsAllocationAndRegisterPool[callerHash];
		if (!data.finished) {
			data.finished = true;
			data.allocated_tls = a1;

			spdlog::info("(SetPoolToTls)Finished registering pool name:<{}>, hash: <{}>, allocated_tls: <{}>", data.name, data.hash, data.allocated_tls);
		}
	}
	
	return result;
}
inline static void* (*oSetPoolToTlsAlias_0)(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6);
void* __fastcall SetPoolToTlsAlias_0(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6)
{
	auto result = oSetPoolToTlsAlias_0(a1,a2,a3,a4,a5,a6);
	auto size = (std::uintptr_t)a2;

	auto callerHash = (std::uintptr_t(_AddressOfReturnAddress()) * size);

	if (LinkBetweenTlsAllocationAndRegisterPool.find(callerHash) != LinkBetweenTlsAllocationAndRegisterPool.end()) {
		PoolRegistrationData& data = LinkBetweenTlsAllocationAndRegisterPool[callerHash];
		if (!data.finished) {
			data.finished = true;
			data.allocated_tls = a1;
			CPoolManagement::Pools[data.hash] = (atPoolBase*)data.allocated_tls;
			spdlog::info("(set_pool_to_tls_alias) Finished registering pool name:<{}>, hash: <{}>, allocated_tls: <{}>", data.name, data.hash, data.allocated_tls);
		}
	}
	return result;
}
void CPoolManagement::Hook() {
	
	o_poolRealease = (void(__cdecl*)(atPoolBase*, void*))(hook::get_call(hook::get_pattern("48 8B D3 E8 ? ? ? ? 0F 28 45 E0 66 0F 7F", 3)));
	
	auto registerPoolCall = hook::get_pattern("E8 ? ? ? ? C6 44 24 ? ? 4C 8D 0D ? ? ? ? F3 0F 10 15 ? ? ? ? 48 8B CB 8B D0 C7 44 24 ? ? ? ? ? 89 7C 24 20 E8 ? ? ? ? 48 89 1D ? ? ? ? EB 08");

	auto registerPool = hook::get_call(registerPoolCall);

	if (MH_CreateHook(registerPool, &RegisterPoolHook, reinterpret_cast<void**>(&g_oRegisterPool)) != MH_OK) {
		std::cout << "Failed hooking RegisterPool" << std::endl;
	}
	
	auto PoolAllocateCaller = hook::get_pattern("E8 ? ? ? ? 38 1D ? ? ? ? 48 8B F8 74 0C 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 85 FF 74 18");
	auto PoolAllocator = hook::get_call(PoolAllocateCaller);

	MH_CreateHook(PoolAllocator, CPoolManagement::PoolAllocateInternal, (void**)&g_origPoolAllocate);

	if (MH_EnableHook(PoolAllocator) != MH_OK)
		MessageBox(0, "Failed PoolAllocator. ", "PoolAllocator", MB_ICONERROR);

	if (MH_EnableHook(registerPool) != MH_OK)
		MessageBox(0, "Failed registerPool. ", "registerPool", MB_ICONERROR);

	auto bugged_function_which_calls_register_pool = hook::pattern("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 30 80 3D ? ? ? ? ? 48 8B 0D ? ? ? ? 8B 2D ? ? ? ? 0F 29 74 24 ? F3 0F 10 35 ? ? ? ?").count(1).get(0).get<void>();
	if (MH_CreateHook(bugged_function_which_calls_register_pool, &ThisRegistersMaxLoadedInfoLoopSeemsBug, nullptr) != MH_OK) {
		std::cout << "Failed hooking bugged_function_which_calls_register_pool" << std::endl;
	}
	if (MH_EnableHook(bugged_function_which_calls_register_pool) != MH_OK)
		MessageBox(0, "Failed bugged_function_which_calls_register_pool. ", "bugged_function_which_calls_register_pool", MB_ICONERROR);


	auto set_pool_to_tls_caller = hook::get_pattern("E8 ? ? ? ? 48 89 1D ? ? ? ? EB 08 48 83 25 ? ? ? ? ? 48 8B 5C 24 ?");
	auto set_pool_to_tls_call = hook::get_call(set_pool_to_tls_caller);

	if (MH_CreateHook(set_pool_to_tls_call, &SetPoolToTls, (void**)&oSetPoolToTls) != MH_OK) {
		std::cout << "Failed hooking set_pool_to_tls_call" << std::endl;
	}

	auto set_pool_to_tls_alias = hook::pattern("40 53 48 83 EC 20 8B 44 24 50 48 83 21 00 48 83 61 ? ? 89 51 10 8A 54 24 58 89 41 14 48 8B D9 C7 41 ? ? ? ? ? E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B").count(1).get(0).get<void>();
	if (MH_CreateHook(set_pool_to_tls_alias, &SetPoolToTlsAlias_0, (void**)&oSetPoolToTlsAlias_0) != MH_OK) {
		std::cout << "Failed hooking SetPoolToTlsAlias_0" << std::endl;
	}

	if (MH_EnableHook(set_pool_to_tls_alias) != MH_OK)
		MessageBox(0, "Failed set_pool_to_tls_alias. ", "set_pool_to_tls_alias", MB_ICONERROR);
}
void CPoolManagement::Unhook() {

}
//

void CPoolManagement::OnGameHook() {

}

void CPoolManagement::OnGameFrame() {

}
std::uint64_t __fastcall  CPoolManagement::RegisterPoolHook(void* allist, unsigned int hash, int a3) {
	/*if (Pools.find(hash) == Pools.end()) {
		std::string poolName = poolEntries.LookupHash(hash);
		

		auto base = g_oRegisterPool(allist, hash, a3);
		Pools[hash] = base;
		InversePools.insert({ base, hash });

		spdlog::info("Registered pool name:<{}>, hash: <{}>, base: <{}>", poolName, hash, (std::uintptr_t)base);
	}*/
	auto size = reinterpret_cast<std::uint64_t(__fastcall*)(void* allist, unsigned int hash, int a3)>(g_oRegisterPool)(allist, hash, a3);
	std::string poolName = poolEntries.LookupHash(hash);
	
	auto callerHash = (std::uintptr_t(_AddressOfReturnAddress()) * size);

	if (LinkBetweenTlsAllocationAndRegisterPool.find(callerHash) == LinkBetweenTlsAllocationAndRegisterPool.end()) {
		std::string poolName = poolEntries.LookupHash(hash);
		LinkBetweenTlsAllocationAndRegisterPool[callerHash] = PoolRegistrationData{nullptr, poolName, hash};
		spdlog::info("Started registering pool name:<{}>, hash: <{}>", poolName, hash);
	}
	return size;
}

void* CPoolManagement::PoolAllocate(atPoolBase* base) {

	static auto PoolAllocateCaller = hook::get_pattern("E8 ? ? ? ? 38 1D ? ? ? ? 48 8B F8 74 0C 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 85 FF 74 18");
	static auto PoolAllocatorCall = hook::get_call(PoolAllocateCaller);


	return reinterpret_cast<void*(__fastcall*)(atPoolBase*)>(PoolAllocatorCall)(base);
}
void* __fastcall CPoolManagement::PoolAllocateInternal(atPoolBase* pool)
{

		// aici da crash
		void* value = g_origPoolAllocate(pool);

		if (!value)
		{
			auto it = InversePools.find(pool);
			std::string poolName = "<<unknown pool>>";

			if (it != InversePools.end())
			{
				uint32_t poolHash = it->second;

				poolName = poolEntries.LookupHash(poolHash);
			}

			// non-fatal pools
			if (poolName == "AircraftFlames")
			{
				return nullptr;
			}

			std::stringstream ss;
			ss << poolName << " Pool Full, Size == " << pool->GetSize() << std::endl;
			MessageBox(0, ss.str().c_str(), "Error!", MB_OK);


		}
	return value;
}

void CPoolManagement::PoolRelease(atPoolBase* pool, void* a1)
{
	o_poolRealease(pool, a1);
}


atPoolBase* CPoolManagement::GetPoolBase(uint32_t hash) {
	auto it = Pools.find(hash);

	if (it == Pools.end())
	{
		return nullptr;
	}

	return it->second;
}

CPoolManagement* g_PoolManagement = new CPoolManagement();