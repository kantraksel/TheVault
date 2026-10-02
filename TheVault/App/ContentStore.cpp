#include "ContentStore.h"
#include "Systems/YamlDoc.h"
#include "Engine/Logger.h"
#include "Files/FileStream.h"
#include "Vault/Crypto.h"

constexpr uint32_t DocumentVersion = 2;

enum struct PassType
{
	Text,
	File,
};

struct Pass
{
	PassType type;
	SecureArray content;
};

ContentStore::ContentStore(UnsavedState& unsavedState) : unsavedState(unsavedState)
{
}

ContentStore::~ContentStore()
{
}

void ContentStore::Reset()
{
	mStore.clear();
}

int ContentStore::GetCount()
{
	return static_cast<int>(mStore.size());
}

bool ContentStore::CheckBounds(int i)
{
	assert(i >= 0 && i < mStore.size() && "Store bound check failed");
	return i >= 0 && i < mStore.size();
}

bool ContentStore::IsText(int i)
{
	if (!CheckBounds(i))
		return false;
	return mStore[i].second.type == PassType::Text;
}

bool ContentStore::IsFile(int i)
{
	if (!CheckBounds(i))
		return false;
	return mStore[i].second.type == PassType::File;
}

int ContentStore::Add(const std::string_view& name)
{
	int i = static_cast<int>(mStore.size());

	mStore.emplace_back(std::string(name), Pass
		{
			.type = PassType::Text,
		});

	unsavedState.NotifyChange();
	return i;
}

std::string_view ContentStore::GetName(int i)
{
	if (!CheckBounds(i))
		return {};
	return mStore[i].first;
}

std::string_view ContentStore::GetText(int i)
{
	if (!CheckBounds(i))
		return {};

	auto& pass = mStore[i].second;
	if (pass.type != PassType::Text)
	{
		assert(false && "Incorrect entry type");
		return {};
	}

	return { reinterpret_cast<char*>(pass.content.data()), pass.content.size() - 1 };
}

static SecureArray CopyPassword(const std::string_view& password)
{
	SecureArray buff(password.size() + 1);
	Crypto::ZeroMemory(buff);
	buff.copyFrom(SecureArray::CreateRef(password.data(), password.size()));
	return buff;
}

void ContentStore::Remove(int i)
{
	if (!CheckBounds(i))
		return;

	auto it = mStore.begin();
	for (int k = 1; k <= i; ++k)
	{
		++it;
		if (it == mStore.end())
		{
			assert(false && "Bound check failed");
			return;
		}
	}
	mStore.erase(it);

	unsavedState.NotifyChange();
}

void ContentStore::AddText(const std::string_view& name, const std::string_view& password)
{
	int i = Add(name);
	SetText(i, password);
}

void ContentStore::SetText(int i, const std::string_view& password)
{
	if (!CheckBounds(i))
		return;

	auto& pass = mStore[i].second;
	pass.content = CopyPassword(password);
	unsavedState.NotifyChange();
}

void ContentStore::SetName(int i, const std::string_view& name)
{
	if (!CheckBounds(i))
		return;

	mStore[i].first = name;
	unsavedState.NotifyChange();
}

bool ContentStore::AddFile(const std::string_view& name, const std::wstring_view& file)
{
	int i = Add(name);
	return SetFile(i, file);
}

bool ContentStore::SetFile(int i, const std::wstring_view& file)
{
	if (!CheckBounds(i))
		return false;

	FileStream stream;
	if (!stream.OpenRead(file))
	{
		Logger::LogError(L"Failed to open {} for reading", file);
		return false;
	}

	SecureArray buffer(stream.Length());
	if (stream.Read(buffer.data(), static_cast<int64_t>(buffer.size())) != buffer.size())
	{
		Logger::LogError(L"Failed to read content of {}", file);
		return false;
	}

	auto& pass = mStore[i].second;
	pass.content = std::move(buffer);
	unsavedState.NotifyChange();
	return true;
}

bool ContentStore::ExtractFile(int i, const std::wstring_view& file)
{
	if (!CheckBounds(i))
		return false;

	auto& pass = mStore[i].second;
	if (pass.type != PassType::File)
	{
		assert(false && "Incorrect entry type");
		return false;
	}

	FileStream stream;
	if (!stream.OpenWrite(file))
	{
		Logger::LogError(L"Failed to open {} for writing", file);
		return false;
	}

	auto& buff = pass.content;
	if (!stream.Write(buff.data(), static_cast<int64_t>(buff.size())))
	{
		Logger::LogError(L"Failed to write content to {}", file);
		return false;
	}
	return true;
}

struct RymlSecureAllocator
{
	static auto Get()
	{
		ryml::Callbacks callbacks{};
		callbacks.m_allocate = alloc;
		callbacks.m_free = dealloc;
		callbacks.m_error_basic = error;
		callbacks.m_error_parse = error;
		callbacks.m_error_visit = error;
		return callbacks;
	}

	static void* alloc(size_t len, void*, void*)
	{
		return sodium_malloc(len);
	}

	static void dealloc(void* mem, size_t, void*)
	{
		sodium_free(mem);
	}

	static void error(ryml::csubstr msg, ryml::ErrorDataBasic const& errdata, void*)
	{
		throw std::runtime_error(std::string(msg.data(), msg.size()));
	}

	static void error(ryml::csubstr msg, ryml::ErrorDataParse const& errdata, void*)
	{
		throw std::runtime_error(std::string(msg.data(), msg.size()));
	}

	static void error(ryml::csubstr msg, ryml::ErrorDataVisit const& errdata, void*)
	{
		throw std::runtime_error(std::string(msg.data(), msg.size()));
	}
};

SecureArray ContentStore::Serialize()
{
	YamlDoc doc(RymlSecureAllocator::Get());
	doc["version"] = DocumentVersion;
	auto node = doc["password"].SetMap();

	for (auto& [name, pass] : mStore)
	{
		if (pass.type == PassType::Text)
		{
			auto child = node[name].SetMap();
			child["type"] = "Text";

			if (pass.content.empty())
				child["content"] = "";
			else
				child["content"] = std::string_view(reinterpret_cast<char*>(pass.content.data()), pass.content.size() - 1);
		}
		else if (pass.type == PassType::File)
		{
			SecureArray buffer;
			auto content = Crypto::BufferToBase64(pass.content, buffer);
			if (content.empty())
			{
				Logger::LogError("Could not convert buffer to base64");
				return {};
			}

			auto child = node[name].SetMap();
			child["type"] = "Data";
			child["content"] = content;
		}
		else
		{
			assert(false && "Unknown entry type - cannot serialize");
			Logger::LogWarn("Cannot serialize entry {} - type {} not defined in serializer", name, static_cast<int>(pass.type));
		}
	}

	auto buffer = SecureArray(1048576);
	auto bufferView = FixedArrayChar::CreateRef(buffer.data(), buffer.size());
	auto view = doc.Serialize(bufferView);
	if (view.empty())
		return nullptr;
	if (!view.data())
	{
		buffer = SecureArray(view.size());
		bufferView = FixedArrayChar::CreateRef(buffer.data(), buffer.size());
		view = doc.Serialize(bufferView);
		if (view.empty() || !view.data())
			return {};
	}

	if (buffer.size() != view.size())
	{
		auto result = SecureArray(view.size());
		result.copyFrom(buffer.span(0, view.size()));
		buffer = std::move(result);
	}
	return buffer;
}

bool ContentStore::Deserialize(const SecureArray& data)
{
	YamlDoc doc(RymlSecureAllocator::Get());
	auto arr = FixedArrayChar::CreateRef(const_cast<uint8_t*>(data.data()), static_cast<unsigned int>(data.size()));
	if (!doc.Load(arr, L"internal"))
	{
		Logger::LogError("Failed to read YamlDoc: invalid content");
		return false;
	}

	uint32_t version = 0;
	if (!doc["version"].TryGetUInt(version) || version != DocumentVersion)
	{
		Logger::LogError("Failed to read document: unsupported version {}", version);
		return false;
	}

	auto node = doc["password"];
	if (!node.IsMap())
	{
		Logger::LogError("Failed to read document: invalid structure");
		return false;
	}

	for (YamlNode n : node.Children())
	{
		if (n.IsMap())
		{
			std::string_view type;
			if (!n["type"].TryGetString(type))
			{
				Logger::LogWarn("Invalid type of entry {} - type not specified", n.GetKey());
				continue;
			}

			if (type == "Data")
			{
				std::string_view content;
				if (!n["value"].TryGetString(content))
				{
					Logger::LogWarn("Invalid type of entry {} - string expected", n.GetKey());
					continue;
				}

				Pass pass;
				pass.type = PassType::File;
				pass.content = Crypto::Base64ToBuffer(content);
				mStore.emplace_back(std::string(n.GetKey()), std::move(pass));
			}
			else if (type == "Text")
			{
				std::string_view content;
				if (!n["value"].TryGetString(content))
				{
					Logger::LogWarn("Invalid type of entry {} - string expected", n.GetKey());
					continue;
				}

				Pass pass;
				pass.type = PassType::Text;
				pass.content = CopyPassword(content);
				mStore.emplace_back(std::string(n.GetKey()), std::move(pass));
			}
			else
				Logger::LogWarn("Invalid type of entry {} - unknown type {}", n.GetKey(), type);
		}
		else
			Logger::LogWarn("Invalid type of entry {} - not an object", n.GetKey());
	}
	return true;
}
