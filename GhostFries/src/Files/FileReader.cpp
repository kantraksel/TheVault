#include "Files/FileReader.h"
#include "Systems/FileSystem.h"
#include "Utility/StringUtils.h"

FileReader::FileReader() : hFile(nullptr)
{
}

FileReader::FileReader(FSStream* stream) : hFile(stream)
{
}

FileReader::~FileReader()
{
	Close();
}

bool FileReader::Open(const std::string_view& name)
{
	if (!FileSystem::IsGlobalPresent())
		return false;

	auto* stream = FileSystem::GetGlobal().OpenFile(name);
	if (!stream)
		return false;

	hFile = stream;
	return true;
}

bool FileReader::Open(const std::wstring_view& name)
{
	return Open(StringUtils::WideStringToUtf8(name));
}

void FileReader::Close()
{
	if (hFile)
	{
		hFile->Close();
		hFile = nullptr;
	}
}

bool FileReader::IsOpen()
{
	return hFile != nullptr;
}

unsigned int FileReader::Read(void* lpBuffer, unsigned int nLength)
{
	return hFile->Read(lpBuffer, nLength);
}

unsigned long long FileReader::Length()
{
	return hFile->Length();
}

unsigned long long FileReader::Tell()
{
	return hFile->Tell();
}

bool FileReader::Seek(unsigned long long position, Direction dir)
{
	static_assert((int)Direction::Begin == 0);
	static_assert((int)Direction::Current == 1);
	static_assert((int)Direction::End == 2);

	return hFile->Seek(position, (int)dir);
}
