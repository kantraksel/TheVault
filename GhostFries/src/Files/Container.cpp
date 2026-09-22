#include "Files/Container.h"
#include "Engine/Logger.h"

static constexpr unsigned short Magic = 0x4647;

struct Header
{
	unsigned short magic;
	unsigned short type;
	unsigned short version;
	unsigned short reserved0;

	unsigned int dataSize;
};

bool Container::Open(FileReader& reader, unsigned short type, unsigned short version, unsigned int& dataSize)
{
	Header header;
	if (reader.Read(header))
	{
		if (header.magic != Magic)
		{
			Logger::LogWarn("Unrecognized file format");
			return false;
		}

		if (header.type != type)
		{
			Logger::LogWarn("Tried to open {} file as {}", header.type, type);
			return false;
		}

		if (header.version != version)
		{
			Logger::LogWarn("File version mismatch: expected {}, got {}", version, header.version);
			return false;
		}

		dataSize = header.dataSize;
		return true;
	}

	return false;
}

bool Container::Write(FileWriter& os, unsigned short type, unsigned short version, unsigned int dataSize)
{
	Header header{ Magic, type, version, 0, dataSize };
	return os.Write(header);
}

bool Container::BeginWrite(FileWriter& os, unsigned short type, unsigned short version)
{
	Header header{ Magic, type, version, 0, 0 };
	return os.Write(header);
}

bool Container::EndWrite(FileWriter& os, unsigned short type, unsigned short version)
{
	auto size = std::min(os.Length() - sizeof(Header), 0xFFFFFFFFull);
	Header header{ Magic, type, version, 0, (unsigned int)size };
	return os.Seek(0) && os.Write(header);
}
