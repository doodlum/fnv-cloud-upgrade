#include <cmath>
#include <cassert>
#include <iostream>

#include <PerlinNoise.hpp>

class CloudsUpgrade
{
public:
	static void InstallPatches()
	{
		Patches::Install();
	}

	static CloudsUpgrade* GetSingleton()
	{
		static CloudsUpgrade control;
		return &control;
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

	CloudsUpgrade() {
		noise = siv::PerlinNoise{ 0 };
		speed = 500.0;
		maxSpeed = 2.5;
		volatility = 5.0;
		layers[0] = { siv::PerlinNoise{ 1 }, 0.2, 0.2 };
		layers[1] = { siv::PerlinNoise{ 2 }, 1.0, 0.3 };
		layers[2] = { siv::PerlinNoise{ 3 }, 0.7, 0.4 };
		layers[3] = { siv::PerlinNoise{ 4 }, 0.3, 0.5 };
		LoadINI();
	}
};
