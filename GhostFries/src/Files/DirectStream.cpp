#include "Files/DirectStream.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

DirectStream::DirectStream()
{
	hFile = INVALID_HANDLE_VALUE;
}

DirectStream::~DirectStream()
{
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
}

void DirectStream::Open(void* hFile)
{
	this->hFile = hFile;
}

void DirectStream::Close()
{
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

bool DirectStream::IsOpen()
{
	return hFile != INVALID_HANDLE_VALUE;
}

unsigned int DirectStream::Read(void* lpBuffer, unsigned int nLength)
{
	DWORD bytesRead;
	if (ReadFile(hFile, lpBuffer, nLength, &bytesRead, NULL))
		return bytesRead;
	return 0;
}

bool DirectStream::Write(const void* lpBuffer, unsigned int nLength)
{
	DWORD bytesWritten;
	if (WriteFile(hFile, lpBuffer, nLength, &bytesWritten, NULL))
		return bytesWritten == nLength;
	return false;
}

unsigned long long DirectStream::Length()
{
	LARGE_INTEGER size;
	if (GetFileSizeEx(hFile, &size))
		return size.QuadPart;
	return 0;
}

unsigned long long DirectStream::Tell()
{
	LARGE_INTEGER dist{};
	LARGE_INTEGER ptr{};
	if (SetFilePointerEx(hFile, dist, &ptr, FILE_CURRENT))
		return ptr.QuadPart;
	return 0;
}

bool DirectStream::Seek(unsigned long long position, int dir)
{
	static_assert(FILE_BEGIN == 0);
	static_assert(FILE_CURRENT == 1);
	static_assert(FILE_END == 2);

	LARGE_INTEGER dist;
	dist.QuadPart = position;
	if (SetFilePointerEx(hFile, dist, nullptr, dir))
		return true;
	return false;
}
