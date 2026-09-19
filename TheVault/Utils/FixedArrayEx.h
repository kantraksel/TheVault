#pragma once
#include <cstring>
#include <memory>
#include <limits>
#if _DEBUG
#include <stdexcept>
#endif

// Windows SDK defs
#undef min
#undef max

template<typename T>
concept DefaultConstructible = std::conjunction_v<std::is_default_constructible<T>, std::is_destructible<T>>;

template<typename A, typename B>
concept SimilarIntegrals = std::conjunction_v<std::is_integral<A>, std::is_integral<B>, std::bool_constant<sizeof(A) == sizeof(B)>>;

template <typename T>
struct Allocator
{
	static T* allocate(size_t n)
	{
		return new T[n];
	}

	static void deallocate(T* p)
	{
		delete[] p;
	}
};

template<DefaultConstructible T, typename S = size_t, typename Alloc = Allocator<T>>
class FixedArrayEx
{
private:
	T* mpArray;
	S mSize;
	bool mOwnsArray;

public:
	FixedArrayEx() : mpArray(nullptr), mSize(0), mOwnsArray(false)
	{
	}

	FixedArrayEx(std::nullptr_t) : mpArray(nullptr), mSize(0), mOwnsArray(false)
	{
	}

	FixedArrayEx(S n)
	{
		if (n <= 0)
		{
			mOwnsArray = false;
			mSize = 0;
			mpArray = nullptr;
			return;
		}

		mOwnsArray = true;
		mSize = n;
		mpArray = Alloc::allocate(n);
	}

	~FixedArrayEx()
	{
		if (mOwnsArray)
			Alloc::deallocate(mpArray);
	}

private:
	FixedArrayEx(T* pArray, S n, bool ownsArray) : mpArray(pArray), mSize(n), mOwnsArray(ownsArray)
	{
	}

public:
	FixedArrayEx(const FixedArrayEx& other) requires std::is_trivially_copyable_v<T>
	{
		if (other.mSize == 0)
		{
			mOwnsArray = false;
			mSize = 0;
			mpArray = nullptr;
			return;
		}

		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;

		if (mOwnsArray)
		{
			mpArray = Alloc::allocate(mSize);
			memcpy(mpArray, other.mpArray, mSize * sizeof(T));
		}
		else
			mpArray = other.mpArray;
	}

	FixedArrayEx& operator=(const FixedArrayEx& other) requires std::is_trivially_copyable_v<T>
	{
		if (mOwnsArray)
			Alloc::deallocate(mpArray);

		if (other.mSize == 0)
		{
			mOwnsArray = false;
			mSize = 0;
			mpArray = nullptr;
			return *this;
		}

		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		
		if (mOwnsArray)
		{
			mpArray = Alloc::allocate(mSize);
			memcpy(mpArray, other.mpArray, mSize * sizeof(T));
		}
		else
			mpArray = other.mpArray;

		return *this;
	}

	FixedArrayEx(FixedArrayEx&& other) noexcept
	{
		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		mpArray = other.mpArray;
		other.mOwnsArray = false;
		other.mSize = 0;
		other.mpArray = nullptr;
	}

	FixedArrayEx& operator=(FixedArrayEx&& other) noexcept
	{
		if (mOwnsArray)
			Alloc::deallocate(mpArray);

		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		mpArray = other.mpArray;
		other.mOwnsArray = false;
		other.mSize = 0;
		other.mpArray = nullptr;

		return *this;
	}

	FixedArrayEx& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	T& operator[](S i) const
	{
#if _DEBUG
		if (i >= mSize || i < 0)
			throw std::out_of_range("Array index is out of range");

		if (!mpArray)
			throw std::logic_error("Array pointer is null");
#endif

		return mpArray[i];
	}

	T* data()
	{
		return mpArray;
	}

	const T* data() const
	{
		return mpArray;
	}

	S size() const
	{
		return mSize;
	}

	bool empty() const
	{
		return mSize == 0;
	}

	explicit operator bool() const
	{
		return mSize != 0;
	}

	void reset()
	{
		if (mOwnsArray)
			Alloc::deallocate(mpArray);

		mpArray = nullptr;
		mSize = 0;
		mOwnsArray = false;
	}

	FixedArrayEx span(S offset, S size = std::numeric_limits<S>::max())
	{
		if (mSize == 0 || size <= 0)
			return nullptr;

		auto pArray = mpArray + offset;
		auto pArrayBound = mpArray + mSize;
		if (pArray < mpArray || pArrayBound <= pArray)
			return nullptr;
		auto maxSize = static_cast<S>(pArrayBound - pArray);

		size = std::min(size, maxSize);
		return FixedArrayEx(pArray, size, false);
	}

	const FixedArrayEx span(S offset, S size = std::numeric_limits<S>::max()) const
	{
		if (mSize == 0 || size <= 0)
			return nullptr;

		auto pArray = mpArray + offset;
		auto pArrayBound = mpArray + mSize;
		if (pArray < mpArray || pArrayBound <= pArray)
			return nullptr;
		auto maxSize = static_cast<S>(pArrayBound - pArray);

		size = std::min(size, maxSize);
		return FixedArrayEx(pArray, size, false);
	}

	FixedArrayEx getRef()
	{
		return FixedArrayEx(mpArray, mSize, false);
	}

	const FixedArrayEx getRef() const
	{
		return FixedArrayEx(mpArray, mSize, false);
	}

	S copyFrom(const FixedArrayEx& other) requires std::is_trivially_copyable_v<T>
	{
		S size = std::min(mSize, other.mSize);
		if (size == 0)
			return 0;

		memcpy(mpArray, other.mpArray, size * sizeof(T));
		return size;
	}

	S copyTo(FixedArrayEx& other) const requires std::is_trivially_copyable_v<T>
	{
		S size = std::min(mSize, other.mSize);
		if (size == 0)
			return 0;

		memcpy(other.mpArray, mpArray, size * sizeof(T));
		return size;
	}

	static FixedArrayEx CreateRef(T* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		return FixedArrayEx(pArray, n, false);
	}

	static const FixedArrayEx CreateRef(const T* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		return FixedArrayEx(const_cast<T*>(pArray), n, false);
	}

	template<typename E>
	static FixedArrayEx CreateRef(E* pArray, S n) requires SimilarIntegrals<T, E>
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<T*>(pArray);
		return FixedArrayEx(tArray, n, false);
	}

	template<typename E>
	static const FixedArrayEx CreateRef(const E* pArray, S n) requires SimilarIntegrals<T, E>
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<const T*>(pArray);
		return FixedArrayEx(const_cast<T*>(tArray), n, false);
	}

	static FixedArrayEx CreateRefUnsafe(void* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<T*>(pArray);
		return FixedArrayEx(tArray, n, false);
	}

	static const FixedArrayEx CreateRefUnsafe(const void* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<const T*>(pArray);
		return FixedArrayEx(const_cast<T*>(tArray), n, false);
	}

	static FixedArrayEx Copy(const T* pArray, S n) requires std::is_trivially_copyable_v<T>
	{
		if (n <= 0)
			return nullptr;

		auto out = FixedArrayEx(Alloc::allocate(n), n, true);
		memcpy(out.mpArray, pArray, n * sizeof(T));
		return out;
	}

	static FixedArrayEx Copy(const FixedArrayEx& array) requires std::is_trivially_copyable_v<T>
	{
		return Copy(array.data(), array.size());
	}

	/* Use this after upgrading from older version */
	/*
	void* operator&() = delete;
	const void* operator&() const = delete;
	*/
};

using FixedArrayExChar = FixedArrayEx<char, unsigned int>;
using FixedArrayExUChar = FixedArrayEx<unsigned char, unsigned int>;
using FixedArrayExCharS = FixedArrayEx<char, size_t>;
using FixedArrayExUCharS = FixedArrayEx<unsigned char, size_t>;
