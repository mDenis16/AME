#include "CScriptVM.h"
#include "Hooking.h"
#include <MinHook.h>
#include "nativeList.h"
#include "CLevelLoader.h"

#include "Script.h"
#include "CScrEngine.h"
class tlsContext
{
public:
	char m_padding1[0xC8];          // 0x00
	void* m_allocator;   // 0xC8
	char m_padding2[0x760];         // 0xD0
	void* m_script_thread;     // 0x830
	bool m_is_script_thread_active; // 0x838

	static tlsContext* get()
	{
		return *reinterpret_cast<tlsContext**>(__readgsqword(0x58));
	}
};
using run_script_threads_t = bool(*)(std::uint32_t ops_to_execute);
run_script_threads_t run_script_threads_original = nullptr;

//E8 ? ? ? ? 41 03 FC 3B FE 7C E2 C6 05 ? ? ? ? ? EB 07 44 88 25 ? ? ? ?
//char __fastcall sub_7FF75CA966E4(__int64 a1, __int64 a2, __int64 a3)
static __m128(*g_RelatedToVehicleDensityCalc)(__int64 a1, __int64 a2, __int64 a3);
__m128 __fastcall RelatedToVehicleDensityCalc() {

	
	__m128 t{ 0.f, 0.f, 0.f, 0.f };


	return (__m128)t;
}

//48 83 EC 28 B1 01 E8 ? ? ? ? 84 C0 74 40 83 C9 FF E8 ? ? ? ? 84 C0 74 34 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 84 C0 74 1C
static char*(*g_SC_GET_NICKNAME_2_HK)(__int64 a1);
const char* __fastcall SC_GET_NICKNAME_2_HK(__int64 a1) {
	return "Light_Square";
}
void CScriptVM::add_script(std::unique_ptr<script> script)
{
	std::lock_guard lock(m_mutex);
	m_scripts.push_back(std::move(script));
}

void CScriptVM::remove_all_scripts()
{
	std::lock_guard lock(m_mutex);
	m_scripts.clear();
}
//E8 ? ? ? ? 48 85 FF 48 89 1D ? ? ? ? 8B D0 65 48 8B 04 25 ? ? ? ? 48 8B 0C F0 0F 95 C0 41 88 04 0E 8B C2 4A 89 3C 39 EB 02
__int64 __fastcall CrashesOnEscape(__int64 a1, __int64** a2, DWORD* a3, __int64 a4) {
	printf("CrashesOnEscape \n");
	return 0;
}

void CScriptVM::Hook() {
	
//	MessageBox(0, "CScriptVM::Hook", "Try hooking", MB_OK);
	


	//auto SC_GET_NICKNAME_TARGET = hook::pattern("48 83 EC 28 B1 01 E8 ? ? ? ? 84 C0 74 40 83 C9 FF E8 ? ? ? ? 84 C0 74 34 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 84 C0 74 1C").count(1).get(0).get<void>();
	//if (MH_CreateHook(SC_GET_NICKNAME_TARGET, &SC_GET_NICKNAME_2_HK, nullptr) != MH_OK) {
	//	std::cout << "Failed hooking SC_GET_NICKNAME_TARGET " << std::endl;
	//	return;
	//}

	//if (MH_EnableHook(SC_GET_NICKNAME_TARGET) != MH_OK)
	//	MessageBox(0, "Failed hooking SC_GET_NICKNAME_TARGET. ", "CScriptVM::SC_GET_NICKNAME_TARGET", MB_ICONERROR);


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
static std::vector<std::uint32_t> lista;

char  __fastcall CScriptVM::VMRun(std::uint32_t ops_to_execute)
{

 
	return run_script_threads_original(ops_to_execute);
}
static script* MainScript;

void Detest() {
	printf("mearsa scriptu \n");
	while (true) {
		static bool Spawned = false;
		if (!Spawned && GetAsyncKeyState(VK_F2)) {

			STREAMING::REQUEST_MODEL(0x705E61F2);
			while (!STREAMING::HAS_MODEL_LOADED(0x705E61F2))
				MainScript->yield();

			GamePed player = PLAYER::PLAYER_PED_ID();
			auto coords = ENTITY::GET_ENTITY_COORDS(player, true);

			PED::CREATE_PED(1, 0x705E61F2, coords.x, coords.y, coords.z, 0.f, true, false);
			Spawned = true;
			printf("sa spawnat cacatu \n");

		}
		MainScript->yield();
	}

	

}
void CScriptVM::OnGameHook()
{
//	MainScript = new script(Detest);
	//add_script(std::unique_ptr<script>(MainScript));
}
CScriptVM* g_ScriptVM = new CScriptVM();