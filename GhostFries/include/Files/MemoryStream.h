#pragma once
#include <type_traits>
#include "Utility/Base.h"

template<typename T>
concept ScalarValue = std::is_arithmetic_v<T> || std::is_enum_v<T>;

template<typename T>
concept CompoundValue = !(std::is_arithmetic_v<T> || std::is_enum_v<T>);

class MemoryStream : InsuredObject
{
private:
	char* mpData;
	size_t mSize;
	size_t mPtr;

public:
	MemoryStream(void* pData, size_t size);

	size_t Write(const void* data, size_t size);
	size_t Read(void* data, size_t size);
	void Seek(ptrdiff_t n);
	void ResetPos();
	size_t GetPos() { return mPtr; }
	size_t GetSize() { return mSize; }
	void* GetData() { return mpData; }
	size_t GetReadableSize() { return mSize - mPtr; }
	char* GetCurrentData() { return mpData + mPtr; }

	template<CompoundValue T>
	bool Write(const T& value)
	{
		static_assert(std::is_trivially_copyable<T>(), "Write() requires trivially-copyable type");
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	template<ScalarValue T>
	bool Write(T value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	template<typename T>
	bool Read(T& value)
	{
		static_assert(std::is_trivially_copyable<T>(), "Read() requires trivially-copyable type");
		return Read(&value, sizeof(value)) == sizeof(value);
	}
};
