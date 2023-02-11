#include "CScriptVM.h"
#include "Hooking.h"
#include <MinHook.h>
#include "nativeList.h"
#include "CLevelLoader.h"
#include <string>
using run_script_threads_t = bool(*)(std::uint32_t ops_to_execute);
run_script_threads_t run_script_threads_original = nullptr;

//E8 ? ? ? ? 41 03 FC 3B FE 7C E2 C6 05 ? ? ? ? ? EB 07 44 88 25 ? ? ? ?
//char __fastcall sub_7FF75CA966E4(__int64 a1, __int64 a2, __int64 a3)
static __m128(*g_RelatedToVehicleDensityCalc)(__int64 a1, __int64 a2, __int64 a3);
__m128 __fastcall RelatedToVehicleDensityCalc() {

	
	__m128 t{ 0.f, 0.f, 0.f, 0.f };


	return (__m128)t;
}

float parked_density_calc() {

	return 0.f;
}
//85 D2 0F 88 ? ? ? ? B8 ? ? ? ? 75 0E 44 3B C8 0F 85 ? ? ? ? E9 ? ? ? ? 3B D0 75 17 45 85 C9 0F 84 ? ? ? ? 44 3B C8 0F 85 ? ? ? ? E9 ? ? ? ?
void __fastcall ManageAllWorldGenerators() {

}
char __fastcall parked_vehicles_generator_loop(__int64 a1, char a2) {
	return 1;
}


void CScriptVM::Hook() {
	
//	MessageBox(0, "CScriptVM::Hook", "Try hooking", MB_OK);

	auto RunScriptThreadTarget = hook::get_pattern<void>("45 33 F6 8B E9 85 C9 B8", -0x1F);


	char* ScriptVmCaller = hook::pattern(/*"E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 4C 8D 9C 24 ? ? ? ? 49 8B 5B 40 49 8B 73 48 49 8B E3 "*/"45 33 F6 8B E9 85 C9 B8").count(1).get(0).get<char>(-0x1F);
	///char* ScriptVMCall = hook::get_call(ScriptVmCaller);
		//
	if (MH_CreateHook(ScriptVmCaller, &VMRun, reinterpret_cast<void**>(&run_script_threads_original)) != MH_OK) {
		std::cout << "Failed hooking run script threads " << std::endl;
		return;
	}


	if (MH_EnableHook(ScriptVmCaller) != MH_OK)
		MessageBox(0, "Failed hooking VMScript thread. ", "CScriptVM::Hook", MB_ICONERROR);
	
	

	//return;
	//auto target = hook::pattern("48 8B C4 48 89 58 08 57 48 81 EC ? ? ? ? 0F 29 70 E8 0F 29 78 D8 44 0F 29 40 ? 33 DB 8B CB 44 0F 29 48 ? 44 0F 29 50 ? 44 0F 29 58 ? 44 0F 29 60 ? 44 0F 29 6C 24 ? 48 8B 05 ? ? ? ? 48 85 C0 74 04 0F B6 48 06").count(1).get(0).get<void>();
	//if (MH_CreateHook(target, &RelatedToPedDensityHK, nullptr) != MH_OK) {
	//	std::cout << "Failed hooking RelatedToPedDensityHK " << std::endl;
	//	return;
	//}


	//if (MH_EnableHook(target) != MH_OK)
	//	MessageBox(0, "Failed hooking RelatedToPedDensityHK. ", "CScriptVM::RelatedToPedDensityHK", MB_ICONERROR);


	//auto vehicle_density_call = hook::pattern("E8 ? ? ? ? F3 0F 59 05 ? ? ? ? F3 0F 59 05 ? ? ? ? F3 0F 2C D8 85 DB 7E 4D 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? F6 46 24 40 74 14 48 8D 8D ? ? ? ? 45 8A C4 BA ? ? ? ? E8 ? ? ? ?").count(1).get(0).get<void>();
	//auto vehicle_density_target = hook::get_call(vehicle_density_call);

	//if (MH_CreateHook(vehicle_density_target, &RelatedToVehicleDensityCalc, nullptr) != MH_OK) {
	//	std::cout << "Failed hooking RelatedToVehicleDensityCalc " << std::endl;
	//	return;
	//}


	//if (MH_EnableHook(vehicle_density_target) != MH_OK)
	//	MessageBox(0, "Failed hooking RelatedToVehicleDensityCalc. ", "CScriptVM::RelatedToVehicleDensityCalc", MB_ICONERROR);

	//
	//auto parked_density_call = hook::pattern("E8 ? ? ? ? 48 8B 05 ? ? ? ? 0F 28 F0 0F B6 58 04 F3 0F 59 35 ? ? ? ? E8 ? ? ? ? 0F 57 D2 0F 57 C9 F3 0F 59 05 ? ? ? ? F3 48 0F 2A CB F3 48 0F 2A D7 F3 0F 59 C1 F3 0F 59 D6 F3 0F 58 C2 F3 0F 2C C0 3B 05 ? ? ? ? 0F 4F 05 ? ? ? ? ").count(1).get(0).get<void>();
	//auto parket_density_target = hook::get_call(parked_density_call);

	//if (MH_CreateHook(vehicle_density_target, &parked_density_calc, nullptr) != MH_OK) {
	//	std::cout << "Failed hooking RelatedToVehicleDensityCalc " << std::endl;
	//	return;
	//}


	//if (MH_EnableHook(vehicle_density_target) != MH_OK)
	//	MessageBox(0, "Failed hooking parked_density_call. ", "CScriptVM::parked_density_call", MB_ICONERROR);


	////E8 ? ? ? ? 8B 0D ? ? ? ? 3B C8 7C 05 41 8B C4 EB 02

	//auto return_calculated_scenario_ped_density_caller = hook::pattern("E8 ? ? ? ? 8B 0D ? ? ? ? 3B C8 7C 05 41 8B C4 EB 02").count(1).get(0).get<void>();
	//auto return_calculated_scenario_ped_density_call = hook::get_call(return_calculated_scenario_ped_density_caller);

	//if (MH_CreateHook(return_calculated_scenario_ped_density_call, &parked_density_calc, nullptr) != MH_OK) {
	//	std::cout << "Failed hooking RelatedToVehicleDensityCalc " << std::endl;
	//	return;
	//}


	//if (MH_EnableHook(return_calculated_scenario_ped_density_call) != MH_OK)
	//	MessageBox(0, "Failed hooking parked_density_call. ", "CScriptVM::parked_density_call", MB_ICONERROR);

}

void CScriptVM::Unhook() {

}
class cpedfactory {
public:

};
bool CScriptVM::VMRun(std::uint32_t ops_to_execute)
{

	//E8 ? ? ? ? 90 EB 85 CC 48 CC
	// 
	

	//MessageBox(0, "Mearsa", "Mearsa", MB_OK);
	//if (GetAsyncKeyState(VK_SHIFT)) {
	//	static void* getFunctionCaller = hook::pattern("E8 ? ? ? ? 90 EB 85 CC 48 CC").count(2).get(1).get<void>();
	//	static void* getFunctinonCall = hook::get_call(getFunctionCaller);
	//	GamePed player = PLAYER::PLAYER_PED_ID();
	//	printf("player %i \n", player);
	//	//_int64 __fastcall GIVE_WEAPON_TO_PED_REAL_FUNCTION(__int64 a1, unsigned int a2, unsigned int a3, char a4, char a5)
	//	reinterpret_cast<void(__cdecl*)(__int64 a1, unsigned int a2, unsigned int a3, char a4, char a5)>(getFunctinonCall)(player, 0xBFE256D4, 30, 0, 0);
	//}


	//if (GetAsyncKeyState(VK_SHIFT)) {
	//	static void* getFunctionCaller = hook::get_pattern("48 89 5C 24 ? 57 48 83 EC 40 80 3D ? ? ? ? ? 41 8A D8 40 8A FA 75 5D E8 ? ? ? ? 4C 8B D8 48 85 C0 74 50", 1);


	//	GamePed player = PLAYER::PLAYER_PED_ID();
	//	NativeVehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(player);

	//	printf("player %i \n", player);
	//	printf("player vehicle %i", vehicle);
	//
	//	static auto PointerToHandleCaller = hook::pattern("E8 ? ? ? ? 48 8D 4D A7 8B D8 41 89 06 E8 ? ? ? ? 8B 55 6F 48 8D 0D ? ? ? ? 41 B8 ? ? ? ? 03 15 ? ? ? ? E8 ? ? ? ? 81 A6 ? ? ? ? ? ? ? ? 0F BA AE ? ? ? ? ? E9 ? ? ? ?").count(1).get(1).get<void>();
	//	static auto PointerToHandleCal = hook::get_call(PointerToHandleCaller);
	//		
	//	
	//	auto handle = reinterpret_cast<int(__cdecl*)(void* a1)>(PointerToHandleCal)(local_player);
	//	//_int64 __fastcall GIVE_WEAPON_TO_PED_REAL_FUNCTION(__int64 a1, unsigned int a2, unsigned int a3, char a4, char a5)
	//	//reinterpret_cast<void(__cdecl*)(__int64 a1, char a2, char a3)>(getFunctionCaller)(vehicle, 1, 0);
	//}

	//
	
	static bool Called = false;

	if (!Called && CBaseFactory<CLevelLoader>::Get().state == GAME_STATE::IN_GAME) {
		printf("Called spawn player faster \n");
		GamePed player = PLAYER::PLAYER_PED_ID();

		SCRIPT::SHUTDOWN_LOADING_SCREEN();
		CAM::DO_SCREEN_FADE_IN(0);

		ENTITY::SET_ENTITY_COORDS(player, 293.089f, 180.466f, 104.301f, true, true, true, false);
		Called = true;
	}

	//static bool m_hosted = false;
	//if (GetAsyncKeyState(VK_SHIFT)) {
	//	printf("NETWORK::NETWORK_IS_HOST() %d\n", NETWORK::NETWORK_IS_HOST());
	//	if (!m_hosted && NETWORK::NETWORK_IS_HOST())
	//	{
	//		// NETWORK_SESSION_VALIDATE_JOIN
	//		NETWORK::_NETWORK_SESSION_HOSTED(true);
	//		//NETWORK::NETWORK_DO_TRANSITION_TO_FREEMODE()
	//		printf("_NETWORK_SESSION_HOSTED \n");
	//		m_hosted = true;
	//	}
	//}
	//static bool hosted = false;
	//if (!hosted && GetAsyncKeyState(VK_MENU)) {
	//	//hostGame(0, 32, 0x0);
	//	printf("trying hosting \n");
	//	NETWORK::NETWORK_HOST_TRANSITION(1, 1, 1, 1, 0, true, true, 1, 1, 32768);
	//	NETWORK::NETWORK_LAUNCH_TRANSITION();
	//}

	return static_cast<decltype(&VMRun)>(run_script_threads_original)(ops_to_execute);
}

void CScriptVM::OnGameHook()
{

}
CScriptVM* g_ScriptVM = new CScriptVM();