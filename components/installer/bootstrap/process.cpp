#include "stdafx.h"
#include "process.h"

static BOOL CloseHandle_s(HANDLE &h)
{
	if (h == NULL)
		return TRUE;
	const auto result = CloseHandle(h);
	h = NULL;
	return result;
}

Pipe::Pipe(bool inheritHandle) {
	SECURITY_ATTRIBUTES pipeAttr;
	AutoZeroMemory(pipeAttr);
	pipeAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	pipeAttr.bInheritHandle = inheritHandle;
	pipeAttr.lpSecurityDescriptor = NULL;

	// We currently don't use the _hWrite handle for anything
	// but it is *REQUIRED* for CreatePipe()
	if (!CreatePipe(&_hRead, &_hWrite, &pipeAttr, 0)) {
		throw std::runtime_error("Unable to create pipe!");
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited
	if (!SetHandleInformation(_hRead, HANDLE_FLAG_INHERIT, 0)) {
		throw std::runtime_error("_hRead SetHandleInformation");
	}
}

Pipe::~Pipe(void) {
	CloseHandle_s(_hRead);
	CloseHandle_s(_hWrite);
}

bool Pipe::Read(std::string& dst) {
	constexpr size_t bufLen = 256;

	//
	// _hWrite needs to be closed before reading
	// in order for ReadFile to correctly report 
	// if the child process has been terminated
	//
	// This must be happen each time you attempt to read apparently
	//
	CloseHandle_s(_hWrite);

	dst.clear();
	dst.resize(bufLen);

	DWORD dwRead = 0;
	bool result = ReadFile(_hRead, (void*)dst.data(), bufLen - 1, &dwRead, NULL);
	if (!result || !dwRead)
		return false;

	dst[dwRead] = 0;
	return true;
}

Process::Process(const PROCESS_INFORMATION &processInfo)
{
	this->processInfo = processInfo;
}

Process::~Process(void)
{
	CloseHandle_s(this->processInfo.hProcess);
	CloseHandle_s(this->processInfo.hThread);

	if (this->processInfo.hProcess != NULL)
	{
		CloseHandle(this->processInfo.hProcess);
		this->processInfo.hProcess = NULL;
	}

	CloseHandle(this->processInfo.hProcess);
	this->processInfo.hProcess = NULL;
}

Process Process::Create(const LaunchInfo& info, const Pipe& pipe)
{
	return Create(info, &pipe);
}

Process Process::Create(const LaunchInfo& info, const Pipe* pipe)
{
	STARTUPINFO startInfo;
	AutoZeroMemory(startInfo);
	startInfo.cb = sizeof(STARTUPINFO);
	if (pipe != nullptr) {
		startInfo.hStdInput = NULL;
		startInfo.hStdOutput = pipe->_hWrite;
		startInfo.hStdError = pipe->_hWrite;
		startInfo.dwFlags |= STARTF_USESTDHANDLES;
	}

	PROCESS_INFORMATION processInfo;
	AutoZeroMemory(processInfo);

	auto cmd = info.parameters == nullptr
		? va("\"%s\"", info.filename)
		: va("\"%s\" %s", info.filename, info.parameters);

	BOOL result = CreateProcess(
		NULL,				// No module name (use command line)
		(LPTSTR)cmd.c_str(),// Command line
		NULL,				// Process handle not inheritable
		NULL,				// Thread handle not inheritable
		pipe != nullptr,	// handles are inherited if we're piping the output
		CREATE_NO_WINDOW,	// Don't create the console window
		NULL,				// Use parent's environment block
		info.currentDir,	// Use parent's starting directory
		&startInfo,			// Pointer to STARTUPINFO structure
		&processInfo		// Pointer to PROCESS_INFORMATION structure
	);

	if (!result)
	{
		throw std::runtime_error("Unable to create process");
	}

	return Process(processInfo);
}

BOOL Process::WaitForProcess(DWORD dwMilliseconds)
{
	return WaitForSingleObject(this->processInfo.hProcess, dwMilliseconds);
}

DWORD Process::WaitForExit(void)
{
	if (!this->processInfo.hProcess) {
		throw std::runtime_error("Invalid process handle");
	}

	DWORD exitCode = 0;

	do {
		this->WaitForProcess(INFINITE);
		if (!GetExitCodeProcess(this->processInfo.hProcess, &exitCode)) {
			if (exitCode == STILL_ACTIVE) {
				continue;
			}
			auto msg = va("GetExitCodeProcess() failure: %lu\n", GetLastError());
			throw std::runtime_error(msg.c_str());
		}
	} while (exitCode == STILL_ACTIVE);

	return exitCode;
}