#pragma once
#include <WTypes.h>
#include "vector.h"

//
// Clamp an integer value to the 0 - 255 range
//
int ClampByte(int v);

//
// Encode a float ( 0.0 - 1.0 ) to a BYTE ( 0 - 255 )
//
BYTE __cdecl EncodeFloatInByte(float flt);

WORD __cdecl EncodeFloatInWord(float flt);
