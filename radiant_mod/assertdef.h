#pragma once

#define ASSERT(cond) if(!(cond)) assert(__FILE__, __LINE__, (cond), #cond);

void assert(const char *File, int Line, bool Condition, const char *Format, ...);