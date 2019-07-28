#include "TemperFactorManager.h"

#include "skse64_common/BranchTrampoline.h"

#include <cmath>
#include <cstdio>
#include <set>
#include <typeinfo>

#include "Settings.h"

#include "RE/Skyrim.h"
#include "REL/Relocation.h"


std::string TemperFactorManager::AsVanilla(UInt32 a_level, bool a_isWeapon)
{
	auto gameSettings = RE::GameSettingCollection::GetSingleton();
	switch (a_level) {
	case 1:
		if (a_isWeapon) {
			auto sHealthDataPrefixWeap1 = gameSettings->GetSetting("sHealthDataPrefixWeap1");
			return sHealthDataPrefixWeap1->GetString();
		} else {
			auto sHealthDataPrefixArmo1 = gameSettings->GetSetting("sHealthDataPrefixArmo1");
			return sHealthDataPrefixArmo1->GetString();
		}
	case 2:
		if (a_isWeapon) {
			auto sHealthDataPrefixWeap2 = gameSettings->GetSetting("sHealthDataPrefixWeap2");
			return sHealthDataPrefixWeap2->GetString();
		} else {
			auto sHealthDataPrefixArmo2 = gameSettings->GetSetting("sHealthDataPrefixArmo2");
			return sHealthDataPrefixArmo2->GetString();
		}
	case 3:
		if (a_isWeapon) {
			auto sHealthDataPrefixWeap3 = gameSettings->GetSetting("sHealthDataPrefixWeap3");
			return sHealthDataPrefixWeap3->GetString();
		} else {
			auto sHealthDataPrefixArmo3 = gameSettings->GetSetting("sHealthDataPrefixArmo3");
			return sHealthDataPrefixArmo3->GetString();
		}
	case 4:
		if (a_isWeapon) {
			auto sHealthDataPrefixWeap4 = gameSettings->GetSetting("sHealthDataPrefixWeap4");
			return sHealthDataPrefixWeap4->GetString();
		} else {
			auto sHealthDataPrefixArmo4 = gameSettings->GetSetting("sHealthDataPrefixArmo4");
			return sHealthDataPrefixArmo4->GetString();
		}
	case 5:
		if (a_isWeapon) {
			auto sHealthDataPrefixWeap5 = gameSettings->GetSetting("sHealthDataPrefixWeap5");
			return sHealthDataPrefixWeap5->GetString();
		} else {
			auto sHealthDataPrefixArmo5 = gameSettings->GetSetting("sHealthDataPrefixArmo5");
			return sHealthDataPrefixArmo5->GetString();
		}
	default:
		if (a_isWeapon) {
			auto sHealthDataPrefixWeap6 = gameSettings->GetSetting("sHealthDataPrefixWeap6");
			return sHealthDataPrefixWeap6->GetString();
		} else {
			auto sHealthDataPrefixArmo6 = gameSettings->GetSetting("sHealthDataPrefixArmo6");
			return sHealthDataPrefixArmo6->GetString();
		}
	}
}


std::string TemperFactorManager::AsVanillaPlus(UInt32 a_level, bool a_isWeapon)
{
	auto result = AsVanilla(a_level, a_isWeapon);
	if (a_level > 5) {
		result += " ";
		result += std::to_string(a_level - 5);
	}
	return result;
}


std::string TemperFactorManager::AsPlusN(UInt32 a_level, bool a_isWeapon)
{
	return std::string("+") + std::to_string(a_level);
}


std::string TemperFactorManager::AsInternal(UInt32 a_level, bool a_isWeapon)
{
	std::string result = std::to_string((a_level / 10) + 1);
	result += ".";
	result += std::to_string(a_level % 10);
	return result;
}


std::string TemperFactorManager::AsCustom(UInt32 a_level, bool a_isWeapon)
{
	std::size_t idx = a_level - 1;
	if (idx < Settings::customNames.size()) {
		return Settings::customNames[idx];
	} else {
		return Settings::customNames.empty() ? "" : Settings::customNames.back();
	}
}


std::string TemperFactorManager::AsRomanNumeral(UInt32 a_level, bool a_isWeapon)
{
	constexpr std::pair<UInt32, const char*> MILESTONES[] = {
		{ 1, "I" },
		{ 4, "IV" },
		{ 5, "V" },
		{ 9, "IX" },
		{ 10, "X" },
		{ 40, "XL" },
		{ 50, "L" },
		{ 100, "C" },
		{ 400, "CD" },
		{ 500, "D" },
		{ 900, "CM" },
		{ 1000, "M" }
	};

	constexpr std::size_t SIZE = std::extent<decltype(MILESTONES)>::value;

	std::string result;
	for (auto i = SIZE; a_level; --i) {
		auto div = a_level / MILESTONES[i].first;
		a_level = a_level % MILESTONES[i].first;
		while (div--) {
			result += MILESTONES[i].second;
		}
	}

	return result;
}


const char* TemperFactorManager::GetTemperFactor(float a_factor, bool a_isWeapon)
{
	if (a_factor <= 0.0 || a_factor == 1.0) {
		return 0;
	}

	a_factor -= 1.0;
	UInt32 level = static_cast<UInt32>(std::roundf(a_factor * 10.0));

	auto it = _stringCache.insert(_formatterMap(level, a_isWeapon));
	return it.first != _stringCache.end() ? it.first->c_str() : 0;
}


void TemperFactorManager::VFormat(RE::BSString* a_dst, const char* a_fmt, ...)
{
	std::string fmt = "%s";
	fmt += Settings::prefix;
	fmt += "%s";
	fmt += Settings::postfix;

	std::va_list args1;
	va_start(args1, a_fmt);
	std::va_list args2;
	va_copy(args2, args1);
	std::vector<char> buf(std::vsnprintf(0, 0, fmt.c_str(), args1) + 1);
	va_end(args1);
	std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args2);
	va_end(args2);

	std::string_view view(buf.data(), buf.size());
	*a_dst = view;
}


void TemperFactorManager::sprintf_s(char* a_buffer, std::size_t a_buffSize, const char* a_fmt, ...)
{
	std::string fmt = Settings::prefix;
	fmt += "%s";
	fmt += Settings::postfix;

	std::va_list args;
	va_start(args, a_fmt);
	std::vsnprintf(a_buffer, a_buffSize, fmt.c_str(), args);
	va_end(args);
}


void TemperFactorManager::InstallHooks()
{
	// E8 ? ? ? ? F3 0F 11 73 2C
	constexpr std::uintptr_t FUNC_ADDR = 0x0013CC20;	// 1_5_80
	REL::Offset<std::uintptr_t> funcBase(FUNC_ADDR);

	g_branchTrampoline.Write5Call(funcBase.GetAddress() + 0x59, unrestricted_cast<std::uintptr_t>(&TemperFactorManager::GetTemperFactor));
	g_branchTrampoline.Write5Call(funcBase.GetAddress() + 0x1A6, unrestricted_cast<std::uintptr_t>(&TemperFactorManager::VFormat));
	g_branchTrampoline.Write5Call(funcBase.GetAddress() + 0x177, unrestricted_cast<std::uintptr_t>(&TemperFactorManager::VFormat));
	g_branchTrampoline.Write5Call(funcBase.GetAddress() + 0x13D, unrestricted_cast<std::uintptr_t>(&TemperFactorManager::sprintf_s));

	_MESSAGE("Installed hooks for %s", typeid(TemperFactorManager).name());
}


TemperFactorManager::FormatterMap::FormatterMap() :
	_map()
{
	_map.insert({ "Vanilla", TemperFactorManager::AsVanilla });
	_map.insert({ "VanillaPlus", TemperFactorManager::AsVanillaPlus });
	_map.insert({ "PlusN", TemperFactorManager::AsPlusN });
	_map.insert({ "Internal", TemperFactorManager::AsInternal });
	_map.insert({ "Custom", TemperFactorManager::AsCustom });
	_map.insert({ "RomanNumeral", TemperFactorManager::AsRomanNumeral });
}


std::string TemperFactorManager::FormatterMap::operator()(UInt32 a_factor, bool a_isWeapon)
{
	auto it = _map.find(Settings::style);
	if (it != _map.end()) {
		return it->second(a_factor, a_isWeapon);
	} else {
		return TemperFactorManager::AsVanilla(a_factor, a_isWeapon);
	}
}


decltype(TemperFactorManager::_stringCache) TemperFactorManager::_stringCache;
decltype(TemperFactorManager::_formatterMap) TemperFactorManager::_formatterMap;
