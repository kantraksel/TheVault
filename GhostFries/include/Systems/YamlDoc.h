#pragma once
#include "Utility/FixedArray.h"
#include "YamlNode.h"

struct YamlDoc
{
	FixedArrayChar mContent;
	ryml::Tree mTree;
	std::wstring mFile;

	YamlDoc();
	YamlDoc(const ryml::Callbacks& callbacks);
	~YamlDoc();

	bool Load(const std::wstring_view& file);
	bool Load(FixedArrayChar& data, const std::wstring_view& name);

	bool Save(const std::wstring_view& file);
	bool Save();

	YamlNode GetRootNode();
	YamlNode operator[](const std::string_view& str);

	bool Serialize(std::string& content);
	FixedArrayChar Serialize(FixedArrayChar& content);
	bool Deserialize(const std::string_view& content);

private:
	bool Load(FixedArrayChar& content);
};
