#pragma once
#include "global.h"

template<typename T>
class ForeignPointer
{
public:
	T* pAddress;

	bool operator==(void* address) {return pAddress == address};

	T operator*(void) const
	{
		T out;
		SIZE_T numofbytesread = 0;
		ReadProcessMemory(g_hProcess,pAddress,&out,sizeof(T),&numofbytesread);
		return out;
	};
	
	T operator[](int index) const
	{
		T out;
		SIZE_T numofbytesread = 0;
		ReadProcessMemory(g_hProcess,pAddress + index,&out,sizeof(T),&numofbytesread);
		return out;
	};

	T* operator->(void) const
	{
		return (T*)pAddress;
	}

	ForeignPointer<T>(void) {};
	ForeignPointer<T>(void* address) : pAddress(address) {};
	ForeignPointer<T>(T* address) : pAddress(address) {};
	~ForeignPointer<T>(void) {};
};

