#include "stdafx.h"

#include <WinInet.h>
#pragma comment(lib, "wininet.lib")

#include "jsoncpp/include/json/json.h"
#pragma comment(lib, "lib_json.lib")

#include <vector>

#include "shared_assert.h"

void printfd(const char *lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	char buffer[4096 * 4];
	int nBuf = _vsnprintf_s(buffer, ARRAYSIZE(buffer) - 1, lpszFormat, args);
	OutputDebugStringA(buffer);
	va_end(args);
}

std::vector<BYTE> DownloadFile(const wchar_t *fileURL)
{
	constexpr size_t CHUNK_SIZE = 1;

	HINTERNET hInternet = InternetOpen(L"Default_User_Agent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hFile = InternetOpenUrl(hInternet, GITHUB_RELEASE_URL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);

	std::vector<BYTE> data;
	data.reserve(CHUNK_SIZE);

	BYTE *buffer = new BYTE[CHUNK_SIZE];

	BOOL success = false;
	for (DWORD readCount = 0; success = InternetReadFile(hFile, buffer, CHUNK_SIZE, &readCount);)
	{
		if (!readCount)
			break;
		data.insert(data.end(), buffer, buffer + readCount);
	}

	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);

	if (!success)
	{
		printfd("ERROR: %d\n", GetLastError());
		return std::vector<BYTE>();
	}

	bool binary = false;
	if (!binary && data[data.size() - 1] != '\0')
	{
		data.push_back(0);
	}

	return data;
}

std::string DownloadReleaseJSon()
{
	auto data = DownloadFile(GITHUB_RELEASE_URL);
	return std::string((char *)data.data());
}

void debug_main()
{
	auto j = DownloadReleaseJSon();
	printfd("\n");

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(j, root))
		printfd("Couldn't parse JSon");

	ASSERT(root.isArray());
	printfd("FOUND %d RELEASES!\n", root.size());
	for (auto release : root)
	{
		auto name = release["name"];
		auto tag = release["tag_name"];

		std::string str = name.toStyledString();
		printfd("%s\n", str.c_str());
		MessageBoxA(0, str.c_str(), 0, 0);
	}

	
}
