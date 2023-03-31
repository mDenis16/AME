#include "CCleanWorld.h"
#include "Hooking.h"
#include <MinHook.h>
#include <spdlog/spdlog.h>
static char(*OPOPULATE_WORLD_WITH_VEHICLES)(void*, void*);
char POPULATE_WORLD_WITH_VEHICLES(void* a1, void* a2) {
	//printf("called POPULATE_WORLD_WITH_VEHICLES \n");
	return 0;
}
void __fastcall vehicle_populatiion_generator(__int64 a1, __int64 a2) {
	//printf("called vehicle_populatiion_generator \n");
}
__int64 __fastcall posible_related_some_population_generator(float* a1, float* a2, float a3) {
	//printf("called posible_related_some_population_generator \n");
	return 0;
}
float RelatedToPedDensityHK() {
	//48 8B C4 48 89 58 08 57 48 81 EC ? ? ? ? 0F 29 70 E8 0F 29 78 D8 44 0F 29 40 ? 33 DB 8B CB 44 0F 29 48 ? 44 0F 29 50 ? 44 0F 29 58 ? 44 0F 29 60 ? 44 0F 29 6C 24 ? 48 8B 05 ? ? ? ? 48 85 C0 74 04 0F B6 48 06
	return 0.f;
}

//E9 ? ? ? ? CC 24 E8 E8 ? ? ? ? 90 E9 ? ? ? ? F3 9B CC CD E7 E8 ? ? ? ? 90 E9 ? ? ? ?
void __fastcall test() {

}
void CCleanWorld::Hook() {
	
	spdlog::info("CCleanWorld::Hook");

	auto worldVehGen = hook::pattern("E8 ? ? ? ? EB 26 48 8D 15 ? ? ? ? 0F 28 D7 48 8B CE E8 ? ? ? ? EB 12 48 8D 15 ? ? ? ? 0F 28 D7 48 8B CE E8 ? ? ? ?").count(1).get(0).get<void>();
	auto worldVehGenCall = hook::get_call(worldVehGen);
	
	if (MH_CreateHook(worldVehGenCall, &POPULATE_WORLD_WITH_VEHICLES, nullptr) != MH_OK) {
		std::cout << "Failed hooking worldVehGenCall " << std::endl;
		return;
	}

	if (MH_EnableHook(worldVehGenCall) != MH_OK)
		MessageBox(0, "Failed hooking POPULATE_WORLD_WITH_VEHICLES . ", "CScriptVM::POPULATE_WORLD_WITH_VEHICLES", MB_ICONERROR);
	//
	auto worldGen2 = hook::pattern("E8 ? ? ? ? 44 8B 05 ? ? ? ? 41 FF C0 B8 ? ? ? ? 41 F7 E0 D1 EA 8D 04 52 44 2B C0 44 89 05 ? ? ? ? 44 3B C3").count(1).get(0).get<void>();
	auto worldGen2Call = hook::get_call(worldGen2);

	if (MH_CreateHook(worldGen2Call, &vehicle_populatiion_generator, nullptr) != MH_OK) {
		std::cout << "Failed hooking vehicle_populatiion_generator " << std::endl;
		return;
	}
	if (MH_EnableHook(worldGen2Call) != MH_OK)
		MessageBox(0, "Failed hooking vehicle_populatiion_generator . ", "CScriptVM::vehicle_populatiion_generator", MB_ICONERROR);

	auto worldGen3 = hook::pattern("E8 ? ? ? ? EB 12 48 8D 15 ? ? ? ? 0F 28 D7 48 8B CE E8 ? ? ? ? 44 8B 05 ? ? ? ?").count(1).get(0).get<void>();
	auto worldGen3Call = hook::get_call(worldGen3);

	if (MH_CreateHook(worldGen3Call, &posible_related_some_population_generator, nullptr) != MH_OK) {
		std::cout << "Failed hooking posible_related_some_population_generator " << std::endl;
		return;
	}
	if (MH_EnableHook(worldGen3Call) != MH_OK)
		MessageBox(0, "Failed hooking posible_related_some_population_generator . ", "CScriptVM::posible_related_some_population_generator", MB_ICONERROR);

	auto target = hook::pattern("48 8B C4 48 89 58 08 57 48 81 EC ? ? ? ? 0F 29 70 E8 0F 29 78 D8 44 0F 29 40 ? 33 DB 8B CB 44 0F 29 48 ? 44 0F 29 50 ? 44 0F 29 58 ? 44 0F 29 60 ? 44 0F 29 6C 24 ? 48 8B 05 ? ? ? ? 48 85 C0 74 04 0F B6 48 06").count(1).get(0).get<void>();
	if (MH_CreateHook(target, &RelatedToPedDensityHK, nullptr) != MH_OK) {
		std::cout << "Failed hooking RelatedToPedDensityHK " << std::endl;
		return;
	}


	if (MH_EnableHook(target) != MH_OK)
		MessageBox(0, "Failed hooking RelatedToPedDensityHK. ", "CScriptVM::RelatedToPedDensityHK", MB_ICONERROR);



	
}
void CCleanWorld::Unhook() {

}

void CCleanWorld::OnGameHook() {

}
CCleanWorld* g_CleanWorld = new CCleanWorld();
