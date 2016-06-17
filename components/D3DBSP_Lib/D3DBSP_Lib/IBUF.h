#pragma once

#include <WTypes.h>

class IBUF
{
private:
	BYTE* pBuf;
	unsigned int len;

public:
	IBUF(BYTE* buf);
	void write(void* data, unsigned int size);
};
