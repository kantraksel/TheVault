#include <string>
#include "Systems/YamlNode.h"

YamlNode YamlNode::operator[](const std::string_view& str)
{
	auto i = ryml::csubstr(str.data(), str.size());
	return node[i];
}

YamlNode YamlNode::Get(const char* str, size_t len)
{
	auto i = ryml::csubstr(str, len);
	return node[i];
}

YamlNode YamlNode::AppendKey(const std::string_view& str)
{
	auto n = node.append_child();
	n.change_type(ryml::MAP);

	auto i = ryml::csubstr(str.data(), str.size());
	return n[i];
}

YamlNode& YamlNode::SetMap()
{
	if (node.is_seed())
		node |= ryml::MAP;
	else if (!node.is_map())
		node.change_type(ryml::MAP);

	SerializeKey();
	return *this;
}

YamlNode& YamlNode::SetSequence()
{
	if (node.is_seed())
		node |= ryml::SEQ;
	else if (!node.is_seq())
		node.change_type(ryml::SEQ);

	SerializeKey();
	return *this;
}

std::string_view YamlNode::GetKey()
{
	if (!node.readable() || !node.has_key())
		return {};

	auto key = node.key();
	return { key.data(), key.size() };
}

void YamlNode::SetKey(const std::string_view& val)
{
	auto i = ryml::csubstr(val.data(), val.size());
	node.set_key_serialized(i);
}

int YamlNode::GetInt(int fallback)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtol(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
			return v;
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << fallback;
	SerializeKey();
	return fallback;
}

unsigned int YamlNode::GetUInt(unsigned int fallback)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtoul(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
			return v;
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << fallback;
	SerializeKey();
	return fallback;
}

float YamlNode::GetFloat(float fallback)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		float v = std::strtof(str.data(), &endPtr);
		if (str.data() != endPtr && err != ERANGE)
			return v;
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << fallback;
	SerializeKey();
	return fallback;
}

bool YamlNode::GetBool(bool fallback)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();
		if (str == "true")
			return true;
		else if (str == "false")
			return false;
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << (fallback ? "true" : "false");
	SerializeKey();
	return fallback;
}

std::string_view YamlNode::GetString()
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();
		return std::string_view(str.data(), str.size());
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << "";
	SerializeKey();
	return std::string_view();
}

long long YamlNode::GetLong(long long fallback)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtoll(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
			return v;
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << fallback;
	SerializeKey();
	return fallback;
}

unsigned long long YamlNode::GetULong(unsigned long long fallback)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtoull(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
			return v;
	}
	else if (!node.is_seed() && node.has_children())
		node.change_type(ryml::VAL);

	node << fallback;
	SerializeKey();
	return fallback;
}

bool YamlNode::TryGetInt(int& val)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtol(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
		{
			// in case of Linux env :)
			static_assert(sizeof(val) == sizeof(v));
			val = v;
			return true;
		}
	}

	return false;
}

bool YamlNode::TryGetUInt(unsigned int& val)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtoul(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
		{
			// in case of Linux env :)
			static_assert(sizeof(val) == sizeof(v));
			val = v;
			return true;
		}
	}

	return false;
}

bool YamlNode::TryGetFloat(float& val)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtof(str.data(), &endPtr);
		if (str.data() != endPtr && err != ERANGE)
		{
			val = v;
			return true;
		}
	}

	return false;
}

bool YamlNode::TryGetBool(bool& val)
{
	if (!node.readable() || !node.has_val())
		return false;
	
	auto str = node.val();
	if (str == "true")
		val = true;
	else if (str == "false")
		val = false;
	else
		return false;

	return true;
}

bool YamlNode::TryGetString(std::string_view& val)
{
	if (!node.readable() || !node.has_val())
		return false;
	
	auto str = node.val();
	val = { str.data(), str.size() };
	return true;
}

bool YamlNode::TryGetLong(long long& val)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtoll(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
		{
			val = v;
			return true;
		}
	}

	return false;
}

bool YamlNode::TryGetULong(unsigned long long& val)
{
	if (node.readable() && node.has_val())
	{
		auto str = node.val();

		int& err = errno;
		err = 0;
		char* endPtr;
		auto v = std::strtoull(str.data(), &endPtr, 10);
		if (str.data() != endPtr && err != ERANGE)
		{
			val = v;
			return true;
		}
	}

	return false;
}

void YamlNode::SetInt(int val)
{
	node << val;
	SerializeKey();
}

void YamlNode::SetUInt(unsigned int val)
{
	node << val;
	SerializeKey();
}

void YamlNode::SetFloat(float val)
{
	node << val;
	SerializeKey();
}

void YamlNode::SetBool(bool val)
{
	node << (val ? "true" : "false");
	SerializeKey();
}

void YamlNode::SetString(const std::string_view& val)
{
	node << ryml::csubstr(val.data(), val.size());
	SerializeKey();
}

void YamlNode::SetLong(long long val)
{
	node << val;
	SerializeKey();
}

void YamlNode::SetULong(unsigned long long val)
{
	node << val;
	SerializeKey();
}

void YamlNode::SerializeKey()
{
	if (!node.readable() || !node.has_key())
		return;

	auto key = node.key();
	if (!node.tree()->in_arena(key))
		node.set_key_serialized(key);
}
