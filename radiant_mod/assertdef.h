#pragma once

#define ASSERT(cond) if(!(cond)) assert(__FILE__, __LINE__, #cond);

void assert(const char *File, int Line, const char *Format, ...);