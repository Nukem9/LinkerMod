#include "IBuf.h"

IBUF::IBUF(BYTE* buf) : pBuf(buf)
{
	len = 0;
};

void IBUF::write(void* data, unsigned int size)
{
	memcpy(pBuf + len, data, size);
	len += size;
};
