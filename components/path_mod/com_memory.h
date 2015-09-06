#pragma once

void *Hunk_Alloc(int size, const char *name, int type);
void Z_Free(void *ptr);
void *Z_Malloc(int size);