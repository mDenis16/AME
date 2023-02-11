#include "CGameEvents.h"
#include <MinHook.h>
#include "Hooking.h"
#include "XBRVirtual.h"

void CGameEvents::Hook() {

}void CGameEvents::Unhook() {

}
static const char* g_eventNames2612[251]
{
	"UNUSED_0",
	"UNUSED_1",
	"UNUSED_2",
	"UNUSED_3",
	"UNUSED_4",
	"UNUSED_5",
	"CEventAgitated",
	"CEventAgitatedAction",
	"CEventEncroachingPed",
	"CEventCallForCover",
	"CEventCarUndriveable",
	"CEventClimbLadderOnRoute",
	"CEventClimbNavMeshOnRoute",
	"CEventAcquaintancePedDead",
	"CEventCommunicateEvent",
	"CEventCopCarBeingStolen",
	"CEventCrimeReported",
	"CEventDamage",
	"CEventDeadPedFound",
	"CEventDeath",
	"CEventDraggedOutCar",
	"UNUSED_21",
	"CEventAcquaintancePedLike",
	"CEventExplosionHeard",
	"CEventAcquaintancePedDislike",
	"UNUSED_25",
	"CEventFootStepHeard",
	"CEventGetOutOfWater",
	"CEventGivePedTask",
	"CEventGunAimedAt",
	"CEventHelpAmbientFriend",
	"CEventInjuredCryForHelp",
	"CEventCrimeCryForHelp",
	"CEventInAir",
	"UNUSED_34",
	"UNUSED_35",
	"CEventAcquaintancePedWanted",
	"UNUSED_37",
	"CEventLeaderExitedCarAsDriver",
	"CEventLeaderHolsteredWeapon",
	"CEventLeaderLeftCover",
	"CEventLeaderUnholsteredWeapon",
	"CEventMeleeAction",
	"CEventMustLeaveBoat",
	"CEventNewTask",
	"UNUSED_45",
	"CEventObjectCollision",
	"CEventOnFire",
	"CEventOpenDoor",
	"CEventShovePed",
	"CEventPedCollisionWithPed",
	"CEventPedCollisionWithPlayer",
	"CEventPedEnteredMyVehicle",
	"CEventPedJackingMyVehicle",
	"CEventPedOnCarRoof",
	"UNUSED_55",
	"UNUSED_56",
	"CEventPlayerCollisionWithPed",
	"UNUSED_58",
	"CEventPotentialBeWalkedInto",
	"CEventPotentialBlast",
	"CEventPotentialGetRunOver",
	"UNUSED_62",
	"UNUSED_63",
	"CEventPotentialWalkIntoVehicle",
	"CEventProvidingCover",
	"UNUSED_66",
	"CEventRanOverPed",
	"UNUSED_68",
	"CEventReactionEnemyPed",
	"CEventReactionInvestigateDeadPed",
	"CEventReactionInvestigateThreat",
	"CEventRequestHelpWithConfrontation",
	"CEventRespondedToThreat",
	"UNUSED_74",
	"CEventScriptCommand",
	"CEventShockingBrokenGlass",
	"CEventShockingCarAlarm",
	"CEventShockingCarChase",
	"CEventShockingCarCrash",
	"CEventShockingBicycleCrash",
	"CEventShockingCarPileUp",
	"CEventShockingCarOnCar",
	"CEventShockingDangerousAnimal",
	"CEventShockingDeadBody",
	"CEventShockingDrivingOnPavement",
	"CEventShockingBicycleOnPavement",
	"CEventShockingEngineRevved",
	"CEventShockingExplosion",
	"CEventShockingFire",
	"CEventShockingGunFight",
	"CEventShockingGunshotFired",
	"CEventShockingHelicopterOverhead",
	"CEventShockingParachuterOverhead",
	"CEventShockingPedKnockedIntoByPlayer",
	"CEventShockingHornSounded",
	"CEventShockingInDangerousVehicle",
	"CEventShockingInjuredPed",
	"CEventShockingMadDriver",
	"CEventShockingMadDriverExtreme",
	"CEventShockingMadDriverBicycle",
	"CEventShockingMugging",
	"CEventShockingNonViolentWeaponAimedAt",
	"CEventShockingPedRunOver",
	"CEventShockingPedShot",
	"CEventShockingPlaneFlyby",
	"CEventShockingPotentialBlast",
	"CEventShockingPropertyDamage",
	"CEventShockingRunningPed",
	"CEventShockingRunningStampede",
	"CEventShockingSeenCarStolen",
	"CEventShockingSeenConfrontation",
	"CEventShockingSeenGangFight",
	"CEventShockingSeenInsult",
	"CEventShockingSeenMeleeAction",
	"CEventShockingSeenNiceCar",
	"CEventShockingSeenPedKilled",
	"CEventShockingVehicleTowed",
	"CEventShockingWeaponThreat",
	"CEventShockingWeirdPed",
	"CEventShockingWeirdPedApproaching",
	"CEventShockingSiren",
	"CEventShockingStudioBomb",
	"CEventShockingVisibleWeapon",
	"CEventGunShot",
	"CEventGunShotBulletImpact",
	"CEventGunShotWhizzedBy",
	"CEventFriendlyAimedAt",
	"CEventFriendlyFireNearMiss",
	"CEventShoutBlockingLos",
	"CEventShoutTargetPosition",
	"CEventStaticCountReachedMax",
	"CEventStuckInAir",
	"CEventSuspiciousActivity",
	"CEventSwitch2NM",
	"CEventUnidentifiedPed",
	"CEventVehicleCollision",
	"CEventVehicleDamageWeapon",
	"CEventVehicleOnFire",
	"UNUSED_139",
	"CEventDisturbance",
	"CEventEntityDamaged",
	"CEventEntityDestroyed",
	"CEventWrithe",
	"CEventHurtTransition",
	"CEventPlayerUnableToEnterVehicle",
	"CEventScenarioForceAction",
	"CEventStatChangedValue",
	"CEventPlayerDeath",
	"CEventPedSeenDeadPed",
	"UNUSED_150",
	"UNUSED_151",
	"UNUSED_152",
	"CEventNetworkPlayerJoinScript",
	"CEventNetworkPlayerLeftScript",
	"CEventNetworkStorePlayerLeft",
	"CEventNetworkStartSession",
	"CEventNetworkEndSession",
	"CEventNetworkStartMatch",
	"UNUSED_159",
	"CEventNetworkRemovedFromSessionDueToStall",
	"CEventNetworkRemovedFromSessionDueToComplaints",
	"CEventNetworkConnectionTimeout",
	"CEventNetworkPedDroppedWeapon",
	"CEventNetworkPlayerSpawn",
	"CEventNetworkPlayerCollectedPickup",
	"CEventNetworkPlayerCollectedAmbientPickup",
	"CEventNetworkPlayerCollectedPortablePickup",
	"CEventNetworkPlayerDroppedPortablePickup",
	"UNUSED_169",
	"CEventNetworkInviteAccepted",
	"CEventNetworkInviteConfirmed",
	"CEventNetworkInviteRejected",
	"CEventNetworkSummon",
	"CEventNetworkScriptEvent",
	"UNUSED_175",
	"CEventNetworkSignInStateChanged",
	"CEventNetworkSignInChangeActioned",
	"CEventNetworkRosChanged",
	"CEventNetworkBail",
	"CEventNetworkHostMigration",
	"CEventNetworkFindSession",
	"CEventNetworkHostSession",
	"CEventNetworkJoinSession",
	"CEventNetworkJoinSessionResponse",
	"CEventNetworkCheatTriggered",
	"CEventNetworkEntityDamage",
	"CEventNetworkPlayerArrest",
	"CEventNetworkTimedExplosion",
	"CEventNetworkPrimaryClanChanged",
	"CEventNetworkClanJoined",
	"CEventNetworkClanLeft",
	"CEventNetworkClanInviteReceived",
	"CEventNetworkVoiceSessionStarted",
	"CEventNetworkVoiceSessionEnded",
	"CEventNetworkVoiceConnectionRequested",
	"CEventNetworkVoiceConnectionResponse",
	"CEventNetworkVoiceConnectionTerminated",
	"CEventNetworkTextMessageReceived",
	"CEventNetworkCloudFileResponse",
	"CEventNetworkPickupRespawned",
	"CEventNetworkPresence_StatUpdate",
	"CEventNetworkPedLeftBehind",
	"CEventNetwork_InboxMsgReceived",
	"CEventNetworkAttemptHostMigration",
	"CEventNetworkIncrementStat",
	"CEventNetworkSessionEvent",
	"CEventNetworkTransitionStarted",
	"CEventNetworkTransitionEvent",
	"CEventNetworkTransitionMemberJoined",
	"CEventNetworkTransitionMemberLeft",
	"CEventNetworkTransitionParameterChanged",
	"CEventNetworkClanKicked",
	"CEventNetworkTransitionStringChanged",
	"CEventNetworkTransitionGamerInstruction",
	"CEventNetworkPresenceInvite",
	"CEventNetworkPresenceInviteRemoved",
	"CEventNetworkPresenceInviteReply",
	"CEventNetworkCashTransactionLog",
	"CEventNetworkClanRankChanged",
	"CEventNetworkVehicleUndrivable",
	"CEventNetworkPresenceTriggerEvent",
	"CEventNetworkEmailReceivedEvent",
	"CEventNetworkFollowInviteReceived",
	"UNUSED_224",
	"CEventNetworkSpectateLocal",
	"CEventNetworkCloudEvent",
	"CEventNetworkShopTransaction",
	"UNUSED_228",
	"UNUSED_229",
	"CEventNetworkOnlinePermissionsUpdated",
	"CEventNetworkSystemServiceEvent",
	"CEventNetworkRequestDelay",
	"CEventNetworkSocialClubAccountLinked",
	"CEventNetworkScAdminPlayerUpdated",
	"CEventNetworkScAdminReceivedCash",
	"CEventNetworkClanInviteRequestReceived",
	"CEventNetworkMarketingEmailReceivedEvent",
	"CEventNetworkStuntPerformed",
	"CEventNetworkFiredDummyProjectile",
	"CEventNetworkPlayerEnteredVehicle",
	"CEventNetworkPlayerActivatedSpecialAbility",
	"CEventNetworkPlayerDeactivatedSpecialAbility",
	"CEventNetworkPlayerSpecialAbilityFailedActivation",
	"CEventNetworkFiredVehicleProjectile",
	"UNUSED_245",
	"UNUSED_246",
	"UNUSED_247",
	"CEventErrorUnknownError",
	"CEventErrorArrayOverflow",
	"CEventErrorInstructionLimit",
};


inline void* get_virtual(void* _class, unsigned int index) { return reinterpret_cast<void*>((*reinterpret_cast<int**>(_class))[index]); }
template <typename FuncType>
__forceinline static FuncType call_virtual(void* ppClass, int index) {
	int* pVTable = *(int**)ppClass; //-V206
	int dwAddress = pVTable[index]; //-V108
	return (FuncType)(dwAddress);
}
// Created with ReClass.NET 1.2 by KN4CK3R

class FWeVENT
{
public:
	char pad_0000[56]; //0x0000
	float N000000B5; //0x0038
	char pad_003C[4]; //0x003C
	float x;
	float y;
	float z;
}; //Size: 0x0084

class fwEvent 
{
public:

	virtual int32_t Function0();
	virtual int32_t Function1();
	virtual int32_t Function2();
	virtual int32_t Function3();
	virtual int32_t Function4();
	virtual int32_t Function5();



};

class fwEventGroup
{
public:
	size_t unk; // somehow always ended up to be 0x0000000000000000

	// it looked like it was filling 30-ish 64 bit values.
	// until now I got a total of 13 events at one time.
	fwEvent* events[30];

	uint32_t unk2;
	uint32_t unk3; // it's like this one is used to store how much of these event wrappers are processed.
	uint32_t eventsCount; // Stores the amount of events, there's an exception case for a count of 32.
	uint32_t unk4;

	size_t unk5;

	virtual ~fwEventGroup() = 0;
};
static const char* GetEventName(fwEvent* ev)
{

	//MessageBox(0, "GetEventName", "Please attach debugger", MB_OK);
//	int eventId = ev->Function4();

	int test1 = ev->Function1();
	int test2 = ev->Function2();
	int eventId = ev->Function3();
	int test4 = ev->Function4();
	int test5 = ev->Function5();

	//printf("Event id %i , Event id x2 %i \n", eventId, test5);

	auto mata = (FWeVENT*)(ev);
	printf("event x %f, y %f, z %f \n", mata->x, mata->y, mata->z);

		return (eventId < _countof(g_eventNames2612)) ? g_eventNames2612[eventId] : nullptr;


}

void* (*g_eventCall1)(void* group, void* event);
void* (*g_eventCall2)(void* group, void* event);
void* (*g_eventCall3)(void* group, void* event);

template<decltype(g_eventCall3)* TFunc>
void* HandleEventWrap(fwEventGroup* group, fwEvent* event)
{
	if (event)
	{
		try
		{
			if (auto eventName = GetEventName(event))
			{
				printf("Called event name %s \n", eventName);
				//GameEventMetaData data{ eventName, 0 };

				// brute-force the argument count
				// since these functions should early exit, most cost here will be VMT dispatch
				// ^ as these functions have a hardcoded size check, though not all...
				/*for (int i = 0; i < _countof(data.arguments); i++)
				{
					if (event->GetArguments(data.arguments, i * sizeof(size_t)))
					{
						data.numArguments = i;
						break;
					}
				}*/

			//	OnTriggerGameEvent(data);
			}
		}
		catch (std::exception& e)
		{
		}
	}

	return (*TFunc)(group, event);
}

template<decltype(g_eventCall1)* TFunc, int stream>
void* HandleEventWrapExt(fwEventGroup* group, fwEvent* event)
{
	printf("Handle event wrap \n");
	if (event)
	{
		/*
		 * event:
		 * These events are stored with different member variables and with different layouts per class/struct,
		 * to get more data from them we'll have to investigate them case by case.
		 */

		try
		{
			if (auto eventName = GetEventName(event))
			{
				printf("Called event name %s \n", eventName);
				//GameEventData data{ eventName };

				//msgpack::sbuffer buf;
				//msgpack::packer packer(buf);

				//packer.pack_array(3); // we'll offer 3 parameters

				//if constexpr (stream == 1)
				//{
				//	// retrieve all entities this event is being sent to
				//	size_t size = group->eventsCount;
				//	packer.pack_array(size);

				//	for (size_t i = 0; i < size; ++i)
				//	{
				//		packer.pack(GetGuidFromBaseSafe(group->events[i]->GetEntity()));
				//	}
				//}
				//else
				//{
				//	// retrieve entity who is triggering this
				//	// this group has a different layout
				//	msgpack_array(packer, GetGuidFromBaseSafe(reinterpret_cast<rage::fwEventGroup2*>(group)->entity));
				//}

				//// retrieve entity related to the event
				//msgpack_pack(packer, GetGuidFromBaseSafe(event->GetEntity()));

				//// retrieve extra event data
				//const char* eventSubName = data.name + 6; // get part after "CEvent"

				//if (memcmp(eventSubName, "Shoc", 4) == 0) // Shocking
				//{
				//	msgpack_array(packer, (float(&)[3])event[8]);
				//}
				//else
				//{
				//	packer.pack_array(0); // empty array
				//}

				//data.argsData = std::string_view(buf.data(), buf.size());
				//OnTriggerGameEventExt(data);
			}
		}
		catch (std::exception&)
		{
			// an std::bad_alloc could be thrown when packing (msgpack);
			// should we do anything else except for not sending this to scripts?
		}
	}

	return (*TFunc)(group, event);
}

void CGameEvents::OnGameHook() {
	return;
	// these are for ped events, we don't handle getting entities/positions from aiEvent instances yet
	{
		auto matches = hook::pattern("83 BF ? ? 00 00 ? 75 ? 48 8B CF E8 ? ? ? ? 83 BF").count(2);

		MH_CreateHook(matches.get(0).get<void>(-0x36), HandleEventWrapExt<&g_eventCall1, 0>, (void**)&g_eventCall1);
		MH_CreateHook(matches.get(1).get<void>(-0x36), HandleEventWrapExt<&g_eventCall2, 1>, (void**)&g_eventCall2);
	}

	{
		MH_CreateHook(hook::get_pattern("81 BF ? ? 00 00 ? ?  00 00 75 ? 48 8B CF E8", -0x36), HandleEventWrap<&g_eventCall3>, (void**)&g_eventCall3);
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		MessageBox(0, "Failed hooking game event", "INfo", MB_OK);
	}
}
CGameEvents* g_GameEvents = new CGameEvents();