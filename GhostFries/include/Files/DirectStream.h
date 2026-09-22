#pragma once
#include "FSStream.h"
#include "Utility/Base.h"

class DirectStream : LockedObject, public FSStream
{
private:
	void* hFile;

public:
	DirectStream();
	~DirectStream();

	void Open(void* hFile);
	void Close() override;
	bool IsOpen();

	unsigned int Read(void* lpBuffer, unsigned int nLength) override;
	bool Write(const void* lpBuffer, unsigned int nLength) override;
	unsigned long long Length() override;

	unsigned long long Tell() override;
	bool Seek(unsigned long long position, int dir) override;
};
