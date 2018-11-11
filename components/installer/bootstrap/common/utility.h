#pragma once
#include <string>
#include <algorithm>
#include <cctype>

template<class _Elem,
	class _Traits = std::char_traits<_Elem>,
	class _Alloc = std::allocator<_Elem>>
static inline std::basic_string<_Elem, _Traits, _Alloc> str_toupper(std::basic_string<_Elem, _Traits, _Alloc> str) {
	std::transform(str.begin(), str.end(), str.begin(),
		[](_Elem c) { return std::toupper(c); }
	);
	return str;
}

std::string str_toupper(const char* pStr);

std::string va(const char* fmt, ...);
std::string va(const char* fmt, va_list args);

template <typename T>
static auto AutoZeroMemory(T &obj)
{
	return ZeroMemory(&obj, sizeof(T));
}
