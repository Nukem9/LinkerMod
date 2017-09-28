#pragma once

struct bdByteBuffer;

template<typename T>
struct bdReference
{
	T *m_ptr;
};

struct bdTaskResult
{
	virtual void __vecDelDtor(unsigned int);
	virtual bool deserialize(bdReference<bdByteBuffer> buffer) = 0;
	virtual unsigned int sizeOf() = 0;
};

struct bdSecurityID
{
	char ab[8];
};

struct bdSessionID : bdTaskResult
{
	virtual bool deserialize(bdReference<bdByteBuffer> buffer) override;
	virtual unsigned int sizeOf() override;
	virtual void serialize(struct bdByteBuffer& buffer);

	bdSecurityID m_sessionID;
};
STATIC_ASSERT_OFFSET(bdSessionID, m_sessionID, 0x4);
STATIC_ASSERT_SIZE(bdSessionID, 0xC);

struct bdMatchMakingInfo : bdTaskResult
{
	virtual bool deserialize(bdReference<bdByteBuffer> buffer) override;
	virtual unsigned int sizeOf() override;
	virtual void serialize(struct bdByteBuffer& buffer);

	bdSecurityID m_sessionID;
	char m_hostAddr[255];
	unsigned int m_hostAddrSize;
	unsigned int m_gameType;
	unsigned int m_maxPlayers;
	unsigned int m_numPlayers;
};
STATIC_ASSERT_OFFSET(bdMatchMakingInfo, m_hostAddr, 0xC);
STATIC_ASSERT_OFFSET(bdMatchMakingInfo, m_hostAddrSize, 0x10C);
STATIC_ASSERT_OFFSET(bdMatchMakingInfo, m_gameType, 0x110);
STATIC_ASSERT_OFFSET(bdMatchMakingInfo, m_maxPlayers, 0x114);

struct MatchMakingInfo : bdMatchMakingInfo
{
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
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_memberNETCODE_VERSION, 0x11C);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_membersecKey, 0x120);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_memberservername, 0x131);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_membermaprotation, 0x178);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_membermapname, 0x180);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_memberGAME_TYPE, 0x1A4);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_membermod, 0x1E0);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_memberDDL_VERSION, 0x210);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_memberLICENSE, 0x214);
STATIC_ASSERT_OFFSET(MatchMakingInfo, m_active, 0x220);
STATIC_ASSERT_SIZE(MatchMakingInfo, 0x228);

struct invite_t
{
	unsigned __int64 from;
	bdSessionID sessionID;
	char password[32];
};
STATIC_ASSERT_OFFSET(invite_t, from, 0x0);
STATIC_ASSERT_OFFSET(invite_t, sessionID, 0x8);
STATIC_ASSERT_OFFSET(invite_t, password, 0x14);
STATIC_ASSERT_SIZE(invite_t, 0x38);

extern const int g_protocolVersion;

typedef void __cdecl Session_Modify_t(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots);
extern Session_Modify_t* Session_Modify;

int SV_UpdateCategory();

bool Com_SessionMode_IsZombiesGame();
void __cdecl Session_Modify_Fix(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots);
void Live_JoinSessionInProgressComplete_CheckMod();
void Live_AcceptInviteAsyncComplete_CheckMod();