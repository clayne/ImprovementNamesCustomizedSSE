#pragma once

namespace Settings
{
	inline std::string style = "PlusN"s;
	inline std::string prefix = " ["s;
	inline std::string postfix = "]"s;
	inline std::vector<std::string> customNames;

	inline void LoadSettings() noexcept
	{
		try {
			const auto f = mmio::mapped_file_source("Data/SKSE/Plugins/ImprovementNamesCustomizedSSE.json"sv);
			const auto j = nlohmann::json::parse(f.data(), f.data() + f.size());

#define GET(a_value) Settings::a_value = j.at(#a_value).get<std::decay_t<decltype(a_value)>>()
			GET(style);
			GET(prefix);
			GET(postfix);
			GET(customNames);
#undef GET
		} catch (const std::exception& a_error) {
			util::report_and_fail(fmt::format("failed to parse settings file\n{}"sv, a_error.what()));
		}
	}
}
