#include "Patches.h"
#include "CloudUpgrade.h"

namespace Patches
{
	void Patches::Install()
	{
		CloudUpgrade::InstallPatches();
		PrintLog("Installed all patches");
	}
}
