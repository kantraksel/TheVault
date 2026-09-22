#include "Files/FileStream.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

FileStream::FileStream() : size(0), filePtr(0)
{
	hFile = INVALID_HANDLE_VALUE;
}

FileStream::~FileStream()
{
	Close();
}

bool FileStream::OpenRead(const std::string_view& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	// WinAPI needs null-terminated string. std::string_view cannot guarantee it
	std::string str(name);
	return OpenRead(str);
}

bool FileStream::OpenRead(const std::wstring_view& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	// WinAPI needs null-terminated string. std::string_view cannot guarantee it
	std::wstring str(name);
	return OpenRead(str);
}

bool FileStream::OpenWrite(const std::string_view& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	// WinAPI needs null-terminated string. std::string_view cannot guarantee it
	std::string str(name);
	return OpenWrite(str);
}

bool FileStream::OpenWrite(const std::wstring_view& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	// WinAPI needs null-terminated string. std::string_view cannot guarantee it
	std::wstring str(name);
	return OpenWrite(str);
}

bool FileStream::OpenRead(const std::string& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	hFile = CreateFileA(name.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	size = 0;
	filePtr = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER lint;
		if (GetFileSizeEx(hFile, &lint))
			size = lint.QuadPart;
	}
	return hFile != INVALID_HANDLE_VALUE;
}

bool FileStream::OpenRead(const std::wstring& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	hFile = CreateFileW(name.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	size = 0;
	filePtr = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER lint;
		if (GetFileSizeEx(hFile, &lint))
			size = lint.QuadPart;
	}
	return hFile != INVALID_HANDLE_VALUE;
}

bool FileStream::OpenWrite(const std::string& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	hFile = CreateFileA(name.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	size = 0;
	filePtr = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER lint;
		if (GetFileSizeEx(hFile, &lint))
			size = lint.QuadPart;
	}
	return hFile != INVALID_HANDLE_VALUE;
}

bool FileStream::OpenWrite(const std::wstring& name)
{
	if (hFile != INVALID_HANDLE_VALUE)
		return false;

	hFile = CreateFileW(name.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	size = 0;
	filePtr = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER lint;
		if (GetFileSizeEx(hFile, &lint))
			size = lint.QuadPart;
	}
	return hFile != INVALID_HANDLE_VALUE;
}

bool FileStream::Close()
{
	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;
	size = 0;
	filePtr = 0;
	return true;
}

constexpr int64_t MaxUint32 = 0xFFFFFFFFll;

int64_t FileStream::Read(void* lpBuffer, int64_t nLength)
{
	nLength = std::min(nLength, size - filePtr);
	nLength = std::min(nLength, MaxUint32);

	DWORD bytesRead;
	if (!ReadFile(hFile, lpBuffer, (DWORD)nLength, &bytesRead, NULL))
	{
		LARGE_INTEGER lint;
		lint.QuadPart = filePtr;
		SetFilePointerEx(hFile, lint, NULL, FILE_BEGIN);
		return 0;
	}

	filePtr += bytesRead;
	return bytesRead;
}

bool FileStream::Write(const void* lpBuffer, int64_t nLength)
{
	if (nLength > MaxUint32)
		return false;

	DWORD bytesWritten;
	if (WriteFile(hFile, lpBuffer, (DWORD)nLength, &bytesWritten, NULL))
	{
		auto n = filePtr + nLength;
		if (size < n)
			size = n;
		filePtr = n;

		return true;
	}

	LARGE_INTEGER lint;
	lint.QuadPart = filePtr;
	SetFilePointerEx(hFile, lint, NULL, FILE_BEGIN);
	return false;
}

int64_t FileStream::Length()
{
	return size;
}

int64_t FileStream::Tell()
{
	return filePtr;
}

bool FileStream::Seek(int64_t position, Direction dir)
{
	switch (dir)
	{
		case Direction::Current:
		{
			position += filePtr;
			break;
		}

		case Direction::End:
		{
			position = size - position;
			break;
		}
	}

	LARGE_INTEGER lint;
	lint.QuadPart = position;
	if (SetFilePointerEx(hFile, lint, NULL, FILE_BEGIN))
	{
		filePtr = position;
		return true;
	}
	return false;
}

bool FileStream::Good()
{
	return hFile != INVALID_HANDLE_VALUE;
}
