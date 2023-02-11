#pragma once
#include "CBaseFactory.h"
#include <iostream>
#include <unordered_map>
#include <functional>
namespace RAGE
{
	class ScrNativeCallContext
	{
	public:
		void Reset()
		{
			ArgCount = 0;
			DataCount = 0;
		}

		template <typename T>
		void PushArg(T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(Args) + (ArgCount++)) = std::forward<T>(value);
		}

		template <typename T>
		T& GetArg(std::size_t index)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			return *reinterpret_cast<T*>(reinterpret_cast<std::uint64_t*>(Args) + index);
		}

		template <typename T>
		void SetArg(std::size_t index, T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(Args) + index) = std::forward<T>(value);
		}

		template <typename T>
		T* GetReturnValue()
		{
			return reinterpret_cast<T*>(ReturnVal);
		}

		template <typename T>
		void set_return_value(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(ReturnVal) = std::forward<T>(value);
		}
	protected:
		void* ReturnVal;
		std::uint32_t ArgCount;
		void* Args;
		std::int32_t DataCount;
		std::uint32_t Data[48];
	};

	using ScrNativeHash = std::uint64_t;
	using ScrNativeMapping = std::pair<ScrNativeHash, ScrNativeHash>;
	using ScrNativeHandler = void(*)(ScrNativeCallContext*);

	class ScrNativeRegistration;

#pragma pack(push, 1)
	class ScrNativeRegistrationTable
	{
		ScrNativeRegistration* Entries[0xFF];
		std::uint32_t Unknown;
		bool Initialized;
	};
#pragma pack(pop)

	//assert(sizeof(scrNativeCallContext) == 0xE0);
}

using NativeVoid = void;
using NativeAny = std::uint32_t;
using NativeHash = std::uint32_t;
using NativeEntity = std::int32_t;
using NativePlayer = std::int32_t;
using NativeFireId = std::int32_t;
using NativeInterior = std::int32_t;
using NativePed = NativeEntity;
using NativeVehicle = NativeEntity;
using NativeCam = std::int32_t;
using NativeObject = NativeEntity;
using NativePickup = NativeObject;
using NativeBlip = std::int32_t;
using NativeCamera = NativeEntity;
using NativeScrHandle = NativeEntity;
using NativeVector3 = glm::vec3;
#define fullHashMapCount 6354
#define fullHashMapDepth 24

#pragma pack(push)
#pragma pack(4)		// _unknown 4 bytes
// https://www.unknowncheats.me/forum/grand-theft-auto-v/144028-reversal-thread-81.html#post1931323
struct NativeRegistration {
	uint64_t nextRegBase;
	uint64_t nextRegKey;
	RAGE::ScrNativeHandler handlers[7];
	uint32_t numEntries1;
	uint32_t numEntries2;
	uint32_t _unknown;
	uint64_t hashes[7];

	/*
		// decryption
		key = this ^ nextRegKey  // only lower 32 bits
		nextReg = nextRegBase ^ key<<32 ^ key
		// encryption
		key = this ^ nextRegKey  // only lower 32 bits
		nextRegBase = nextReg ^ key<<32 ^ key
		only lower 32 bits of this^nextRegKey are used, higher 32 bits are ignored.
		thus, higher 32 bit of nexRegBase must contain the info of (masked) higher address of next registration.
		the first two members of struct are named as Base/Key respectively in that sense.
	*/
	inline NativeRegistration* getNextRegistration() {
		uint32_t key = static_cast<uint32_t>(reinterpret_cast<uint64_t>(this) ^ nextRegKey);
		return reinterpret_cast<NativeRegistration*>(nextRegBase ^ (static_cast<uint64_t>(key) << 32) ^ key);
	}

	inline void setNextRegistration(NativeRegistration* nextReg, uint64_t nextKey) {
		nextRegKey = nextKey;
		uint32_t key = static_cast<uint32_t>(reinterpret_cast<uint64_t>(this) ^ nextRegKey);
		nextRegBase = reinterpret_cast<uint64_t>(nextReg) ^ (static_cast<uint64_t>(key) << 32) ^ key;
	}

	inline uint32_t getNumEntries() {
		return static_cast<uint32_t>(reinterpret_cast<uint64_t>(&numEntries1)) ^ numEntries1 ^ numEntries2;
	}

	inline uint64_t getHash(uint32_t index) {
		uint32_t key = static_cast<uint32_t>(reinterpret_cast<uint64_t>(&hashes[2 * index]) ^ hashes[2 * index + 1]);
		return hashes[2 * index] ^ (static_cast<uint64_t>(key) << 32) ^ key;
	}
};
#pragma pack(pop)

class NativeCallContext : public RAGE::ScrNativeCallContext
{
public:
	NativeCallContext();
private:
	std::uint64_t ReturnStack[10];
	std::uint64_t ArgsStack[100];
};

#include <Windows.h>
CFactory(CNativeInvoker)
{
public:

	CNativeInvoker() { MessageBox(0, "Construct CNativeInvoker", "Constructor info", MB_OK); };

	static bool OnLookAlive();

	void Hook() override;
	void Unhook() override;
	void OnGameHook() override;
	static void __fastcall BeforeRegisteringNatives(void* a1);
	static void RegisterNativeHK(void* a1, void* a2);
	RAGE::ScrNativeHandler GetNativeHandler(uint64_t oldHash);
	void BeginCall();
	template <typename T>
	void PushArg(T&& value)
	{
		CallContext.PushArg(std::forward<T>(value));
	}

	template <typename T>
	T& GetReturnValue()
	{
		return *CallContext.GetReturnValue<T>();
	}
	uint64_t GetNewHashFromOldHash(uint64_t oldHash);
	uint64_t fullHashMap[fullHashMapCount][fullHashMapDepth];
	int SearchDepth = 23;

	NativeRegistration** registrationTable;
	std::unordered_map<uint64_t, RAGE::ScrNativeHandler> foundHashCache;
	void EndCall(RAGE::ScrNativeHash hash);

	void* FixVectors;
	void* GetNativeHandleCall;

	void DumpTable();
private:
	NativeCallContext CallContext;
	std::unordered_map<RAGE::ScrNativeHash, RAGE::ScrNativeHandler> HandlerCache;
};
extern CNativeInvoker* g_NativeInvoker;