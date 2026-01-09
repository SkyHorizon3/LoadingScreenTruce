
struct CombatAIHook
{
	static bool thunk(RE::Actor* a_target)
	{
		auto result = func(a_target);
		if (!result) {
			const auto ui = RE::UI::GetSingleton();
			if (ui && (ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME) || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME))) {
				return true;
			}
		}

		return result;
	};
	static inline REL::Relocation<decltype(thunk)> func;

	static void Install()
	{
		REL::Relocation<std::uintptr_t> callTarget{ RELOCATION_ID(32469, 33216), REL::Relocate(0xFF, 0xEC) };
		stl::write_thunk_call<CombatAIHook>(callTarget.address());
	}
};

#define DLLEXPORT __declspec(dllexport)

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
	{
		SKSE::PluginVersionData v;
		v.PluginName(Plugin::NAME);
		v.AuthorName("Maxsu SkyHorizon"sv);
		v.PluginVersion(Plugin::VERSION);
		v.UsesAddressLibrary();
		v.UsesNoStructs();
		return v;
	}
();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse, true);

	spdlog::set_pattern("[%H:%M:%S:%e] [%l] %v"s);
	spdlog::set_level(spdlog::level::info);
	spdlog::flush_on(spdlog::level::info);

	SKSE::log::info("Game version: {}", skse->RuntimeVersion());

	SKSE::AllocTrampoline(14);
	CombatAIHook::Install();

	return true;
}