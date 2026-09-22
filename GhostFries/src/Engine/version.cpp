#include "Engine/version.h"

const char* EngineVersion::Version()
{
	return "PreAlpha NO_COMMIT";
}

const char* EngineVersion::VersionType()
{
	return "PreAlpha";
}

int EngineVersion::VersionMajor()
{
	return 0;
}

int EngineVersion::VersionMinor()
{
	return 0;
}

const char* EngineVersion::VersionCommit()
{
	return "NO_COMMIT";
}
