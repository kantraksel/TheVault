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
class FixedArray
{
private:
	T* mpArray;
	S mSize;
	bool mOwnsArray;

public:
	FixedArray() : mpArray(nullptr), mSize(0), mOwnsArray(false)
	{
	}

	FixedArray(std::nullptr_t) : mpArray(nullptr), mSize(0), mOwnsArray(false)
	{
	}

	FixedArray(S n)
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

	~FixedArray()
	{
		if (mOwnsArray)
			Alloc::deallocate(mpArray);
	}

private:
	FixedArray(T* pArray, S n, bool ownsArray) : mpArray(pArray), mSize(n), mOwnsArray(ownsArray)
	{
	}

public:
	FixedArray(const FixedArray& other) requires std::is_trivially_copyable_v<T>
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

	FixedArray& operator=(const FixedArray& other) requires std::is_trivially_copyable_v<T>
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

	FixedArray(FixedArray&& other) noexcept
	{
		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		mpArray = other.mpArray;
		other.mOwnsArray = false;
		other.mSize = 0;
		other.mpArray = nullptr;
	}

	FixedArray& operator=(FixedArray&& other) noexcept
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

	FixedArray& operator=(std::nullptr_t)
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

	FixedArray span(S offset, S size = std::numeric_limits<S>::max())
	{
		if (mSize == 0 || size <= 0)
			return nullptr;

		auto pArray = mpArray + offset;
		auto pArrayBound = mpArray + mSize;
		if (pArray < mpArray || pArrayBound <= pArray)
			return nullptr;
		auto maxSize = static_cast<S>(pArrayBound - pArray);

		size = std::min(size, maxSize);
		return FixedArray(pArray, size, false);
	}

	const FixedArray span(S offset, S size = std::numeric_limits<S>::max()) const
	{
		if (mSize == 0 || size <= 0)
			return nullptr;

		auto pArray = mpArray + offset;
		auto pArrayBound = mpArray + mSize;
		if (pArray < mpArray || pArrayBound <= pArray)
			return nullptr;
		auto maxSize = static_cast<S>(pArrayBound - pArray);

		size = std::min(size, maxSize);
		return FixedArray(pArray, size, false);
	}

	FixedArray getRef()
	{
		return FixedArray(mpArray, mSize, false);
	}

	const FixedArray getRef() const
	{
		return FixedArray(mpArray, mSize, false);
	}

	S copyFrom(const FixedArray& other) requires std::is_trivially_copyable_v<T>
	{
		S size = std::min(mSize, other.mSize);
		if (size == 0)
			return 0;

		memcpy(mpArray, other.mpArray, size * sizeof(T));
		return size;
	}

	S copyTo(FixedArray& other) const requires std::is_trivially_copyable_v<T>
	{
		S size = std::min(mSize, other.mSize);
		if (size == 0)
			return 0;

		memcpy(other.mpArray, mpArray, size * sizeof(T));
		return size;
	}

	static FixedArray CreateRef(T* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		return FixedArray(pArray, n, false);
	}

	static const FixedArray CreateRef(const T* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		return FixedArray(const_cast<T*>(pArray), n, false);
	}

	template<typename E>
	static FixedArray CreateRef(E* pArray, S n) requires SimilarIntegrals<T, E>
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<T*>(pArray);
		return FixedArray(tArray, n, false);
	}

	template<typename E>
	static const FixedArray CreateRef(const E* pArray, S n) requires SimilarIntegrals<T, E>
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<const T*>(pArray);
		return FixedArray(const_cast<T*>(tArray), n, false);
	}

	static FixedArray CreateRefUnsafe(void* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<T*>(pArray);
		return FixedArray(tArray, n, false);
	}

	static const FixedArray CreateRefUnsafe(const void* pArray, S n)
	{
		if (n <= 0)
			return nullptr;
		auto tArray = reinterpret_cast<const T*>(pArray);
		return FixedArray(const_cast<T*>(tArray), n, false);
	}

	static FixedArray Copy(const T* pArray, S n) requires std::is_trivially_copyable_v<T>
	{
		if (n <= 0)
			return nullptr;

		auto out = FixedArray(Alloc::allocate(n), n, true);
		memcpy(out.mpArray, pArray, n * sizeof(T));
		return out;
	}

	static FixedArray Copy(const FixedArray& array) requires std::is_trivially_copyable_v<T>
	{
		return Copy(array.data(), array.size());
	}

	/* Use this after upgrading from older version */
	/*
	void* operator&() = delete;
	const void* operator&() const = delete;
	*/
};

using FixedArrayChar = FixedArray<char, unsigned int>;
using FixedArrayUChar = FixedArray<unsigned char, unsigned int>;
using FixedArrayCharS = FixedArray<char, size_t>;
using FixedArrayUCharS = FixedArray<unsigned char, size_t>;
