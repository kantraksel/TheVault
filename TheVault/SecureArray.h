#pragma once
#include <memory>
#if _DEBUG
	#include <stdexcept>
#endif

class SecureArray
{
private:
	unsigned char* mpArray;
	size_t mSize;

	typedef void(*Deleter)(void*);
	Deleter mDeleter;

public:
	SecureArray() : mpArray(nullptr), mSize(0), mDeleter(nullptr)
	{
	}

	SecureArray(std::nullptr_t) : mpArray(nullptr), mSize(0), mDeleter(nullptr)
	{
	}

	SecureArray(const SecureArray& other) = delete;
	SecureArray& operator=(const SecureArray& other) = delete;

	SecureArray(SecureArray&& other) noexcept
	{
		mSize = other.mSize;
		mpArray = other.mpArray;
		mDeleter = other.mDeleter;
		other.mSize = 0;
		other.mpArray = nullptr;
		other.mDeleter = nullptr;
	}

	SecureArray& operator=(SecureArray&& other) noexcept
	{
		if (mDeleter)
			mDeleter(mpArray);

		mSize = other.mSize;
		mpArray = other.mpArray;
		mDeleter = other.mDeleter;
		other.mSize = 0;
		other.mpArray = nullptr;
		other.mDeleter = nullptr;

		return *this;
	}

	SecureArray& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	~SecureArray()
	{
		if (mDeleter)
			mDeleter(mpArray);
	}

	void reset()
	{
		if (mDeleter)
			mDeleter(mpArray);
		mpArray = nullptr;
		mSize = 0;
		mDeleter = nullptr;
	}

	auto& operator[](size_t i) const
	{
#if _DEBUG
		if (i >= mSize)
			throw std::out_of_range("Array index is out of range");

		if (!mpArray)
			throw std::logic_error("Array pointer is null");
#endif

		return mpArray[i];
	}

	operator unsigned char*()
	{
		return mpArray;
	}

	auto* operator&()
	{
		return mpArray;
	}

	operator const unsigned char*() const
	{
		return mpArray;
	}

	const auto* operator&() const
	{
		return mpArray;
	}

	auto size() const
	{
		return mSize;
	}

	bool empty() const
	{
		return mSize == 0;
	}

	char* str() const
	{
		return (char*)mpArray;
	}

	static SecureArray Wrap(unsigned char* pArray, size_t n, Deleter deleter)
	{
		SecureArray array;
		array.mpArray = pArray;
		array.mSize = n;
		array.mDeleter = deleter;
		return array;
	}

	static SecureArray Wrap(char* pArray, size_t n, Deleter deleter)
	{
		SecureArray array;
		array.mpArray = (unsigned char*)pArray;
		array.mSize = n;
		array.mDeleter = deleter;
		return array;
	}
};
