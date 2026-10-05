#include <RE/Skyrim.h>
#include <RE/N/NativeFunction.h>
#include <SKSE/SKSE.h>

#include <Windows.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string>
#include <string_view>

namespace
{
	constexpr auto PLUGIN_NAME = "DynamicUnrelentingForce";
	constexpr auto OUR_INI = ".\\Data\\SKSE\\Plugins\\DynamicUnrelentingForce.ini";

	struct Settings
	{
		bool enable{ true };
		bool scaleUnrelentingForcePush{ true };
		bool requirePlayerCaster{ true };
		bool countSpentSouls{ true };
		bool debugLogging{ false };

		std::uint32_t maxDragonSouls{ 50 };

		float minDistanceMultiplier{ 1.0F };
		float distanceMultiplier{ 0.04F };
		float minPushForce{ 15.0F };
		float maxPushForce{ 100.0F };
	};

	Settings g_settings;
	bool g_settingsLoaded{ false };
	std::atomic_uint32_t g_pushLogCount{ 0 };

	bool FileExists(const char* a_path)
	{
		const auto attributes = GetFileAttributesA(a_path);
		return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	std::string Trim(std::string_view a_value)
	{
		auto first = a_value.begin();
		auto last = a_value.end();

		while (first != last && std::isspace(static_cast<unsigned char>(*first))) {
			++first;
		}

		while (first != last && std::isspace(static_cast<unsigned char>(*(last - 1)))) {
			--last;
		}

		return { first, last };
	}

	std::string ReadString(const char* a_path, const char* a_section, const char* a_key, std::string_view a_default)
	{
		char buffer[256]{};
		GetPrivateProfileStringA(
			a_section,
			a_key,
			std::string(a_default).c_str(),
			buffer,
			static_cast<DWORD>(sizeof(buffer)),
			a_path);
		return Trim(buffer);
	}

	float ParseFloat(const std::string& a_value, float a_default)
	{
		const auto trimmed = Trim(a_value);
		if (trimmed.empty()) {
			return a_default;
		}

		char* end = nullptr;
		errno = 0;
		const float parsed = std::strtof(trimmed.c_str(), &end);
		if (end == trimmed.c_str() || errno == ERANGE || !std::isfinite(parsed)) {
			return a_default;
		}

		return parsed;
	}

	std::uint32_t ParseUInt(const std::string& a_value, std::uint32_t a_default)
	{
		const auto trimmed = Trim(a_value);
		if (trimmed.empty()) {
			return a_default;
		}

		char* end = nullptr;
		errno = 0;
		const auto parsed = std::strtoul(trimmed.c_str(), &end, 0);
		if (end == trimmed.c_str() || errno == ERANGE || parsed > std::numeric_limits<std::uint32_t>::max()) {
			return a_default;
		}

		return static_cast<std::uint32_t>(parsed);
	}

	bool ParseBool(const std::string& a_value, bool a_default)
	{
		auto trimmed = Trim(a_value);
		std::ranges::transform(trimmed, trimmed.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});

		if (trimmed == "1" || trimmed == "true" || trimmed == "yes" || trimmed == "on") {
			return true;
		}

		if (trimmed == "0" || trimmed == "false" || trimmed == "no" || trimmed == "off") {
			return false;
		}

		return a_default;
	}

	float ReadFloat(const char* a_path, const char* a_section, const char* a_key, float a_default)
	{
		return ParseFloat(ReadString(a_path, a_section, a_key, std::to_string(a_default)), a_default);
	}

	std::uint32_t ReadUInt(const char* a_path, const char* a_section, const char* a_key, std::uint32_t a_default)
	{
		return ParseUInt(ReadString(a_path, a_section, a_key, std::to_string(a_default)), a_default);
	}

	bool ReadBool(const char* a_path, const char* a_section, const char* a_key, bool a_default)
	{
		return ParseBool(ReadString(a_path, a_section, a_key, a_default ? "1" : "0"), a_default);
	}

	void ApplyOwnSettings(const char* a_path, Settings& a_settings)
	{
		if (!FileExists(a_path)) {
			return;
		}

		a_settings.enable = ReadBool(a_path, "General", "bEnable", a_settings.enable);
		a_settings.debugLogging = ReadBool(a_path, "General", "bEnableDebugLogging", a_settings.debugLogging);
		a_settings.countSpentSouls = ReadBool(a_path, "General", "bCountSpentSouls", a_settings.countSpentSouls);

		a_settings.distanceMultiplier = ReadFloat(a_path, "Scaling", "fDistanceMultiplier", a_settings.distanceMultiplier);
		a_settings.minDistanceMultiplier = ReadFloat(a_path, "Scaling", "fMinDistanceMultiplier", a_settings.minDistanceMultiplier);
		a_settings.maxDragonSouls = ReadUInt(a_path, "Scaling", "iMaxDragonSouls", a_settings.maxDragonSouls);

		a_settings.scaleUnrelentingForcePush = ReadBool(a_path, "UnrelentingForce", "bScalePush", a_settings.scaleUnrelentingForcePush);
		a_settings.requirePlayerCaster = ReadBool(a_path, "UnrelentingForce", "bRequirePlayerCaster", a_settings.requirePlayerCaster);
		a_settings.minPushForce = ReadFloat(a_path, "UnrelentingForce", "fMinPushForce", a_settings.minPushForce);
		a_settings.maxPushForce = ReadFloat(a_path, "UnrelentingForce", "fMaxPushForce", a_settings.maxPushForce);
	}

	void SanitizeSettings(Settings& a_settings)
	{
		a_settings.maxDragonSouls = std::min(a_settings.maxDragonSouls, 10000U);
		a_settings.distanceMultiplier = std::max(0.0F, a_settings.distanceMultiplier);
		a_settings.minPushForce = std::clamp(a_settings.minPushForce, 0.0F, 1000.0F);
		a_settings.maxPushForce = std::clamp(a_settings.maxPushForce, a_settings.minPushForce, 1000.0F);
	}

	void LoadSettings()
	{
		Settings settings;
		ApplyOwnSettings(OUR_INI, settings);
		SanitizeSettings(settings);
		g_settings = settings;

		spdlog::set_level(g_settings.debugLogging ? spdlog::level::debug : spdlog::level::info);
		SKSE::log::info(
			"Settings: enable={}, minDistance={}, distancePerSoul={}, maxSouls={}, countSpent={}, minPush={}, maxPush={}",
			g_settings.enable,
			g_settings.minDistanceMultiplier,
			g_settings.distanceMultiplier,
			g_settings.maxDragonSouls,
			g_settings.countSpentSouls,
			g_settings.minPushForce,
			g_settings.maxPushForce);
		g_settingsLoaded = true;
	}

	std::uint32_t CountKnownShoutWords(RE::PlayerCharacter* a_player)
	{
		if (!a_player) {
			return 0;
		}

		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) {
			return 0;
		}

		std::uint32_t knownWords = 0;
		for (auto* shout : dataHandler->GetFormArray<RE::TESShout>()) {
			if (!shout || !a_player->HasShout(shout)) {
				continue;
			}

			for (const auto& variation : shout->variations) {
				if (variation.word && variation.word->GetKnown()) {
					++knownWords;
				}
			}
		}

		return knownWords;
	}

	std::uint32_t GetDragonSoulCountForScaling()
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return 0;
		}

		auto* actorValueOwner = player->AsActorValueOwner();
		const auto unspentSouls = actorValueOwner ? actorValueOwner->GetActorValue(RE::ActorValue::kDragonSouls) : 0.0F;
		const auto safeUnspentSouls = static_cast<std::uint32_t>(std::max(0.0F, std::floor(unspentSouls)));

		if (!g_settings.countSpentSouls) {
			return safeUnspentSouls;
		}

		return safeUnspentSouls + CountKnownShoutWords(player);
	}

	float ResolveDistanceMultiplier(std::uint32_t a_souls)
	{
		const auto souls = std::min(a_souls, g_settings.maxDragonSouls);
		const float multiplier = g_settings.minDistanceMultiplier + static_cast<float>(souls) * g_settings.distanceMultiplier;
		if (!std::isfinite(multiplier)) {
			return 1.0F;
		}

		return std::max(0.0F, multiplier);
	}

	float ResolveSoulProgress()
	{
		const auto souls = std::min(GetDragonSoulCountForScaling(), g_settings.maxDragonSouls);
		if (g_settings.maxDragonSouls == 0) {
			return 1.0F;
		}

		const float minMultiplier = g_settings.minDistanceMultiplier;
		const float maxMultiplier = ResolveDistanceMultiplier(g_settings.maxDragonSouls);
		const float currentMultiplier = ResolveDistanceMultiplier(souls);
		const float multiplierRange = maxMultiplier - minMultiplier;
		if (std::isfinite(multiplierRange) && multiplierRange > 0.0001F) {
			return std::clamp((currentMultiplier - minMultiplier) / multiplierRange, 0.0F, 1.0F);
		}

		return std::clamp(static_cast<float>(souls) / static_cast<float>(g_settings.maxDragonSouls), 0.0F, 1.0F);
	}

	float ResolvePushForce()
	{
		const float progress = ResolveSoulProgress();
		const float force = g_settings.minPushForce + (g_settings.maxPushForce - g_settings.minPushForce) * progress;
		if (!std::isfinite(force)) {
			return 0.0F;
		}

		return std::clamp(force, 0.0F, 1000.0F);
	}

	bool IsPlayerRef(RE::TESObjectREFR* a_ref)
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		return player && a_ref == player;
	}

	float GetScaledPushForce(RE::StaticFunctionTag*, float a_vanillaPushForce, RE::Actor* a_caster)
	{
		if (!g_settingsLoaded) {
			LoadSettings();
		}

		if (!g_settings.enable || !g_settings.scaleUnrelentingForcePush) {
			return a_vanillaPushForce;
		}

		if (g_settings.requirePlayerCaster && !IsPlayerRef(a_caster)) {
			return a_vanillaPushForce;
		}

		const auto newForce = ResolvePushForce();
		if (newForce <= 0.0F || !std::isfinite(newForce)) {
			return a_vanillaPushForce;
		}

		const auto logIndex = g_pushLogCount.fetch_add(1, std::memory_order_relaxed);
		if (logIndex < 10) {
			const auto souls = std::min(GetDragonSoulCountForScaling(), g_settings.maxDragonSouls);
			SKSE::log::info(
				"VoicePushEffectScript scaled PushForce {} -> {} using souls={}/{}",
				a_vanillaPushForce,
				newForce,
				souls,
				g_settings.maxDragonSouls);
		}
		SKSE::log::debug("Scaled VoicePushEffectScript PushForce {} -> {}", a_vanillaPushForce, newForce);
		return newForce;
	}

	void OnDataLoaded()
	{
		LoadSettings();
	}

	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg && a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			OnDataLoaded();
		}
	}

	void SetupLog()
	{
		auto path = SKSE::log::log_directory();
		if (!path) {
			return;
		}

		*path /= "DynamicUnrelentingForce.log";
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
		auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
	}

	bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* a_vm)
	{
		if (!a_vm) {
			return false;
		}

		a_vm->RegisterFunction("GetScaledPushForce", PLUGIN_NAME, GetScaledPushForce, true);
		SKSE::log::info("Registered Papyrus native functions");
		return true;
	}
}

SKSEPluginVersion = []() constexpr {
	SKSE::PluginVersionData data;
	data.PluginVersion({ 1, 1, 1, 0 });
	data.PluginName(PLUGIN_NAME);
	data.AuthorName("dickmna");
	data.UsesAddressLibrary();
	data.UsesUpdatedStructs();
	data.CompatibleVersions({ { 1, 7, 104, 0 } });
	data.MinimumRequiredXSEVersion({ 2, 3, 1, 0 });
	return data;
}();

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	if (!a_skse || a_skse->IsEditor() || a_skse->RuntimeVersion() != REL::Version{ 1, 7, 104, 0 }) {
		return false;
	}

	SKSE::Init(a_skse);
	SetupLog();
	SKSE::log::info("{} v1.1.1 loaded for Skyrim 1.7.104", PLUGIN_NAME);
	LoadSettings();

	const auto papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus || !papyrus->Register(RegisterPapyrusFunctions)) {
		SKSE::log::error("Failed to register Papyrus functions");
		return false;
	}

	const auto messaging = SKSE::GetMessagingInterface();
	if (!messaging || !messaging->RegisterListener(MessageHandler)) {
		SKSE::log::error("Failed to register SKSE messaging listener");
		return false;
	}

	return true;
}
