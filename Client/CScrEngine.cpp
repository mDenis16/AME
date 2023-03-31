#include "atPool.h"

#include "CScrEngine.h"
#include "Hooking.h"
#include <MinHook.h>
#include "nativeList.h"
#include "CPoolManagement.h"
#include  "Hooking.h"

#include <unordered_set>
#include <spdlog/spdlog.h>

#include "CSyncManager.h"
#include "Script.h"

#include "pgCollection.h"

#include <memory>
#include "nativeList.h"
#include "GTA.h"

static pgPtrCollection<ScriptThread>* scrThreadCollection;
static uint32_t activeThreadTlsOffset;

static uint32_t* scrThreadId;
static uint32_t* scrThreadCount;
static std::unordered_set<ScriptThread*> g_ownedThreads;
static scriptHandlerMgr* g_scriptHandlerMgr;
class tlsContext
{
public:
	char m_padding1[0xC8];          // 0x00
	void* m_allocator;   // 0xC8
	char m_padding2[0x760];         // 0xD0
	scrThread* m_script_thread;     // 0x830
	bool m_is_script_thread_active; // 0x838

	static tlsContext* get()
	{
		return *reinterpret_cast<tlsContext**>(__readgsqword(0x58));
	}
};
scriptHandlerMgr* CScrEngine::GetScriptHandleMgr() {

	return g_scriptHandlerMgr;
}

pgPtrCollection<ScriptThread>* CScrEngine::GetThreadCollection() {

	return scrThreadCollection;
}

scrThread* CScrEngine::GetActiveThread() {

	return *reinterpret_cast<scrThread**>(hook::get_tls() + activeThreadTlsOffset);
}

void CScrEngine::SetActiveThread(scrThread* thread) {
	*reinterpret_cast<scrThread**>(hook::get_tls() + activeThreadTlsOffset) = thread;
}
eThreadState ScriptThread::Tick(uint32_t opsToExecute) {

	typedef eThreadState(__thiscall* ScriptThreadTick_t)(ScriptThread* ScriptThread, uint32_t opsToExecute);
	static ScriptThreadTick_t threadTickGta = (ScriptThreadTick_t)hook::pattern("80 B9 ? 01 00 00 00 8B FA 48 8B D9 74 05").count(1).get(0).get<void>(-0xF);

	return threadTickGta(this, opsToExecute);
}

void ScriptThread::Kill() {

	typedef void(__thiscall* ScriptThreadKill_t)(ScriptThread* ScriptThread);
	static ScriptThreadKill_t killScriptThread = (ScriptThreadKill_t)hook::pattern("48 83 EC 20 48 83 B9 ? 01 00 00 00 48 8B D9 74 14").count(1).get(0).get<void>(-6);

	return killScriptThread(this);
}

eThreadState ScriptThread::Run(uint32_t opsToExecute) {

	/*static bool mata = false;
	if (!mata) {
		mata = true;
		MessageBox(0, "mata", "mata", MB_OK);
	}*/
	//MessageBox(0, "Thread::Run", "", MB_OK);
	// Set the current thread

	scrThread* activeThread = CScrEngine::GetActiveThread();
	CScrEngine::SetActiveThread(this);
	
	// Invoke the running thing if we're not dead
	if (m_Context.m_State != ThreadStateKilled) {
		DoRun();
	}

	CScrEngine::SetActiveThread(activeThread);

	return m_Context.m_State;
}

void ScriptThreadInit(ScriptThread* thread) {

	typedef void(__thiscall* ScriptThreadInit_t)(ScriptThread* ScriptThread);
	static ScriptThreadInit_t ScriptThreadInit = (ScriptThreadInit_t)hook::pattern("83 89 ? 01 00 00 FF 83 A1 ? 01 00 00 F0").count(1).get(0).get<void>();

	return ScriptThreadInit(thread);
}

eThreadState ScriptThread::Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount) {

	memset(&m_Context, 0, sizeof(m_Context));

	m_Context.m_State = ThreadStateIdle;
	m_Context.m_iScriptHash = scriptHash;
	m_Context.m_iUnk1 = -1;
	m_Context.m_iUnk2 = -1;
	m_Context.m_iSet1 = 1;
	SetNetworkFlag(true);
	ScriptThreadInit(this);

	g_scriptHandlerMgr->AttachScript(this);
	m_pszExitMessage = (char*)"Normal exit";

	return m_Context.m_State;
}
static void(*origStartupScript)();
//class ScriptManagerThread : public ScriptThread {
//
//
//public:
//
//	void			DoRun() override;
//	eThreadState	Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount) override;
//	void add_script(std::unique_ptr<script> script) {
//		std::lock_guard lock(m_mutex);
//		m_scripts.push_back(std::move(script));
//	}
//private:
//	std::recursive_mutex m_mutex;
//	std::vector<std::unique_ptr<script>> m_scripts;
//};
//static ScriptManagerThread mata;
static void(*g_CTheScripts__Shutdown)(void);

static void CTheScripts__Shutdown()
{
	g_CTheScripts__Shutdown();

	for (auto& thread : g_ownedThreads)
	{
		thread->Reset(thread->GetContext()->m_iScriptHash, nullptr, 0);
	}
}
//static std::unique_ptr<script> TestScript;
//eThreadState ScriptManagerThread::Reset(uint32_t scriptHash, void* pArgs, uint32_t argCount) {
//
//	// Collect all scripts
//	return ScriptThread::Reset(scriptHash, pArgs, argCount);
//}
constexpr size_t patch_size = 24;
static inline std::once_flag once_flag;
static inline std::array<byte, patch_size> backup;
static inline void setup_backup()
{
	static auto spawn_bypass = hook::get_pattern("48 85 C0 0F 84 ? ? ? ? 8B 48 50");
	memcpy(backup.data(), spawn_bypass, patch_size);
}
void TestSmap()
{
	
	while (true) {
		static bool run = false;
		static int created_ped = 0;
		static netObject* netObjTest;
		
		
		/*if (created_ped) {
		
			using handle_to_pointer_t = int64_t(*)(int handle);
			static auto HandleToPointer = hook::get_address< handle_to_pointer_t>("83 f9 ff 74 31 4c 8b 0d ? ? ? ? 44 8b c1 49 8b 41 08");

			static auto getPlayerPedForNetPlayer = hook::get_call(hook::get_pattern<void* (void*)>("84 C0 74 1C 48 8B CF E8 ? ? ? ? 48 8B D8", 7));
			static auto 	g_playerMgr = *hook::get_address<netPlayerMgrBase**>(hook::get_pattern("40 80 FF 20 72 B3 48 8B 0D", 9));

			auto local_ped = getPlayerPedForNetPlayer(g_playerMgr->localPlayer);

			auto netObj = *(netObject**)((char*)local_ped + g_entityNetObjOffset);

			auto cloned_ped_net_obj = *(netObject**)((char*)HandleToPointer(created_ped) + g_entityNetObjOffset);
		
			if (cloned_ped_net_obj) {
				auto st = netObj->GetSyncTree();
				if (cloned_ped_net_obj->GetBlender())
				{
					cloned_ped_net_obj->GetBlender()->SetTimestamp(GAMEPLAY::GET_GAME_TIMER());

					cloned_ped_net_obj->GetBlender()->m_28();
					printf("mearsa blenderu \n");
				}
				else {
					printf("nu merge blenderu \n");
				}
				if (st->CanApplyToObject(cloned_ped_net_obj)) {
					st->ApplyToObject(cloned_ped_net_obj, nullptr);
				}

				cloned_ped_net_obj->PostSync();
			}
		}*/
		//g_SyncManager->OnFrame();
		//script::get_current()->yield();
	}
}
//void ScriptManagerThread::DoRun() {
//	static bool test = false;
//	if (!test) {
//		
//		TestScript = std::make_unique<script>(&TestSmap);
//		m_scripts.push_back(std::move(TestScript));
//		test = true;
//	}
//	static bool ensure_main_fiber = (ConvertThreadToFiber(nullptr), true);
//	std::lock_guard lock(m_mutex);
//	for (auto const& script : m_scripts)
//	{
//		script->tick();
//	}
//}
static void StartupScriptWrap()
{
	for (auto& thread : g_ownedThreads)
	{
		if (!thread->GetContext()->m_iThreadId)
		{
			thread->SetNetworkFlag(1);
			CScrEngine::CreateThread(thread);
		}
	}

	origStartupScript();
}

void CScrEngine::CreateThread(ScriptThread* thread) {
	// get a free thread slot
	auto collection = GetThreadCollection();
	int slot = 0;

	// first try finding the actual thread
	for (auto& threadCheck : *collection)
	{
		if (threadCheck == thread)
		{
			break;
		}

		slot++;
	}

	if (slot == collection->count())
	{
		slot = 0;

		for (auto& threadCheck : *collection)
		{
			auto context = threadCheck->GetContext();

			if (context->m_iThreadId == 0)
			{
				break;
			}

			slot++;
		}
	}

	// did we get a slot?
	if (slot == collection->count())
	{
		return;
	}

	{
		auto context = thread->GetContext();

		if (*scrThreadId == 0)
		{
			(*scrThreadId)++;
		}

		context->m_iThreadId = *scrThreadId;

		(*scrThreadId)++;
		std::stringstream ss;
		ss << "scr_" << (*scrThreadCount) + 1 << std::endl;
		
		thread->SetScriptName(ss.str().c_str());
		context->m_iScriptHash = (*scrThreadCount) + 1;

		(*scrThreadCount)++;

		collection->set(slot, thread);
	}
}
static uint32_t(*g_origScrProgramReturn)(void* a1, uint32_t a2);

static uint32_t ReturnIfMp(void* a1, uint32_t a2)
{
	return -1;
}
static int(*g_origNoScript)(void*, int);

static int JustNoScript(ScriptThread* thread, int a2)
{
	
	if (g_ownedThreads.find(thread) != g_ownedThreads.end())
	{
		thread->Run(0);
	}

	return thread->GetContext()->m_State;
}
void CScrEngine::AddToList(ScriptThread* thread) {
	g_ownedThreads.insert(thread);
}
void CScrEngine::Hook() {

	//g_ownedThreads.insert(&mata);

	MH_CreateHook(hook::pattern("8B 59 14 44 8B 79 18 8B FA 8B 51 0C").count(1).get(0).get<void>(-0x1D), ReturnIfMp, (void**)&g_origScrProgramReturn);
	MH_CreateHook(hook::pattern("48 83 EC 20 80 B9 ? 01 00 00 00 8B FA").count(1).get(0).get<void>(-0xB), JustNoScript, (void**)&g_origNoScript);
	
	auto scrThreadCollectionPattern = hook::pattern("48 8B C8 EB 03 49 8B CD 48 8B 05");

	char* location = scrThreadCollectionPattern.count(1).get(0).get<char>(11);
	if (location == nullptr) {

		spdlog::error("Unable to find scrThreadCollection");
		return;
	}
	scrThreadCollection = reinterpret_cast<decltype(scrThreadCollection)>(location + *(int32_t*)location + 4);
	spdlog::info("scrThreadCollection\t {}", (void*)scrThreadCollection);

	activeThreadTlsOffset = *hook::pattern("48 8B 04 D0 4A 8B 14 00 48 8B 01 F3 44 0F 2C 42 20").count(1).get(0).get<uint32_t>(-4);
	spdlog::info("activeThreadTlsOffset {0}", activeThreadTlsOffset);

	scrThreadId = hook::get_address<uint32_t*>(hook::get_pattern("8B 15 ? ? ? ? 48 8B 05 ? ? ? ? FF C2 89 15 ? ? ? ? 48 8B 0C D8", 2));

	spdlog::info("scrThreadId\t\t {}", (void*)scrThreadId);

	auto scrThreadCountPattern = hook::pattern("FF 0D ? ? ? ? 48 8B D9 75");

	location = scrThreadCountPattern.get(0).get<char>(2);
	if (location == nullptr) {

		spdlog::error("Unable to find scrThreadCount");
		return;
	}
	scrThreadCount = reinterpret_cast<decltype(scrThreadCount)>(location + *(int32_t*)location + 4);
	spdlog::info("scrThreadCount\t {}", (void*)scrThreadCount);

	

	auto g_scriptHandlerMgrPattern = hook::pattern("74 17 48 8B C8 E8 ? ? ? ? 48 8D 0D");

	location = g_scriptHandlerMgrPattern.count(1).get(0).get<char>(13);
	if (location == nullptr) {

		spdlog::error("Unable to find g_scriptHandlerMgr");
		return;
	}
	g_scriptHandlerMgr = reinterpret_cast<decltype(g_scriptHandlerMgr)>(location + *(int32_t*)location + 4);
	spdlog::info("g_scriptHandlerMgr\t {}", (void*)g_scriptHandlerMgr);

	// vector3 pointer fix
	if (auto void_location = hook::pattern("83 79 18 ? 48 8B D1 74 4A FF 4A 18").count(1).get(0).get<void>())
	{
	//	scrNativeCallContext::SetVectorResults = (void(*)(scrNativeCallContext*))(void_location);
	}

	auto mata = hook::get_pattern<char*>("83 FB FF 0F 84 D6 00 00 00", -0x37);

	MH_Initialize();
	MH_CreateHook(mata, StartupScriptWrap, (void**)&origStartupScript);
	MH_EnableHook(MH_ALL_HOOKS);

}
void CScrEngine::Unhook() {

}


void CScrEngine::OnGameHook() {

	MH_CreateHook(hook::get_pattern("48 8B 0D ? ? ? ? 33 D2 48 8B 01 FF 10", -0x58), CTheScripts__Shutdown, (void**)&g_CTheScripts__Shutdown);
}

void CScrEngine::OnGameFrame() {
}

CScrEngine* g_ScrEngine = new CScrEngine();
