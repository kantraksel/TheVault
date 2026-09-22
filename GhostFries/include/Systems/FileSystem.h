#pragma once
#include <list>
#include "Utility/Base.h"
#include "Files/FSStream.h"

class FileSystem : LockedObject
{
private:
	static FileSystem* mGlobalFS;

	std::list<class DirectStream> mDirectStreams;

public:
	FileSystem();
	~FileSystem();

	bool Initialize(struct YamlDoc& doc);
	void Shutdown();

	FSStream* OpenFile(const std::string_view& name);
	FSStream* OpenFileWrite(const std::string_view& name);

	bool SetGlobal();
	static bool IsGlobalPresent();
	static FileSystem& GetGlobal();
};
