#pragma once

enum EResult
{
	k_EResultOK = 1,
	k_EResultNoConnection = 3,
	k_EResultLimitExceeded = 25,
	k_EResultDuplicateRequest = 29,
};

struct EncryptedAppTicketResponse_t
{
	EResult m_eResult;
};
STATIC_ASSERT_OFFSET(EncryptedAppTicketResponse_t, m_eResult, 0x0);

class LiveSteamClient
{
//private:
//	CCallResult<LiveSteamClient, EncryptedAppTicketResponse_t> m_SteamCallResultEncryptedAppTicket;

public:
	EResult resultOnRequestEncryptedAppTicket;
	static EResult rawTicketResult;

	LiveSteamClient() = delete;

	void RequestEncryptedAppTicket(const void *data, const unsigned int size);
	bool GetRetrievedEncryptedAppTicket(void *ticketBuf, const unsigned int ticketBufSize, unsigned int *ticketSize);
	void OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure);
};
STATIC_ASSERT_OFFSET(LiveSteamClient, resultOnRequestEncryptedAppTicket, 0x0);