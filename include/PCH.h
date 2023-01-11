#pragma once

#pragma warning(push)
#include "Internal/Prefix.h"
#include "Internal/JIP_Core.h"
#pragma warning(pop)

#define NVSEAPI __cdecl
#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"
#include <fmt/core.h>
#include <fmt/format.h>

#include <detours.h>

#define ImTextureID unsigned long long

#include <imgui.h>
#include <reshade/reshade.hpp>
