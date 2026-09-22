#include "Files/FileWriter.h"
#include "Systems/FileSystem.h"
#include "Utility/StringUtils.h"

FileWriter::FileWriter() : hFile(nullptr)
{
}

FileWriter::FileWriter(FSStream* stream) : hFile(stream)
{
}

FileWriter::~FileWriter()
{
	Close();
}

bool FileWriter::Open(const std::string_view& name)
{
	if (!FileSystem::IsGlobalPresent())
		return false;

	auto* stream = FileSystem::GetGlobal().OpenFileWrite(name);
	if (!stream)
		return false;

	hFile = stream;
	return true;
}

bool FileWriter::Open(const std::wstring_view& name)
{
	return Open(StringUtils::WideStringToUtf8(name));
}

void FileWriter::Close()
{
	if (hFile)
	{
		hFile->Close();
		hFile = nullptr;
	}
}

bool FileWriter::IsOpen()
{
	return hFile != nullptr;
}

bool FileWriter::Write(const void* lpBuffer, unsigned int nLength)
{
	return hFile->Write(lpBuffer, nLength);
}

unsigned long long FileWriter::Length()
{
	return hFile->Length();
}

unsigned long long FileWriter::Tell()
{
	return hFile->Tell();
}

bool FileWriter::Seek(unsigned long long position, Direction dir)
{
	static_assert((int)Direction::Begin == 0);
	static_assert((int)Direction::Current == 1);
	static_assert((int)Direction::End == 2);

	return hFile->Seek(position, (int)dir);
}
