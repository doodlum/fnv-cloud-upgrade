#include "Patches.h"
#include "CloudsUpgrade.h"

namespace Patches
{
	void Patches::Install()
	{
		CloudsUpgrade::InstallPatches();
		PrintLog("Installed all patches");
	}
}
