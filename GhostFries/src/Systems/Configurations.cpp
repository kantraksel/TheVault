#include "Systems/Configurations.h"
#define ENABLE_CONFIG 1
#include "Systems/AppData.h"

struct SystemInfo
{
	std::wstring file;
	YamlDoc config;
};

Configurations::Configurations()
{
	mSystems.emplace(Type::Graphics, SystemInfo{ L"graphics.yaml" });
}

Configurations::~Configurations()
{
}

void Configurations::LoadAll()
{
	for (auto& [ id, info ] : mSystems)
	{
		AppData::LoadConfig(info.config, info.file);
	}
}

bool Configurations::Load(Type type)
{
	auto i = mSystems.find(type);
	if (i == mSystems.end())
		return false;

	auto& info = i->second;
	return AppData::LoadConfig(info.config, info.file);
}

bool Configurations::Save(Type type)
{
	auto i = mSystems.find(type);
	if (i == mSystems.end())
		return false;

	return i->second.config.Save();
}

YamlDoc* Configurations::Get(Type type)
{
	auto i = mSystems.find(type);
	if (i == mSystems.end())
		return nullptr;

	return std::addressof(i->second.config);
}
