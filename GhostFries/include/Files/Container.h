#pragma once
#include "FileStream.h"

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

	static bool Open(FileStream& stream, unsigned short type, unsigned short version, unsigned int& dataSize);
	static bool Write(FileStream& stream, unsigned short type, unsigned short version, unsigned int dataSize);

	static bool BeginWrite(FileStream& stream, unsigned short type, unsigned short version);
	static bool EndWrite(FileStream& stream, unsigned short type, unsigned short version);

	template<typename T>
	static bool BeginWrite(FileStream& stream, unsigned short type, unsigned short version, const T& data)
	{
		return BeginWrite(stream, type, version) && stream.Write(data);
	}
	template<typename T>
	static bool EndWrite(FileStream& stream, unsigned short type, unsigned short version, const T& data)
	{
		return EndWrite(stream, type, version) && stream.Write(data);
	}

	template<typename F, typename T>
	static bool BeginWrite(FileStream& stream, const T& data)
	{
		return BeginWrite(stream, F::Magic, F::Version) && stream.Write(data);
	}
	template<typename F, typename T>
	static bool EndWrite(FileStream& stream, const T& data)
	{
		return EndWrite(stream, F::Magic, F::Version) && stream.Write(data);
	}
	template<typename F>
	static bool EndWrite(FileStream& stream)
	{
		return EndWrite(stream, F::Magic, F::Version);
	}

	template<typename F, typename T>
	static bool Open(FileStream& stream, unsigned int& dataSize)
	{
		return Open(stream, F::Magic, F::Version, dataSize);
	}
	template<typename F, typename T>
	static bool Open(FileStream& stream, T& data, unsigned int& dataSize)
	{
		if (!Open(stream, F::Magic, F::Version, dataSize) || dataSize < sizeof(data) || !stream.Read(data))
			return false;

		dataSize -= sizeof(data);
		return true;
	}

	template<typename F>
	static bool Write(FileStream& stream, unsigned int dataSize)
	{
		return Write(stream, F::Magic, F::Version, dataSize);
	}
};
