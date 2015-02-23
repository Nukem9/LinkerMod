#pragma once

static int gfxImageTextureTableSize		= 32768;
static GfxImage **gfxImageTextureTable	= (GfxImage **)0x01547F28;

void Material_ReloadTextures();