#define NOMINMAX
#include <ShlObj.h>
#define ENABLE_CONFIG 1
#include "Systems/AppData.h"
#include "Utility/StringUtils.h"

static std::filesystem::path GetLocalAppDataFolder(const std::string_view& appName)
{
	std::filesystem::path AppDataFolder;

	wchar_t* folderPath = nullptr;
	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &folderPath) == S_OK)
	{
		AppDataFolder = std::filesystem::path(folderPath) / StringUtils::Utf8ToWideString(appName);

		std::error_code ec;
		std::filesystem::create_directory(AppDataFolder, ec);
	}
	CoTaskMemFree(folderPath);

	return AppDataFolder;
}

static std::filesystem::path AppDataFolder;

void AppData::SetAppDataName(const std::string_view& appName, bool portable)
{
	if (portable)
	{
		auto cstr = appName.empty() ? "AppData" : appName;
		AppDataFolder = std::filesystem::current_path() / StringUtils::Utf8ToWideString(cstr);

		std::error_code ec;
		std::filesystem::create_directory(AppDataFolder, ec);
		return;
	}

	auto cstr = appName.empty() ? "GhostFries" : appName;
	AppDataFolder = GetLocalAppDataFolder(cstr);
}

std::filesystem::path AppData::GetAppDataFolder()
{
	return AppDataFolder;
}

bool AppData::LoadConfig(YamlDoc& config, const std::wstring_view& file)
{
	auto path = AppDataFolder / file;
	return config.Load(path.native());
}
