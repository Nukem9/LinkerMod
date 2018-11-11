#include "stdafx.h" // TODO: fix this
#include "utility.h"

std::string str_toupper(const char* pStr) {
	std::string str = pStr;
	std::transform(str.begin(), str.end(), str.begin(),
		[](std::string::value_type c) { return std::toupper(c); }
	);
	return str;
}

std::string va(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	auto result = va(fmt, args);
	va_end(args);
	return result;
}

std::string va(const char* fmt, va_list args) {
	std::string result(128, '\0');
	size_t len = vsnprintf((char*)result.data(), 0, fmt, args);

	if (len < 0)
		throw "Invalid string";

	result.resize(len + 1);

	len = vsnprintf((char*)result.data(), len + 1, fmt, args);

	if (len < 0)
		throw "Unable to generate string";

	return result;
}
