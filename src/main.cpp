﻿#include "Settings.h"
#include "TemperFactorManager.h"
#include "version.h"

#include "SKSE/API.h"


extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\ImprovementNamesCustomizedSSE.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);
		SKSE::Logger::TrackTrampolineStats(true);

		_MESSAGE("ImprovementNamesCustomizedSSE v%s", IMPR_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "ImprovementNamesCustomizedSSE";
		a_info->version = IMPR_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!");
			return false;
		}

		auto ver = a_skse->RuntimeVersion();
		if (ver <= SKSE::RUNTIME_1_5_39) {
			_FATALERROR("Unsupported runtime version %s!", ver.GetString().c_str());
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		_MESSAGE("ImprovementNamesCustomizedSSE loaded");

		if (!SKSE::Init(a_skse)) {
			return false;
		}

		if (Settings::loadSettings()) {
			_MESSAGE("Settings successfully loaded");
		} else {
			_FATALERROR("Settings failed to load!");
			return false;
		}

		if (!SKSE::AllocTrampoline(1 << 6)) {
			return false;
		}

		TemperFactorManager::InstallHooks();

		return true;
	}
};
