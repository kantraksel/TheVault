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

bool Container::Open(FileStream& stream, unsigned short type, unsigned short version, unsigned int& dataSize)
{
	Header header;
	if (stream.Read(header))
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

bool Container::Write(FileStream& stream, unsigned short type, unsigned short version, unsigned int dataSize)
{
	Header header{ Magic, type, version, 0, dataSize };
	return stream.Write(header);
}

bool Container::BeginWrite(FileStream& stream, unsigned short type, unsigned short version)
{
	Header header{ Magic, type, version, 0, 0 };
	return stream.Write(header);
}

bool Container::EndWrite(FileStream& stream, unsigned short type, unsigned short version)
{
	auto size = std::min(stream.Length() - sizeof(Header), static_cast<uint64_t>(UINT32_MAX));
	Header header{ Magic, type, version, 0, static_cast<unsigned int>(size) };
	return stream.Seek(0) && stream.Write(header);
}
