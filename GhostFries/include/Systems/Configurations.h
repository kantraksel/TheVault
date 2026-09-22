#pragma once
#include <map>
#include "Utility/Base.h"

class Configurations final : LockedObject
{
public:
	enum class Type
	{
		Graphics,
	};

private:
	std::map<Type, struct SystemInfo> mSystems;

public:
	Configurations();
	~Configurations();

	void LoadAll();
	bool Load(Type type);
	bool Save(Type type);
	struct YamlDoc* Get(Type type);
};
