#include "Settings.h"


bool Settings::loadSettings(bool a_dumpParse)
{
	return Json2Settings::Settings::loadSettings(FILE_NAME, a_dumpParse);
}


decltype(Settings::style)		Settings::style("style", "PlusN");
decltype(Settings::prefix)		Settings::prefix("prefix", " [");
decltype(Settings::postfix)		Settings::postfix("postfix", "]");
decltype(Settings::customNames)	Settings::customNames("customNames");
