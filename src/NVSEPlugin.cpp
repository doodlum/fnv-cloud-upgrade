#include "CloudsUpgrade.h"
#include "Patches.h"

#include <NVSE/PluginAPI.h>

extern "C" __declspec(dllexport) const char* NAME = "Clouds Upgrade NVSE";

extern "C" __declspec(dllexport) const char* DESCRIPTION = "by doodlez.\n";

HMODULE hModuleBackup;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		hModuleBackup = hModule;
		break;
	}
	return TRUE;
}

void Load(const NVSEInterface* nvse)
{
	if (!nvse->isEditor)
		Patches::Install();
}

extern "C" DLLEXPORT bool NVSEAPI NVSEPlugin_Load(const NVSEInterface* nvse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {};
#endif
	s_log().Create(fmt::format("{}.log", Plugin::NAME).data());
	PrintLog("Loaded plugin");

	Load(nvse);

	return true;
}

extern "C" DLLEXPORT bool NVSEAPI NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = Plugin::NAME;
	return true;
}
