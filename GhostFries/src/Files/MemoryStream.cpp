#include <memory>
#include "Files/MemoryStream.h"

MemoryStream::MemoryStream(void* pData, size_t size) : mpData((char*)pData), mSize(size), mPtr(0)
{
}

size_t MemoryStream::Write(const void* data, size_t size)
{
	if (!data)
		return 0;

	auto maxsize = mSize - mPtr;
	if (size > maxsize)
		size = maxsize;

	memcpy(mpData + mPtr, data, size);
	mPtr += size;
	return size;
}

size_t MemoryStream::Read(void* data, size_t size)
{
	if (!data)
		return 0;

	auto maxsize = mSize - mPtr;
	if (size > maxsize)
		size = maxsize;

	memcpy(data, mpData + mPtr, size);
	mPtr += size;
	return size;
}

void MemoryStream::Seek(ptrdiff_t n)
{
	auto max = ptrdiff_t(mSize - mPtr);
	if (n > max)
		n = max;
	
	auto ptr = ptrdiff_t(mPtr) + n;
	if (ptr < 0)
		mPtr = 0;
	else
		mPtr = (size_t)ptr;
}

void MemoryStream::ResetPos()
{
	mPtr = 0;
}
