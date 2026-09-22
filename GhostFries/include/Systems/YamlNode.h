#pragma once
#pragma warning(push, 0)
#include <ryml/ryml.hpp>
#pragma warning(pop)

struct YamlNode
{
	ryml::NodeRef node;

	YamlNode(ryml::NodeRef node) : node(node)
	{
	}

	template <size_t len>
	YamlNode operator[](const char str[len])
	{
		return Get(str, len);
	}
	YamlNode operator[](const std::string_view& str);
	YamlNode Get(const char* str, size_t len);
	YamlNode Append() { return node.append_child(); }
	auto Children() { return node.children(); }
	auto ChildCount() { return node.num_children(); }
	YamlNode AppendKey(const std::string_view& str);

	bool IsMap() { return node.readable() && node.is_map(); }
	bool IsSequence() { return node.readable() && node.is_seq(); }
	bool HasValue() { return node.readable() && node.has_val(); }

	YamlNode& SetMap();
	YamlNode& SetSequence();

	std::string_view GetKey();
	void SetKey(const std::string_view& str);

	int GetInt(int fallback = 0);
	unsigned int GetUInt(unsigned int fallback = 0);
	float GetFloat(float fallback = 0);
	bool GetBool(bool fallback = false);
	std::string_view GetString();
	long long GetLong(long long fallback = 0);
	unsigned long long GetULong(unsigned long long fallback = 0);

	bool TryGetInt(int& val);
	bool TryGetUInt(unsigned int& val);
	bool TryGetFloat(float& val);
	bool TryGetBool(bool& val);
	bool TryGetString(std::string_view& val);
	bool TryGetLong(long long& val);
	bool TryGetULong(unsigned long long& val);

	void SetInt(int val);
	void SetUInt(unsigned int val);
	void SetFloat(float val);
	void SetBool(bool val);
	void SetString(const std::string_view& val);
	void SetLong(long long val);
	void SetULong(unsigned long long val);

	YamlNode& operator=(int val) { SetInt(val); return *this; }
	YamlNode& operator=(unsigned int val) { GetUInt(val); return *this; }
	YamlNode& operator=(float val) { SetFloat(val); return *this; }
	YamlNode& operator=(bool val) { SetBool(val); return *this; }
	YamlNode& operator=(const std::string_view& val) { SetString(val); return *this; }
	YamlNode& operator=(long long val) { SetLong(val); return *this; }
	YamlNode& operator=(unsigned long long val) { SetULong(val); return *this; }
	//otherwise intepreted as bool
	YamlNode& operator=(const char* val) { SetString(val); return *this; }

private:
	void SerializeKey();
};
