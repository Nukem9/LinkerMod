#include "stdafx.h"

EResult LiveSteamClient::rawTicketResult;

// /live/live_steam_client.cpp:18
void LiveSteamClient::RequestEncryptedAppTicket(const void *data, const unsigned int size)
{
	ASSERT_MSG(false, "Function not implemented");
}

// /live/live_steam_client.cpp:25
bool LiveSteamClient::GetRetrievedEncryptedAppTicket(void *ticketBuf, const unsigned int ticketBufSize, unsigned int *ticketSize)
{
	ASSERT(resultOnRequestEncryptedAppTicket == k_EResultOK && ticketBufSize);

	// SteamUser()->GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket);
	auto steamUser = (*(void *(__cdecl **)())0x009A356C)();
	auto getEncryptedAppTicket = *(bool(__thiscall **)(void *, void *, int, unsigned int *))(*(DWORD *)steamUser + 80);

	// Use the real function if Steam had a good ticket
	if (this->rawTicketResult == k_EResultOK)
	{
		if (getEncryptedAppTicket(steamUser, ticketBuf, ticketBufSize, ticketSize))
		{
			// Write generated ticket & DW cookie
			if (UseCachedTicket("players/ticket.bin", ticketBuf, *ticketSize, false) &&
				UseCachedTicket("players/cookie.bin", g_authService->m_steamCookieKey, sizeof(g_authService->m_steamCookieKey), false))
			{
				Com_DPrintf(1, "STEAM: Wrote cached auth ticket\n");
			}

			Com_DPrintf(1, "STEAM: Retrieved ticket from Steam, sending to DemonWare\n");
			return true;
		}
	}
	else
	{
		// Read Steam generated ticket
		if (!UseCachedTicket("players/ticket.bin", ticketBuf, ticketBufSize, true, ticketSize))
			return false;

		// Read Demonware generated cookie
		if (!UseCachedTicket("players/cookie.bin", g_authService->m_steamCookieKey, sizeof(g_authService->m_steamCookieKey)))
			return false;

		return true;
	}

	Com_DPrintf(1, "STEAM: Failed to retrieve ticket\n");
	return false;
}

// /live/live_steam_client.cpp:33
void LiveSteamClient::OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure)
{
	if (bIOFailure)
		return;

	this->rawTicketResult = pEncryptedAppTicketResponse->m_eResult;

	switch (pEncryptedAppTicketResponse->m_eResult)
	{
	case k_EResultOK:
		dwLogonSeAcquiredSteamTicket();
		break;
	case k_EResultLimitExceeded:
		Com_DPrintf(1, "STEAM: Calling RequestEncryptedAppTicket more than once per minute returns this error\n");
		break;
	case k_EResultDuplicateRequest:
		Com_DPrintf(1, "STEAM: Calling RequestEncryptedAppTicket while there is already a pending request results in this error\n");
		break;
	case k_EResultNoConnection:
		Com_DPrintf(1, "STEAM: Calling RequestEncryptedAppTicket while not connected to steam results in this error\n");
		break;
	}

	// If the request was rate limited, check if there's a cached ticket
	if (pEncryptedAppTicketResponse->m_eResult == k_EResultLimitExceeded)
	{
		if (UseCachedTicket("players/ticket.bin", nullptr, 0))
		{
			Com_DPrintf(1, "STEAM: Attempting to use cached auth ticket...\n");

			pEncryptedAppTicketResponse->m_eResult = k_EResultOK;
			dwLogonSeAcquiredSteamTicket();
		}
	}

	this->resultOnRequestEncryptedAppTicket = pEncryptedAppTicketResponse->m_eResult;
}

// /live/live_steam_client.cpp:???
bool LiveSteamClient::UseCachedTicket(const char *Path, void *Buffer, unsigned int BufferSize, bool ReadOnly, unsigned int *ReadSize)
{
	// Check if file can be opened at all
	FILE *f = nullptr;

	if (fopen_s(&f, Path, ReadOnly ? "rb" : "wb") != 0)
		return false;

	// Read only
	if (ReadOnly)
	{
		fseek(f, 0, SEEK_END);
		size_t len = ftell(f);
		fseek(f, 0, SEEK_SET);

		// Limit read length to buffer size
		if (Buffer)
			len = min(len, BufferSize);

		if (ReadSize)
			*ReadSize = len;

		// Do the actual reading
		if (Buffer)
		{
			if (fread(Buffer, 1, len, f) != len)
			{
				// Didn't read the correct length. Failed.
				fclose(f);
				return false;
			}
		}

		fclose(f);
		return true;
	}

	// Write only
	{
		// If (invalid arguments) || (file write failed)
		if ((!Buffer || BufferSize == 0) ||
			(fwrite(Buffer, 1, BufferSize, f) != BufferSize))
		{
			fclose(f);
			return false;
		}
	}

	fclose(f);
	return true;
}