#pragma once
#include <string>
#include "Utility/Base.h"
#include "Utility/FixedArray.h"

/// <summary>
/// Writes files to writtable pack mounted in engine filesystem
/// Creates OS files if no pack is mounted and direct access is enabled in filesystem
/// </summary>
class FileWriter : LockedObject
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
	FileWriter();
	FileWriter(FSStream* stream);
	~FileWriter();

	bool Open(const std::string_view& name);
	bool Open(const std::wstring_view& name);
	void Close();
	bool IsOpen();
	operator bool() { return IsOpen(); }

	bool Write(const void* lpBuffer, unsigned int nLength);
	unsigned long long Length();

	unsigned long long Tell();
	bool Seek(unsigned long long position, Direction dir = Direction::Begin);

	template<typename T>
	bool Write(const T& value)
	{
		static_assert(std::is_trivially_copyable<T>(), "Write() requires trivially-copyable type");
		return Write(&value, sizeof(value));
	}

	template<typename T, typename S>
	bool Write(const FixedArray<T, S>& array)
	{
		auto bytes = sizeof(T) * array.size();
		return Write(array.data(), (unsigned int)bytes);
	}
};
