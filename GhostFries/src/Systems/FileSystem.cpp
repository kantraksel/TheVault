#include <filesystem>
#include "Systems/FileSystem.h"
#include "Files/DirectStream.h"
#include "Systems/YamlDoc.h"
#include "Engine/Logger.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

FileSystem* FileSystem::mGlobalFS = nullptr;

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
	Shutdown();
}

bool FileSystem::Initialize(YamlDoc& doc)
{
	auto root = doc["mounts"];
	root.SetMap();

	return true;
}

void FileSystem::Shutdown()
{
	mDirectStreams.clear();

	if (mGlobalFS == this)
		mGlobalFS = nullptr;
}

FSStream* FileSystem::OpenFile(const std::string_view& name)
{
	if (std::filesystem::exists(name) && std::filesystem::is_regular_file(name))
	{
		// WinAPI needs null-terminated string. std::string_view cannot guarantee it
		std::string str(name);

		auto hFile = CreateFileA(str.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Logger::LogWarn("[FS] Cannot open {} (direct) - ACCESS DENIED", name);
			return nullptr;
		}

		for (auto& stream : mDirectStreams)
		{
			if (!stream.IsOpen())
			{
				stream.Open(hFile);
				return &stream;
			}
		}

		auto& stream = mDirectStreams.emplace_back();
		stream.Open(hFile);
		return &stream;
	}

	Logger::LogDebug("[FS] File {} not found in any mount point", name);
	return nullptr;
}

FSStream* FileSystem::OpenFileWrite(const std::string_view& name)
{
	if (std::filesystem::exists(name) && !std::filesystem::is_regular_file(name))
		return nullptr;

	// WinAPI needs null-terminated string. std::string_view cannot guarantee it
	std::string str(name);

	auto hFile = CreateFileA(str.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Logger::LogWarn("[FS] Cannot open {} (direct) - ACCESS DENIED", name);
		return nullptr;
	}

	for (auto& stream : mDirectStreams)
	{
		if (!stream.IsOpen())
		{
			stream.Open(hFile);
			return &stream;
		}
	}

	auto& stream = mDirectStreams.emplace_back();
	stream.Open(hFile);
	return &stream;
}

bool FileSystem::SetGlobal()
{
	if (mGlobalFS && mGlobalFS != this)
	{
		Logger::LogWarn("[FS] Global FS is already set");
		return false;
	}

	mGlobalFS = this;
	return true;
}

bool FileSystem::IsGlobalPresent()
{
	return mGlobalFS != nullptr;
}

FileSystem& FileSystem::GetGlobal()
{
	return *mGlobalFS;
}
