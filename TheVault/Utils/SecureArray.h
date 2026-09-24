#pragma once
#include <sodium.h>
#include "Utility/FixedArray.h"

struct CryptoAllocator
{
	static uint8_t* allocate(size_t n)
	{
		auto ptr =  static_cast<uint8_t*>(sodium_malloc(n));
		if (!ptr)
			throw std::exception();
		return ptr;
	}

	static void deallocate(uint8_t* p)
	{
		sodium_free(p);
	}
};

typedef FixedArray<uint8_t, size_t, CryptoAllocator> SecureArray;
