#include "skse64_common/BranchTrampoline.h"
#include "skse64_common/skse_version.h"

#include "Settings.h"
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

		_MESSAGE("ImprovementNamesCustomizedSSE v%s", IMPR_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "ImprovementNamesCustomizedSSE";
		a_info->version = IMPR_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		switch (a_skse->RuntimeVersion()) {
		case RUNTIME_VERSION_1_5_73:
		case RUNTIME_VERSION_1_5_80:
			break;
		default:
			_FATALERROR("Unsupported runtime version %08X!\n", a_skse->RuntimeVersion());
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
			_FATALERROR("Settings failed to load!\n");
			return false;
		}

		if (g_branchTrampoline.Create(1024 * 1)) {
			_MESSAGE("Branch trmapoline creation successful");
		} else {
			_FATALERROR("Branch trmapoline creation failed!\n");
			return false;
		}

		TemperFactorManager::InstallHooks();

		return true;
	}
};
