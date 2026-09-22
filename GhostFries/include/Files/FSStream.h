#pragma once

class FSStream
{
public:
	virtual void Close() = 0;

	virtual unsigned int Read(void* lpBuffer, unsigned int nLength) = 0;
	virtual unsigned long long Length() = 0;
	virtual bool Write(const void* lpBuffer, unsigned int nLength) = 0;

	virtual unsigned long long Tell() = 0;
	virtual bool Seek(unsigned long long position, int dir) = 0;
};
