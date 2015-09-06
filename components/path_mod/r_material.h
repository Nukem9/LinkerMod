#pragma once

static int gfxImageTextureTableSize		= 32768;
static GfxImage **gfxImageTextureTable	= (GfxImage **)0x01547F28;

static int *materialVertexDeclCount								= (int *)0x01567F48;
static MaterialVertexDeclaration *materialVertexDeclarations	= (MaterialVertexDeclaration *)0x01567F4C;

void *Material_Alloc(int size);
void Material_ReloadTextures();