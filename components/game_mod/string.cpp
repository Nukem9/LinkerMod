#include "stdafx.h"

bool Str_StartsWith(const char *str, size_t str_len, const char *sub, size_t sub_len)
{
	if (!str || !sub)
		return false;
	if (str_len < sub_len)
		return false;

	return strncmp(str, sub, sub_len) == 0;
}

bool Str_EndsWith(const char *str, size_t str_len, const char *sub, size_t sub_len)
{
	if (!str || !sub)
		return false;
	if (str_len < sub_len)
		return false;

	const auto offset = (str_len - sub_len);
	return strncmp(str + offset, sub, sub_len);
}

bool Str_IStartsWith(const char *str, size_t str_len, const char *sub, size_t sub_len)
{
	if (!str || !sub)
		return false;
	if (str_len < sub_len)
		return false;

	return _strnicmp(str, sub, sub_len) == 0;
}

bool Str_IEndsWith(const char *str, size_t str_len, const char *sub, size_t sub_len)
{
	if (!str || !sub)
		return false;
	if (str_len < sub_len)
		return false;

	const auto offset = (str_len - sub_len);
	return _strnicmp(str + offset, sub, sub_len);
}
