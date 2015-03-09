#pragma once
#include "global.h"

extern HANDLE g_hProcess;

template<typename T>
class ForeignPointer
{
private: 
	T content;
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
		SIZE_T numofbytesread;
		ReadProcessMemory(g_hProcess,(void*)pAddress,(void*)&content,sizeof(T),&numofbytesread);
		return (T*)&content;
	}

	ForeignPointer<T>(void) {};
	ForeignPointer<T>(T* address) : pAddress(address) {};
	//ForeignPointer<T>(ForeignPointer<T>& arg) : pAddress(arg.pAddress) {};
	//ForeignPointer<T>(void* address) : pAddress(address) {};
	~ForeignPointer<T>(void) {};
};

