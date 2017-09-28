#pragma once

client_t *SV_FindClientByAddress(netadr_t from, int qport);
void LiveStats_DecompressStats(char *statsDestBuffer, int numCompressedPackets);
bool SV_DoWeHaveAllStatsPackets(client_t *cl, int numCompressedPackets);
void SV_ReceiveStats(netadr_t addr, msg_t *msg);
void SV_CheckTimeouts();
void SV_PostFrame(bool FrameTimeout);

void mfh_PartyHost_StartMatch(void);
void mfh_SV_SpawnServer(void);
