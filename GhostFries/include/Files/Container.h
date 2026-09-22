#pragma once
#include "FileReader.h"
#include "FileWriter.h"

struct Container
{
	enum FileTypes
	{
		Shaders =		0x5343,
		VertexShader =	0x5356,
		PixelShader =	0x5350,
		Mesh =			0x004D,
		Texture =		0x0054,
		Collider =		0x4350,
		Sound =			0x0057,
		Index =			0x0049,
		Prefab =		0x5053,
		Scene =			0x5353,
	};

	static bool Open(FileReader& reader, unsigned short type, unsigned short version, unsigned int& dataSize);
	static bool Write(FileWriter& os, unsigned short type, unsigned short version, unsigned int dataSize);

	static bool BeginWrite(FileWriter& os, unsigned short type, unsigned short version);
	static bool EndWrite(FileWriter& os, unsigned short type, unsigned short version);

	template<typename T>
	static bool BeginWrite(FileWriter& os, unsigned short type, unsigned short version, const T& data)
	{
		return BeginWrite(os, type, version) && os.Write(&data, sizeof(data));
	}
	template<typename T>
	static bool EndWrite(FileWriter& os, unsigned short type, unsigned short version, const T& data)
	{
		return EndWrite(os, type, version) && os.Write(&data, sizeof(data));
	}

	template<typename F, typename T>
	static bool BeginWrite(FileWriter& os, const T& data)
	{
		return BeginWrite(os, F::Magic, F::Version) && os.Write(&data, sizeof(data));
	}
	template<typename F, typename T>
	static bool EndWrite(FileWriter& os, const T& data)
	{
		return EndWrite(os, F::Magic, F::Version) && os.Write(&data, sizeof(data));
	}
	template<typename F>
	static bool EndWrite(FileWriter& os)
	{
		return EndWrite(os, F::Magic, F::Version);
	}

	template<typename F, typename T>
	static bool Open(FileReader& reader, unsigned int& dataSize)
	{
		return Open(reader, F::Magic, F::Version, dataSize);
	}
	template<typename F, typename T>
	static bool Open(FileReader& reader, T& data, unsigned int& dataSize)
	{
		if (!Open(reader, F::Magic, F::Version, dataSize) || dataSize < sizeof(data) || !reader.Read(data))
			return false;

		dataSize -= sizeof(data);
		return true;
	}

	template<typename F>
	static bool Write(FileWriter& os, unsigned int dataSize)
	{
		return Write(os, F::Magic, F::Version, dataSize);
	}
};
