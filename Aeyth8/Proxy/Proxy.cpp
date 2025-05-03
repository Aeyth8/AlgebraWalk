#include "Proxy.hpp"
#include "../Logger.hpp"

// Wasted time making this as turns out the executable doesn't load it but I was lazy to check
// Update: AFTER LOOKING AT ALL THE IMPORTS, LEGACY.DLL DOES NOT EVEN GET USED ANYWHERE IN THE GAME, NONE OF THE EXTERNALS USE IT EITHER
/*

	After a few hours of wasting my time (not)
	I realized that the only DLL worth proxying is the steamapi, goldberg doesn't work so I'll just write my own.

namespace Legacy
{
	typedef __int64(__fastcall* provider_init_t)(__int64 handle, int* in, __int64** out, __int64* a4);
	static provider_init_t Callback{0};

	EXPORT __int64 __fastcall OSSL_provider_init(__int64 handle, int* in, __int64** out, __int64* a4)
	{
		return (*Callback)(handle, in, out, a4);
	}
}

namespace Glew
{
	//typedef __int64(__fastcall* _glewGetUniformLocation)(__int64, __int64);
	//__glewGetUniformLocation
	// __glewVertexAttribPointer

	EXPORT LPVOID __glewGetUniformLocation{nullptr};
	EXPORT LPVOID __glewVertexAttribPointer{nullptr};


}*/


struct ProxyStructure { void** FunctionPointer; const char* FunctionName; };

static void InitProxyPointers(const HMODULE& hModule, const std::vector<ProxyStructure>& Table)
{
	for (int i{0}; i < Table.size(); ++i)
	{
		*Table[i].FunctionPointer = (void*)(GetProcAddress(hModule, Table[i].FunctionName));
		Logger::Log << "[Proxy] - POINTERS :: " << Table[i].FunctionName << " " << std::hex << std::uppercase << Table[i].FunctionPointer << "\n";
	}


}

namespace SteamAPI
{
	// I just copied/modified the pseudocode bec proxying is SO BORING, I need to write a proxy generator..   

	typedef __int64 _QWORD;

	namespace Decl
	{
		typedef __int64(__fastcall* SteamAPI_RegisterCallback_)(_QWORD, _QWORD);
		SteamAPI_RegisterCallback_ SteamAPI_RegisterCallback{0};

		typedef __int64(__fastcall* SteamInternal_SteamAPI_Init_)(__int64 a1, void* a2);
		SteamInternal_SteamAPI_Init_ SteamInternal_SteamAPI_Init{0};

		typedef char (*SteamAPI_RunCallbacks_)();
		SteamAPI_RunCallbacks_ SteamAPI_RunCallbacks{0};

		typedef __int64(*SteamAPI_Shutdown_)();
		SteamAPI_Shutdown_ SteamAPI_Shutdown{0};

		typedef __int64(__fastcall* SteamInternal_ContextInit_)(__int64 a1);
		SteamInternal_ContextInit_ SteamInternal_ContextInit{0};

		typedef void(__fastcall* SteamAPI_UnregisterCallback_0_)(__int64 a1); // Pretty stupid IMO but the original call was to a wrapper which just called the internal (this)
		SteamAPI_UnregisterCallback_0_ SteamAPI_UnregisterCallback_0{0};

		typedef __int64 (*SteamAPI_GetHSteamUser_)();
		SteamAPI_GetHSteamUser_ SteamAPI_GetHSteamUser{0};

		typedef __int64(__fastcall* SteamInternal_FindOrCreateUserInterface_)(unsigned int a1, __int64 a2);
		SteamInternal_FindOrCreateUserInterface_ SteamInternal_FindOrCreateUserInterface{0};

		const std::vector<ProxyStructure> Table =
		{
			{(void**)&SteamAPI_RegisterCallback, "SteamAPI_RegisterCallback"},
			{(void**)&SteamInternal_SteamAPI_Init, "SteamInternal_SteamAPI_Init"},
			{(void**)&SteamAPI_RunCallbacks, "SteamAPI_RunCallbacks"},
			{(void**)&SteamAPI_Shutdown, "SteamAPI_Shutdown"},
			{(void**)&SteamInternal_ContextInit, "SteamInternal_ContextInit"},
			{(void**)&SteamAPI_UnregisterCallback_0, "SteamAPI_UnregisterCallback"},
			{(void**)&SteamAPI_GetHSteamUser, "SteamAPI_GetHSteamUser"},
			{(void**)&SteamInternal_FindOrCreateUserInterface, "SteamInternal_FindOrCreateUserInterface"}
		};

	}
	
	EXPORT __int64 SteamAPI_RegisterCallback(_QWORD a, _QWORD b)
	{
		return Decl::SteamAPI_RegisterCallback(a, b);
	}

	EXPORT __int64 SteamInternal_SteamAPI_Init(__int64 a1, void* a2)
	{
		return Decl::SteamInternal_SteamAPI_Init(a1, a2);
	}

	EXPORT char SteamAPI_RunCallbacks()
	{
		return Decl::SteamAPI_RunCallbacks();
	}

	EXPORT __int64 SteamAPI_Shutdown()
	{
		return Decl::SteamAPI_Shutdown();
	}

	EXPORT __int64 SteamInternal_ContextInit(__int64 a1)
	{
		return Decl::SteamInternal_ContextInit(a1);
	}

	EXPORT void SteamAPI_UnregisterCallback(__int64 a1)
	{
		return Decl::SteamAPI_UnregisterCallback_0(a1);
	}

	EXPORT __int64 SteamAPI_GetHSteamUser()
	{
		return Decl::SteamAPI_GetHSteamUser();
	}

	EXPORT __int64 SteamInternal_FindOrCreateUserInterface(unsigned int a1, __int64 a2)
	{
		return Decl::SteamInternal_FindOrCreateUserInterface(a1, a2);
	}








}


bool Proxy::Attach(HMODULE CurrentModule)
{
	WCHAR Path[260]{0};

	++AttachCounter;
	if (AttachCounter == 1) Logger::Init();

	Logger::Log << GetCommandLineA() << "\n[Proxy] - ATTACH :: Count = " << AttachCounter << "\n";

	if (!GetModuleFileNameW(CurrentModule, Path, _countof(Path))) return false;

	std::wstring PathToDLL(Path);

	int AddR = PathToDLL.find_last_of(L"\\");

	PathToDLL.insert(AddR + 1, L"r");

	RealDLL = LoadLibraryW(PathToDLL.c_str());

	if (RealDLL == nullptr) return false;

	InitProxyPointers(RealDLL, SteamAPI::Decl::Table);
	//Legacy::Callback = (Legacy::provider_init_t)GetProcAddress(RealDLL, "OSSL_provider_init");

	AllocConsole();

	Logger::Log.flush();

	return true;
}

void Proxy::Detach()
{


}
