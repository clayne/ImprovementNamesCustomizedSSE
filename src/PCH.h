#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <functional>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#pragma warning(push)
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>

#include <fmt/core.h>
#include <mmio/mmio.hpp>
#include <nlohmann/json.hpp>
#include <xbyak/xbyak.h>

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

using namespace std::literals;

namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"

#include "Settings.h"
