#include <cmath>
#include <cassert>
#include <iostream>

#include <PerlinNoise.hpp>

class CloudUpgrade
{
public:
	static void InstallPatches()
	{
		Patches::Install();
	}

	static CloudUpgrade* GetSingleton()
	{
		static CloudUpgrade singleton;
		return &singleton;
	}

	struct CloudSettings
	{
		siv::PerlinNoise noise;
		float            subSpeed;
		float            subVolatility;
	};

	bool          init = false;

	CloudSettings layers[4];
		
	siv::PerlinNoise noise;
	float            speed;
	float            maxSpeed;
	float            volatility;

	void UpdateCloud(int a_layer, float a_daysPassed, float a_noise);
	//void UpdateCloudSunriseSunset(int a_layer);
	void Update();

	static void   InstallHooks();

	struct Patches
	{
		static void Install()
		{
			InstallHooks();
		}
	};

	void LoadINI();

private:

	CloudUpgrade()
	{
		noise = siv::PerlinNoise{ 0 };
		speed = 0.0;
		maxSpeed = 0.0;
		volatility = 0.0;
		layers[0] = { siv::PerlinNoise{ 1 }, 0.0, 0.0 };
		layers[1] = { siv::PerlinNoise{ 2 }, 0.0, 0.0 };
		layers[2] = { siv::PerlinNoise{ 3 }, 0.0, 0.0 };
		layers[3] = { siv::PerlinNoise{ 4 }, 0.0, 0.0 };
		LoadINI();
	}
};
