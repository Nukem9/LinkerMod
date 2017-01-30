#pragma once
#include <WTypes.h>
#include <iostream>

template<typename T>
class ForeignPointer
{
private: 
	T content;
public:
	T* pAddress;

	bool operator==(void* address) {return pAddress == address};

	T operator*(void) const;
	T operator[](int index) const;
	T* operator->(void) const;

	ForeignPointer<T>(void) {};
	ForeignPointer<T>(T* address) : pAddress(address) {};
	//ForeignPointer<T>(ForeignPointer<T>& arg) : pAddress(arg.pAddress) {};
	//ForeignPointer<T>(void* address) : pAddress(address) {};
	~ForeignPointer<T>(void) {};
};

#include "process_info.h"

template<typename T>
T ForeignPointer<T>::operator*(void) const
{
	T out;
	SIZE_T numofbytesread = 0;
	ReadProcessMemory(g_process.handle, pAddress, &out, sizeof(T), &numofbytesread);
	return out;
};

template<typename T>
T ForeignPointer<T>::operator[](int index) const
{
	T out;
	SIZE_T numofbytesread = 0;
	ReadProcessMemory(g_process.handle, pAddress + index, &out, sizeof(T), &numofbytesread);
	return out;
};

template<typename T>
T* ForeignPointer<T>::operator->(void) const
{
	SIZE_T numofbytesread;
	ReadProcessMemory(g_process.handle, (void*)pAddress, (void*)&content, sizeof(T), &numofbytesread);
	return (T*)&content;
}
