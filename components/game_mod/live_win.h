#pragma once

struct bdReferencableVtbl;
struct bdByteBuffer;
struct bdTaskResultVtbl;

struct bdTaskResult
{
  bdTaskResultVtbl *vfptr;
};

struct bdSecurityID
{
  char ab[8];
};

struct bdMatchMakingInfo
{
  bdTaskResult baseclass_0;
  bdSecurityID m_sessionID;
  char m_hostAddr[255];
  unsigned int m_hostAddrSize;
  unsigned int m_gameType;
  unsigned int m_maxPlayers;
  unsigned int m_numPlayers;
};

struct MatchMakingInfo
{
  bdMatchMakingInfo baseclass_0;
  int m_memberNETCODE_VERSION;
  char m_membersecKey[17];
  char m_memberservername[65];
  unsigned __int64 m_membermaprotation;
  char m_membermapname[33];
  int m_memberGAME_TYPE;
  int m_memberPLAYLIST;
  int m_memberPLAYLIST_VERSION;
  int m_memberPARTY_STATE;
  int m_memberMAPPACKS;
  int m_memberGAME_MODE;
  int m_memberSKILL;
  int m_memberTEAM_SIZE_MIN;
  int m_memberTEAM_SIZE_MAX;
  int m_memberGEOGRAPHICAL_LOCATION;
  unsigned __int64 m_memberdemonwareID;
  int m_memberserverType;
  int m_memberroundTime;
  char m_membermod[33];
  unsigned __int64 m_memberaddressport;
  unsigned int m_memberDDL_VERSION;
  unsigned int m_memberLICENSE;
  unsigned int m_pingedtime;
  int m_dirty;
  int m_active;
  int m_membertimesincelastupdate;
};

extern const int g_protocolVersion;

typedef void __cdecl Session_Modify_t(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots);
extern Session_Modify_t* Session_Modify;

int SV_UpdateCategory();

void __cdecl Session_Modify_Fix(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots);
void Live_JoinSessionInProgressComplete_CheckMod();
void Live_AcceptInviteAsyncComplete_CheckMod();