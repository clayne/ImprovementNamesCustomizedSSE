#pragma once

#include "Json2Settings.h"


class Settings : public Json2Settings::Settings
{
public:
	Settings() = delete;
	static bool loadSettings(bool a_dumpParse = false);


	static sSetting					style;
	static sSetting					prefix;
	static sSetting					postfix;
	static aSetting<std::string>	customNames;

private:
	static inline constexpr char FILE_NAME[] = "Data\\SKSE\\Plugins\\ImprovementNamesCustomizedSSE.json";
};
