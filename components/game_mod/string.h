#pragma once

//
// Macro used for easily defining various template overloads for a given function that takes two strings
//
#define DEFINE_FUNC_TEMPLATE_OVERLOADS(FUNC)                                  \
	template <size_t str_len, size_t sub_len>                                 \
	inline bool FUNC(char(&str)[str_len], char(&sub)[sub_len])                \
	{                                                                         \
		return FUNC(str, str_len, sub, sub_len);                              \
	}                                                                         \
                                                                              \
	template <size_t str_len>                                                 \
	inline bool FUNC(char(&str)[str_len], const char *sub, size_t sub_len)    \
	{                                                                         \
		return FUNC(str, str_len, sub, sub_len);                              \
	}                                                                         \
                                                                              \
	template <size_t sub_len>                                                 \
	inline bool FUNC(const char *string, size_t str_len, char(&sub)[sub_len]) \
	{                                                                         \
		return FUNC(str, str_len, sub, sub_len);                              \
	}

bool Str_StartsWith(const char *string, size_t str_len, const char *sub, size_t sub_len);
DEFINE_FUNC_TEMPLATE_OVERLOADS(Str_StartsWith);

bool Str_EndsWith(const char *string, size_t str_len, const char *sub, size_t sub_len);
DEFINE_FUNC_TEMPLATE_OVERLOADS(Str_EndsWith);

bool Str_IStartsWith(const char *string, size_t str_len, const char *sub, size_t sub_len);
DEFINE_FUNC_TEMPLATE_OVERLOADS(Str_IStartsWith);

bool Str_IEndsWith(const char *string, size_t str_len, const char *sub, size_t sub_len);
DEFINE_FUNC_TEMPLATE_OVERLOADS(Str_IEndsWith);

#undef DEFINE_FUNC_TEMPLATE_VARIANTS
