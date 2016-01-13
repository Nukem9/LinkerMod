#pragma once

void hk_R_Cinematic_BinkOpen(void);
bool __cdecl R_Cinematic_BinkOpen(const char *filename, unsigned int playbackFlags, char *errText);

bool __cdecl R_Cinematic_BinkOpenPath(const char *filepath, unsigned int playbackFlags, char *errText);
