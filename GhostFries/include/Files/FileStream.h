#pragma once
#include <cstdint>
#include <string>
#include "Utility/Base.h"
#include "Utility/FixedArray.h"

/// <summary>
/// Universal direct file stream
/// </summary>
class FileStream : LockedObject
{
public:
	enum class Direction
	{
		Begin,
		Current,
		End,
	};

private:
	void* hFile;
	int64_t size;
	int64_t filePtr;

public:
	FileStream();
	~FileStream();
	bool OpenRead(const std::string_view& name);
	bool OpenRead(const std::wstring_view& name);
	bool OpenWrite(const std::string_view& name);
	bool OpenWrite(const std::wstring_view& name);
	bool OpenRead(const std::string& name);
	bool OpenRead(const std::wstring& name);
	bool OpenWrite(const std::string& name);
	bool OpenWrite(const std::wstring& name);
	bool Close();

	int64_t Read(void* lpBuffer, int64_t nLength);
	bool Write(const void* lpBuffer, int64_t nLength);

	int64_t Length();
	int64_t Tell();
	bool Seek(int64_t position, Direction dir = Direction::Begin);

	bool Good();
	inline operator bool()
	{
		return Good();
	}

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
		return Read(array.data(), (int64_t)bytes) == bytes;
	}

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
		return Write(array.data(), (int64_t)bytes);
	}
};
