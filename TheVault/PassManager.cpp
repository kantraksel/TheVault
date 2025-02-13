#include "PassManager.h"
#include "Utility/YamlDoc.h"
#include "Engine/Logger.h"
#include "PackFS/FileReader.h"
#include "PackFS/FileWriter.h"
#include "Crypto.h"

enum struct Type
{
	Text,
	File,
};

struct Pass
{
	Type type;
	FixedArrayUChar content;
};

PassManager::PassManager(UnsavedState& unsavedState) : unsavedState(unsavedState)
{
	mStore.reserve(256);
}

PassManager::~PassManager()
{

}

void PassManager::Reset()
{
	mStore.clear();
}

int PassManager::GetCount()
{
	return (int)mStore.size();
}

bool PassManager::IsPasswordText(int i)
{
	return mStore[i].second.type == Type::Text;
}

bool PassManager::IsPasswordFile(int i)
{
	return mStore[i].second.type == Type::File;
}

std::string_view PassManager::GetName(int i)
{
	return mStore[i].first;
}

std::string_view PassManager::GetPassword(int i)
{
	auto& pass = mStore[i].second;
	if (pass.type != Type::Text)
		return {};

	return std::string_view((char*)&pass.content, pass.content.size() - 1);
}

void PassManager::Add(const std::string_view& name, const std::string_view& password)
{
	FixedArrayUChar buff((unsigned int)password.size() + 1);
	memcpy(buff, password.data(), password.size());
	buff[(unsigned int)password.size()] = 0;

	Pass pass;
	pass.type = Type::Text;
	pass.content = std::move(buff);
	mStore.push_back(std::make_pair(std::string(name), std::move(pass)));

	unsavedState.NotifyChange();
}

void PassManager::Remove(int i)
{
	auto it = mStore.begin();
	for (int k = 1; k <= i; ++k)
	{
		++it;
	}
	mStore.erase(it);

	unsavedState.NotifyChange();
}

void PassManager::Change(int i, const std::string_view& password)
{
	auto& pass = mStore[i].second;
	if (pass.type != Type::Text)
		return;

	FixedArrayUChar buff((unsigned int)password.size() + 1);
	memcpy(buff, password.data(), password.size());
	buff[(unsigned int)password.size()] = 0;

	pass.content = std::move(buff);
	unsavedState.NotifyChange();
}

void PassManager::ChangeName(int i, const std::string_view& name)
{
	mStore[i].first = name;
	unsavedState.NotifyChange();
}

void PassManager::AddFile(const std::string_view& name, const std::wstring_view& file)
{
	FileReader stream;
	if (!stream.Open(file))
		return;

	FixedArrayUChar buffer((unsigned int)stream.Length());
	if (!stream.Read(buffer))
		return;

	Pass pass;
	pass.type = Type::File;
	pass.content = std::move(buffer);
	mStore.push_back(std::make_pair(std::string(name), std::move(pass)));

	unsavedState.NotifyChange();
}

void PassManager::ChangeFile(int i, const std::wstring_view& file)
{
	auto& pass = mStore[i].second;
	if (pass.type != Type::File)
		return;

	FileReader stream;
	if (!stream.Open(file))
		return;

	FixedArrayUChar buffer((unsigned int)stream.Length());
	if (!stream.Read(buffer))
		return;

	pass.content = std::move(buffer);
	unsavedState.NotifyChange();
}

void PassManager::ExtractFile(int i, const std::wstring_view& file)
{
	auto& pass = mStore[i].second;
	if (pass.type != Type::File)
		return;

	FileWriter stream;
	if (!stream.Open(file, true))
		return;

	auto& buff = pass.content;
	if (!stream.Write(buff))
		return;
}

std::string PassManager::Serialize()
{
	YamlDoc doc;
	doc["version"] = 1;
	auto node = doc["password"].SetMap();

	std::string fileBuffer;
	for (auto& [name, pass] : mStore)
	{
		if (pass.type == Type::Text)
			node[name] = std::string_view((char*)&pass.content, pass.content.size() - 1);
		else if (pass.type == Type::File)
		{
			if (!Crypto::BufferToBase64(pass.content, fileBuffer))
			{
				Logger::LogError("Could not convert buffer to base64");
				return {};
			}

			auto child = node[name].SetMap();
			child["type"] = "File";
			child["content"] = fileBuffer;
		}
	}

	try
	{
		return ryml::emitrs_yaml<std::string>(doc.mTree);
	}
	catch (const std::runtime_error& e)
	{
		Logger::LogError("Could not serialize YamlDoc: {}", e.what());
	}
	return {};
}

bool PassManager::Deserialize(const std::string_view& data)
{
	YamlDoc doc;
	auto arr = FixedArrayChar::CreateArrayRef((char*)data.data(), (unsigned int)data.size());
	if (!doc.Load(arr, L"internal"))
		return false;

	uint32_t version;
	if (!doc["version"].TryGetUInt(version) || version != 1)
		return false;

	auto node = doc["password"];
	if (!node.IsMap())
		return false;

	for (YamlNode n : node.Children())
	{
		if (n.HasValue())
		{
			std::string_view str;
			if (!n.TryGetString(str))
				continue;

			FixedArrayUChar buff((unsigned int)str.size() + 1);
			memcpy(buff, str.data(), str.size());
			buff[(unsigned int)str.size()] = 0;

			Pass pass;
			pass.type = Type::Text;
			pass.content = std::move(buff);
			mStore.push_back(std::make_pair(std::string(n.GetKey()), std::move(pass)));
		}
		else if (n.IsMap())
		{
			std::string_view type;
			if (!n["type"].TryGetString(type))
				continue;

			if (type == "File")
			{
				std::string_view content;
				if (!n["content"].TryGetString(content))
					continue;

				Pass pass;
				pass.type = Type::File;
				pass.content = Crypto::Base64ToBuffer(content);
				mStore.push_back(std::make_pair(std::string(n.GetKey()), std::move(pass)));
			}
		}
	}
	return true;
}
