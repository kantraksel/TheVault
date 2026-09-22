#pragma once
#include <string>
#include "Utility/Base.h"
#include "Utility/FixedArray.h"

/// <summary>
/// Reads files mounted in engine filesystem
/// Accesses files if file is not in packs and direct access is enabled in filesystem
/// </summary>
class FileReader : LockedObject
{
public:
	enum class Direction
	{
		Begin,
		Current,
		End,
	};

private:
	class FSStream* hFile;

public:
	FileReader();
	FileReader(FSStream* stream);
	~FileReader();

	bool Open(const std::string_view& name);
	bool Open(const std::wstring_view& name);
	void Close();
	bool IsOpen();
	operator bool() { return IsOpen(); }

	unsigned int Read(void* lpBuffer, unsigned int nLength);
	unsigned long long Length();

	unsigned long long Tell();
	bool Seek(unsigned long long position, Direction dir = Direction::Begin);

	template<typename T>
	bool Read(T& value)
	{
		static_assert(std::is_trivially_copyable<T>(), "Read() requires trivially-copyable type");
		return Read(&value, sizeof(value)) == sizeof(value);
	}

	template<typename T, typename S>
	bool Read(FixedArray<T, S>& array)
	{
		auto bytes = sizeof(T) * array.size();
		return Read(array.data(), (unsigned int)bytes) == bytes;
	}
};
