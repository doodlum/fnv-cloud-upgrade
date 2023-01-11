#include "CloudsUpgrade.h"

#include <SimpleIni.h>

float naive_lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

void CloudsUpgrade::UpdateCloud(int a_layer, float a_timePassed, float a_noise)
{
	if (auto sky = TES::GetSingleton()->sky) {
		if (auto clouds = sky->clouds) {
			if (auto niBlock = clouds->layers[a_layer]) {
				float rotation = a_noise;
				rotation *= speed * layers[a_layer].subSpeed;
				rotation = std::clamp(rotation, -maxSpeed, maxSpeed);
				rotation *= sky->windSpeed;
				rotation *= a_timePassed;
				rotation = naive_lerp(rotation, rotation * (float)layers[a_layer].noise.noise1D(a_layer), layers[a_layer].subVolatility);
				ResultVars  outRot;
				NiMatrix33& rotMat = niBlock->LocalRotate();
				NiVector3   euler = NiVector3{ rotMat };
				euler.z = std::fmod(euler.z + rotation, 360);
				rotMat = NiMatrix33{ euler };
			}
		}
	}
}


// "bumstep lol"
// "Oh bump"
// "That makes more sense2

//float BumStep(float edge0, float edge1, float x)
//{
//	return 1.0 - abs(std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f) - 0.5f) * 2.0f;
//}

//void CloudsUpgrade::UpdateCloudSunriseSunset(int a_layer)
//{
//	if (auto sky = TES::GetSingleton()->sky) {
//		if (auto clouds = sky->clouds) {
//			if (auto niBlock = clouds->layers[a_layer]) {
//				// Sunrise Values
//				float sunriseStart = ThisStdCall<float>(0x595EA0, sky);
//				float sunriseEnd = ThisStdCall<float>(0x595F50, sky);
//
//				// Sunset Values
//				float sunsetStart = ThisStdCall<float>(0x595FC0, sky);
//				float sunsetEnd = ThisStdCall<float>(0x596030, sky);
//
//				float multiplier = 0;
//
//				float gameHour = sky->gameHour;
//				bool  sunset = false;
//
//
//
//				// Sunset
//				if ((gameHour >= sunsetStart) && (gameHour <= sunsetEnd)) {
//					multiplier = BumStep(sunsetStart, sunsetEnd, gameHour);
//					ResultVars  outRot;
//					NiMatrix33& rotMat = niBlock->LocalRotate();
//					NiVector3   euler = NiVector3{ rotMat };
//					euler.z = 180;
//					rotMat = NiMatrix33{ euler };
//				}
//				// Sunrise
//				else if ((gameHour >= sunriseStart) && (gameHour <= sunriseEnd)) {
//					multiplier = BumStep(sunriseStart, sunriseEnd, gameHour);
//					ResultVars  outRot;
//					NiMatrix33& rotMat = niBlock->LocalRotate();
//					NiVector3   euler = NiVector3{ rotMat };
//					euler.z = 0;
//					rotMat = NiMatrix33{ euler };
//				}
//				niBlock->shaderProp->alpha = multiplier;
//				niBlock->shaderProp->fadeAlpha = multiplier;
//				niBlock->materialProp->alpha = multiplier;
//			}
//		}
//	}
//}


float DaysPassed()
{
	int bgnYear = 2281, bgnMonth = 10, bgnDay = 13;
	if (auto globals = GameTimeGlobals::Get()) {
		int totalDays = 0, iter;
		int iYear = globals->year->data, iMonth = globals->month->data, iDay = globals->day->data;
		iYear -= bgnYear;
		if (iYear > 0) {
			totalDays = kDaysPerMonth[bgnMonth] - bgnDay;
			for (iter = bgnMonth + 1; iter < 12; iter++)
				totalDays += kDaysPerMonth[iter];
			iYear--;
			if (iYear)
				totalDays += (365 * iYear);
			for (iter = 0; iter < iMonth; iter++)
				totalDays += kDaysPerMonth[iter];
			totalDays += iDay - 1;
		} else if (bgnMonth < iMonth) {
			totalDays = kDaysPerMonth[bgnMonth] - bgnDay;
			for (iter = bgnMonth + 1; iter < iMonth; iter++)
				totalDays += kDaysPerMonth[iter];
			totalDays += iDay - 1;
		} else
			totalDays = iDay - bgnDay;
		return (float)totalDays + globals->hour->data * (1 / 24.0f);
	} 
	return 0.0f;
}

#define ImTextureID unsigned long long

#include <imgui.h>
#include <reshade/reshade.hpp>

extern HMODULE hModuleBackup;

static void    DisplaySettings(reshade::api::effect_runtime*)
{
	auto clouds = CloudsUpgrade::GetSingleton();

	ImGui::InputFloat("Global Speed", &clouds->speed);
	ImGui::InputFloat("Global Max Speed", &clouds->maxSpeed);
	ImGui::InputFloat("Global Volatility", &clouds->volatility);

	for (int i = 0; i < 4; i++) {
		ImGui::Text(fmt::format("Layer {}", i).c_str());
		auto& layer = clouds->layers[i];
		ImGui::InputFloat(fmt::format("Layer {} Sub Speed", i).c_str(), &layer.subSpeed);
		ImGui::InputFloat(fmt::format("Layer {} Sub Volatility", i).c_str(), &layer.subVolatility);
	}
}

void CloudsUpgrade::Update()
{
	if (!init) {
		init = true;
		if (!reshade::register_addon(hModuleBackup)) {
			PrintLog("Failed to register addon");
		} else {
			PrintLog("Registered addon");
		}
		reshade::register_overlay(nullptr, &DisplaySettings);
	}
	if (auto menu = HUDMainMenu::Get()) {
		if (auto globals = GameTimeGlobals::Get()) {
			float& globalTimeMultiplier = (*(float*)0x11AC3A0);

			float  timePassed = menu->timer.lastSecondsPassed * globalTimeMultiplier / 100;
			float daysPassed = DaysPassed();

			for (int i = 0; i < 4; i++) {
				UpdateCloud(i, timePassed, noise.noise1D(daysPassed * volatility));
			}
		}
	}
}

[[nodiscard]] __declspec(noinline) UInt32 __stdcall DetourVtable(UInt32 addr, UInt32 dst)
{
	UInt32 originalFunction = *(UInt32*)addr;
	SafeWrite32(addr, dst);
	return originalFunction;
}

template <typename T_Ret = UInt32, typename... Args>
__forceinline T_Ret ThisStdCall(UInt32 _addr, const void* _this, Args... args)
{
	return ((T_Ret(__thiscall*)(const void*, Args...))_addr)(_this, std::forward<Args>(args)...);
}

UInt32 originalCloudsUpdateAddr;

void __fastcall CloudsUpdateHook(Clouds* clouds, void* edx, Sky* sky, float value)
{
	ThisCall(originalCloudsUpdateAddr, clouds, sky, value);
	CloudsUpgrade::GetSingleton()->Update();
}

void CloudsUpgrade::InstallHooks()
{
	originalCloudsUpdateAddr = DetourVtable(kVtbl_Clouds + 0xC, UInt32(CloudsUpdateHook));
}

void CloudsUpgrade::LoadINI()
{
	CSimpleIniA                        ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\NVSE\\Plugins\\CloudsUpgrade.ini");

	speed = (float)ini.GetDoubleValue("Global", "Speed", speed);
	maxSpeed = (float)ini.GetDoubleValue("Global", "MaxSpeed", maxSpeed);
	volatility = (float)ini.GetDoubleValue("Global", "Volatility", volatility);

	for (int i = 0; i < 4; i++) {
		std::string layerName = fmt::format("Layer {}", i + 1);
		layers[i].subSpeed = (float)ini.GetDoubleValue(layerName.c_str(), "SubSpeed", layers[i].subSpeed);
		layers[i].subVolatility = (float)ini.GetDoubleValue(layerName.c_str(), "SubVolatility", layers[i].subVolatility);
	}
}
