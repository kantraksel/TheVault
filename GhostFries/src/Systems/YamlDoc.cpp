#include "Systems/YamlDoc.h"
#pragma warning(push, 0)
#include <ryml/ryml_std.hpp>
#pragma warning(pop)
#include "Files/FileStream.h"
#include "Engine/Logger.h"

struct RymlGlobalCallbacks
{
	static auto GetCallbacks()
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
		return new unsigned char[len];
	}

	static void dealloc(void* mem, size_t, void*)
	{
		delete[] mem;
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

static void ValidateTreeRoot(ryml::Tree& tree)
{
	auto root = tree.rootref();
	if (!root.is_map())
	{
		root.clear();
		root |= ryml::MAP;
	}
}

YamlDoc::YamlDoc() : mTree(RymlGlobalCallbacks::GetCallbacks())
{
	ValidateTreeRoot(mTree);
}

YamlDoc::YamlDoc(const ryml::Callbacks& callbacks) : mTree(callbacks)
{
	ValidateTreeRoot(mTree);
}

YamlDoc::~YamlDoc()
{
}

bool YamlDoc::Load(FixedArrayChar& data, const std::wstring_view& name)
{
	mFile = name;
	return Load(data);
}

bool YamlDoc::Load(const std::wstring_view& file)
{
	mFile = file;

	FileStream reader;
	if (!reader.OpenRead(file))
	{
		Logger::LogError(L"YamlDoc: could not open {}", file);
		return false;
	}

	FixedArrayChar content((unsigned int)reader.Length());
	if (content.empty())
	{
		auto root = mTree.rootref();
		root.clear();
		root |= ryml::MAP;
		return true;
	}

	if (!reader.Read(content))
	{
		Logger::LogError(L"YamlDoc: could not read {}", file);
		return false;
	}

	return Load(content);
}

bool YamlDoc::Load(FixedArrayChar& content)
{
	try
	{
		mTree.clear();
		ryml::parse_in_place(ryml::substr(content.data(), content.size()), &mTree);
		ValidateTreeRoot(mTree);
	}
	catch (const std::runtime_error& e)
	{
		Logger::LogError(L"YamlDoc: could not parse {}", mFile);
		Logger::LogWarn(e.what());
		mTree.clear();
		ValidateTreeRoot(mTree);
		return false;
	}
	mContent = std::move(content);
	return true;
}

bool YamlDoc::Save()
{
	return Save(mFile);
}

bool YamlDoc::Save(const std::wstring_view& file)
{
	FileStream stream;
	if (!stream.OpenWrite(file))
	{
		Logger::LogError(L"YamlDoc: could not open {}", file);
		return false;
	}

	try
	{
		auto content = ryml::emitrs_yaml<std::string>(mTree);
		if (!stream.Write(content.data(), content.size()))
		{
			Logger::LogError(L"YamlDoc: could not write {}", file);
			return false;
		}
	}
	catch (const std::runtime_error& e)
	{
		Logger::LogError(L"YamlDoc: could not generate {}", file);
		Logger::LogWarn(e.what());
		return false;
	}
	return true;
}

YamlNode YamlDoc::GetRootNode()
{
	return mTree.rootref();
}

YamlNode YamlDoc::operator[](const std::string_view& str)
{
	auto node = mTree.rootref();
	return node[ryml::csubstr(str.data(), str.size())];
}

bool YamlDoc::Serialize(std::string& content)
{
	try
	{
		content = ryml::emitrs_yaml<std::string>(mTree);
		return true;
	}
	catch (const std::runtime_error& e)
	{
		Logger::LogError("Failed to serialize YamlDoc: {}", e.what());
		return false;
	}
}

FixedArrayChar YamlDoc::Serialize(FixedArrayChar& content)
{
	try
	{
		auto result = ryml::emit_yaml(mTree, mTree.root_id_maybe(), ryml::substr(content.data(), content.size()), false);
		return FixedArrayChar::CreateRef(result.data(), result.size());
	}
	catch (const std::runtime_error& e)
	{
		Logger::LogError("Failed to serialize YamlDoc: {}", e.what());
		return nullptr;
	}
}

bool YamlDoc::Deserialize(const std::string_view& content)
{
	try
	{
		mTree.clear();
		ryml::parse_in_arena(ryml::csubstr(content.data(), content.size()), &mTree);
		ValidateTreeRoot(mTree);
	}
	catch (const std::runtime_error& e)
	{
		Logger::LogError("YamlDoc: could not deserialize doc: {}", e.what());
		mTree.clear();
		ValidateTreeRoot(mTree);
		return false;
	}
	return true;
}
