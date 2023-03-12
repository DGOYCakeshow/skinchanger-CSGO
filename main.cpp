#include "memory.h"
#include <thread>
#include <array>
namespace offsets
{
	//клиент
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA964;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFFEF4;
	constexpr ::std::ptrdiff_t dwClientState = 0x59F19C;


	//игрок
	constexpr ::std::ptrdiff_t m_hMyWeapons = 0x2E08;

	//атрибуты

	constexpr ::std::ptrdiff_t m_nFallbackPaintKit = 0x31D8;
	constexpr ::std::ptrdiff_t m_nFallbackSeed = 0x31DC;
	constexpr ::std::ptrdiff_t m_nFallbackStatTrak = 0x31E4;
	constexpr ::std::ptrdiff_t m_flFallbackWear = 0x31E0;
	constexpr ::std::ptrdiff_t m_iItemDefinitionIndex = 0x2FBA;
	constexpr ::std::ptrdiff_t m_iItemIDHigh = 0x2FD0;
	constexpr ::std::ptrdiff_t m_iEntityQuality = 0x2FBC;
	constexpr ::std::ptrdiff_t m_iAccountID = 0x2FD8;
	constexpr ::std::ptrdiff_t m_OriginalOwnerXuidLow = 0x31D0;
}


constexpr const int GetWeaponPaint(const short& itemDefenition)
{
	switch (itemDefenition)
	{
	//deagle blaze
	case 1:  return 37;

	//awp lightinig strike
	case 9:  return 51;

	//AK-47 vulcan
	case 7:  return 302;

	//SSG 08 Dragonfire
	case 40: return 624;

	//M4A1 Howl
	case 16: return 309;

	//Glock-18 Franklin
	case 4:  return 295;

	//USP-S Whiteout
	case 61: return 102;

	default: return 0;
	}
}

int main()
{
	const auto memory = Memory {"csgo.exe"};


	const auto& client = memory.GetModuleAddress("client.dll");
	const auto& engine = memory.GetModuleAddress("engine.dll");



	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		const auto& localPlayer = memory.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& weapons = memory.Read<std::array<unsigned long, 8>>(localPlayer + offsets::m_hMyWeapons);

		for (const auto& handle : weapons)
		{
			const auto& weapon = memory.Read<uintptr_t>((client + offsets::dwEntityList + (handle & 0xFFF) * 0x10));
			//правильное ли оружие
			if (!weapon)
				continue;


			//проверка нужно ли менять скин
			if (const auto paint = GetWeaponPaint(memory.Read<short>(weapon + offsets::m_iItemDefinitionIndex)))
			{
				//заставляем использовать резервные значения
				memory.Write<std::int32_t>(weapon + offsets::m_iItemIDHigh, -1);

				memory.Write<std::int32_t>(weapon + offsets::m_nFallbackPaintKit, paint);
				memory.Write<float>(weapon + offsets::m_flFallbackWear, 0.1f);




				const bool shouldUpdate = memory.Read<std::int32_t>(weapon + offsets::m_nFallbackPaintKit) != paint;
				if (shouldUpdate)
					memory.Write<std::int32_t>(memory.Read<std::uintptr_t>(engine + offsets::dwClientState) + 0x174, -1);
			}

		}

		
	}
	return 0;
}