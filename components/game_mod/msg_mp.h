#pragma once

enum netadrtype_t
{
	NA_BOT = 0x0,
	NA_BAD = 0x1,
	NA_LOOPBACK = 0x2,
	NA_BROADCAST = 0x3,
	NA_IP = 0x4,
};

struct netadr_t
{
	netadrtype_t type;
	char ip[4];
	unsigned __int16 port;
	int addrHandleIndex;
};
STATIC_ASSERT_OFFSET(netadr_t, ip, 0x4);
STATIC_ASSERT_SIZE(netadr_t, 0x10);

enum netsrc_t
{
	NS_CLIENT1 = 0x0,
	NS_SERVER = 0x1,
	NS_MAXCLIENTS = 0x1,
	NS_PACKET = 0x2,
};

struct msg_t
{
	int overflowed;
	int readOnly;
	char *data;
	char *splitData;
	int maxsize;
	int cursize;
	int splitSize;
	int readcount;
	int bit;
	int lastEntityRef;
	int flush;
	netsrc_t targetLocalNetID;
};

VANILLA_FUNC(MSG_Init, void(__cdecl*)(msg_t *buf, char *data, int length), 0x00622700);
VANILLA_FUNC(MSG_WriteBit0, void(__cdecl*)(msg_t *msg), 0x004475B0);
VANILLA_FUNC(MSG_WriteBit1, void(__cdecl*)(msg_t *msg), 0x004244D0);
VANILLA_FUNC(MSG_WriteBits, void(__cdecl*)(msg_t *msg, int value, int bits), 0x0045DA90);
VANILLA_FUNC(MSG_WriteByte, void(__cdecl*)(msg_t *msg, int c), 0x004EFD70);
VANILLA_FUNC(MSG_WriteShort, void(__cdecl*)(msg_t *msg, int c), 0x005F4710);
VANILLA_FUNC(MSG_WriteLong, void(__cdecl*)(msg_t *msg, int c), 0x0048E070);

VANILLA_FUNC(MSG_ReadByte, int(__cdecl*)(msg_t *msg), 0x00626990);
VANILLA_FUNC(MSG_ReadShort, int(__cdecl*)(msg_t *msg), 0x005F46D0);
VANILLA_FUNC(MSG_ReadData, int(__cdecl*)(msg_t *msg, void *data, int len), 0x00519DD0);

// MSG_WriteString takes up 1 to 32 bytes depending on the length of the string
VANILLA_FUNC(MSG_WriteString, void(__cdecl*)(msg_t *sb, const char *s), 0x00430420);

VANILLA_FUNC(MSG_WriteBigString, void(__cdecl*)(msg_t *sb, const char *s), 0x006465B0);
VANILLA_FUNC(MSG_WriteBitsCompress, int(__cdecl*)(bool trainHuffman, const char *from, int fromSizeBytes, char *to, int toSizeBytes), 0x00403110);

VANILLA_FUNC(MSG_ClearLastReferencedEntity, void(__cdecl*)(msg_t *msg), 0x0063A210);
VANILLA_FUNC(MSG_WriteEntity, int(__cdecl*)(struct SnapshotInfo_s *snapInfo, msg_t *msg, const int time, struct entityState_s *from, struct entityState_s *to, int flags), 0x00531680);

static bool NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *data)
{
	return ((bool(__cdecl *)(netsrc_t, netadr_t, const char *))0x00472850)(sock, adr, data);
}