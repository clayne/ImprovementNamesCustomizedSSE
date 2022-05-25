#include "TemperFactorManager.h"

namespace TemperFactorManager
{
	namespace
	{
		namespace Styles
		{
			class GMSTCache
			{
			public:
				GMSTCache()
				{
					constexpr std::array names = {
						"sHealthDataPrefixWeap1",
						"sHealthDataPrefixArmo1",
						"sHealthDataPrefixWeap2",
						"sHealthDataPrefixArmo2",
						"sHealthDataPrefixWeap3",
						"sHealthDataPrefixArmo3",
						"sHealthDataPrefixWeap4",
						"sHealthDataPrefixArmo4",
						"sHealthDataPrefixWeap5",
						"sHealthDataPrefixArmo5",
						"sHealthDataPrefixWeap6",
						"sHealthDataPrefixArmo6"
					};

					for (const auto& name : names) {
						this->Insert(name);
					}
				}

				[[nodiscard]] auto operator()(std::string_view a_name)
					-> std::string
				{
					const auto it = this->_map.find(a_name);
					return it != this->_map.end() ? it->second->GetString() : "";
				}

			private:
				void Insert(const char* a_name)
				{
					const auto gmst = RE::GameSettingCollection::GetSingleton();
					const auto setting = gmst->GetSetting(a_name);
					if (setting) {
						auto it = this->_map.emplace(a_name, setting);
						assert(it.second);
					} else {
						assert(false);
					}
				}

				std::map<std::string_view, RE::Setting*> _map;
			};

			[[nodiscard]] inline auto AsVanilla(
				std::uint32_t a_level,
				bool a_isWeapon)
				-> std::string
			{
				static GMSTCache cache;
				return cache(fmt::format(
					"sHealthDataPrefix{}{}"sv,
					a_isWeapon ? "Weap"sv : "Armo"sv,
					1 <= a_level && a_level <= 5 ? a_level : 6));
			}

			[[nodiscard]] inline auto AsVanillaPlus(
				std::uint32_t a_level,
				bool a_isWeapon)
				-> std::string
			{
				auto vanilla = AsVanilla(a_level, a_isWeapon);
				if (a_level > 5) {
					return fmt::format("{} {}"sv, vanilla, a_level - 5);
				} else {
					return vanilla;
				}
			}

			[[nodiscard]] inline auto AsPlusN(
				std::uint32_t a_level,
				[[maybe_unused]] bool a_isWeapon)
				-> std::string
			{
				return fmt::format("+{}"sv, a_level);
			}

			[[nodiscard]] inline auto AsInternal(
				std::uint32_t a_level,
				[[maybe_unused]] bool a_isWeapon)
				-> std::string
			{
				return fmt::format(
					"{}.{}"sv,
					(a_level / 10) + 1,
					a_level % 10);
			}

			[[nodiscard]] inline auto AsCustom(
				std::uint32_t a_level,
				[[maybe_unused]] bool a_isWeapon)
				-> std::string
			{
				const std::size_t idx = a_level - 1;
				if (idx < Settings::customNames.size()) {
					return Settings::customNames[idx];
				} else {
					return Settings::customNames.empty() ? ""s : Settings::customNames.back();
				}
			}

			[[nodiscard]] inline auto AsRomanNumeral(
				std::uint32_t a_level,
				[[maybe_unused]] bool a_isWeapon)
				-> std::string
			{
				constexpr std::array milestones = {
					std::make_pair(1, "I"sv),
					std::make_pair(4, "IV"sv),
					std::make_pair(5, "V"sv),
					std::make_pair(9, "IX"sv),
					std::make_pair(10, "X"sv),
					std::make_pair(40, "XL"sv),
					std::make_pair(50, "L"sv),
					std::make_pair(100, "C"sv),
					std::make_pair(400, "CD"sv),
					std::make_pair(500, "D"sv),
					std::make_pair(900, "CM"sv),
					std::make_pair(1000, "M"sv)
				};

				std::string result;
				for (const auto& [i, str] : milestones | std::views::reverse) {
					auto div = a_level / i;
					a_level = a_level % i;
					while (div--) {
						result.append(str);
					}
				}

				return result;
			}
		}

		namespace Hooks
		{
			class FormatterMap
			{
			public:
				FormatterMap()
				{
					this->_map.insert({ "Custom"sv, Styles::AsCustom });
					this->_map.insert({ "Internal"sv, Styles::AsInternal });
					this->_map.insert({ "PlusN"sv, Styles::AsPlusN });
					this->_map.insert({ "RomanNumeral"sv, Styles::AsRomanNumeral });
					this->_map.insert({ "Vanilla"sv, Styles::AsVanilla });
					this->_map.insert({ "VanillaPlus"sv, Styles::AsVanillaPlus });
				}

				[[nodiscard]] auto operator()(
					std::uint32_t a_factor,
					bool a_isWeapon)
					-> std::string
				{
					const auto it = this->_map.find(Settings::style);
					if (it != this->_map.end()) {
						return it->second(a_factor, a_isWeapon);
					} else {
						return Styles::AsVanilla(a_factor, a_isWeapon);
					}
				}

			private:
				std::map<std::string_view, std::function<std::string(std::uint32_t, bool)>> _map;
			};

			inline auto GetTemperFactor(
				float a_factor,
				bool a_isWeapon)
				-> const char*
			{
				static std::set<std::string> cache;
				static FormatterMap map;

				const auto level = std::roundf((a_factor - 1.0f) * 10.0f);
				if (level < 1.0) {
					return 0;
				}

				const auto it = cache.insert(map(static_cast<std::uint32_t>(level), a_isWeapon));
				return it.first != cache.end() ? it.first->c_str() : nullptr;
			}

			inline void VFormat(
				RE::BSString& a_dst,
				const char* a_fmt,
				...)
			{
				const auto format = fmt::format("%s{}%s{}"sv, Settings::prefix, Settings::postfix);

				std::va_list args1;
				va_start(args1, a_fmt);
				std::va_list args2;
				va_copy(args2, args1);
				std::vector<char> buf(std::vsnprintf(nullptr, 0, format.c_str(), args1) + 1);
				va_end(args1);
				std::vsnprintf(buf.data(), buf.size(), format.c_str(), args2);
				va_end(args2);

				a_dst = std::string_view(buf.data(), buf.size());
			}

			inline void sprintf_s(
				char* a_buffer,
				std::size_t a_buffSize,
				const char* a_fmt,
				...)
			{
				const auto format = fmt::format("{}%s{}"sv, Settings::prefix, Settings::postfix);

				std::va_list args;
				va_start(args, a_fmt);
				std::vsnprintf(a_buffer, a_buffSize, format.c_str(), args);
				va_end(args);
			}
		}

		struct GetTemperFactorPatch :
			public Xbyak::CodeGenerator
		{
			GetTemperFactorPatch()
			{
				push(r9);       // volatile
				sub(rsp, 0x8);  // alignment

				movaps(xmm0, xmm6);
				mov(dl, r9b);
				mov(rax, reinterpret_cast<std::uintptr_t>(Hooks::GetTemperFactor));
				call(rax);
				mov(rbx, rax);  // temper factor string

				add(rsp, 0x8);
				pop(r9);
			}
		};
	}

	void Install()
	{
		const auto base = REL::ID(12775).address();  // ExtraTextDisplayData::BuildDisplayName

		auto& trampoline = SKSE::GetTrampoline();

		{
			constexpr std::size_t begin = 0x55;
			constexpr std::size_t end = 0x149;

			REL::make_pattern<"48 8B DD 0F 57 C0 0F 2F F0 0F 86 ?? ?? ?? ?? 0F 2E 35 ?? ?? ?? ?? 0F 84 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 0F 2F 70 08 72 17 45 84 C9 48 8B 05 ?? ?? ?? ?? 75 07 48 8B 05 ?? ?? ?? ?? 48 8B 58 08 48 8B 05 ?? ?? ?? ?? 0F 2F 70 08 72 17 45 84 C9 48 8B 05 ?? ?? ?? ?? 75 07 48 8B 05 ?? ?? ?? ?? 48 8B 58 08 48 8B 05 ?? ?? ?? ?? 0F 2F 70 08 72 17 45 84 C9 48 8B 05 ?? ?? ?? ?? 75 07 48 8B 05 ?? ?? ?? ?? 48 8B 58 08 48 8B 05 ?? ?? ?? ?? 0F 2F 70 08 72 17 45 84 C9 48 8B 05 ?? ?? ?? ?? 75 07 48 8B 05 ?? ?? ?? ?? 48 8B 58 08 48 8B 05 ?? ?? ?? ?? 0F 2F 70 08 72 17 45 84 C9 48 8B 05 ?? ?? ?? ?? 75 07 48 8B 05 ?? ?? ?? ?? 48 8B 58 08 48 8B 05 ?? ?? ?? ?? 0F 2F 70 08 72 17 45 84 C9 48 8B 05 ?? ?? ?? ?? 75 07 48 8B 05 ?? ?? ?? ?? 48 8B 58 08">()
				.match_or_fail(base + begin);

			GetTemperFactorPatch p;
			p.ready();

			REL::safe_fill(base + begin, REL::NOP, end - begin);
			REL::safe_write(base + begin, std::span(p.getCode<const std::byte*>(), p.getSize()));
		}

		REL::make_pattern<"48 85 DB 0F 84 ?? ?? ?? ?? 80 3B 00 0F 84 ?? ?? ?? ?? 4C 8B CB 4C 8D 05 ?? ?? ?? ?? BA ?? ?? ?? ?? 48 8D 4C 24 ?? E8 ?? ?? ?? ?? 48 8D 54 24 ?? 48 8B CE E8 ?? ?? ?? ?? EB 79">()
			.match_or_fail(base + 0x1D0);
		trampoline.write_call<5>(base + 0x1F6, Hooks::sprintf_s);

		REL::make_pattern<"48 83 7F ?? ?? 75 29 83 7F 28 FE 75 23 48 8B CE 48 85 DB 74 15 4C 8B CB 4C 8B 47 10 48 8D 15 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 3A">()
			.match_or_fail(base + 0x20A);
		trampoline.write_call<5>(base + 0x22D, Hooks::VFormat);

		REL::make_pattern<"4D 85 F6 74 2F 49 8B CE E8 ?? ?? ?? ?? 48 8B CE 48 85 DB 74 14 4C 8B C0 4C 8B CB 48 8D 15 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 0B">()
			.match_or_fail(base + 0x23A);
		trampoline.write_call<5>(base + 0x25C, Hooks::VFormat);

		logger::debug("installed hooks for temper factor manager");
	}
}
