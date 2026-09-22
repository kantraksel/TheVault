#pragma once
#include <filesystem>
#if ENABLE_CONFIG
	#include "YamlDoc.h"
#endif

namespace AppData
{
	// do NOT use - reserved for engine
	void SetAppDataName(const std::string_view& appName, bool portable);

	// returns %USERPROFILE%/Documents/{GameName}
	// use path or wstring - do NOT use string
	// path may contain Unicode chars
	std::filesystem::path GetAppDataFolder();

#if ENABLE_CONFIG
	bool LoadConfig(YamlDoc& config, const std::wstring_view& file);
#endif
}
