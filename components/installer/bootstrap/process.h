#pragma once

class TextBuffer {
	virtual void write(const std::string& data) = 0;
	virtual void flush(void) = 0;
};

class Pipe {
	friend class Process;

private:
	HANDLE _hRead;
	HANDLE _hWrite;

public:
	Pipe(bool inheritHandle = false);
	~Pipe(void);

	bool Read(std::string& dst);
};

class Process {
public:
	struct LaunchInfo {
		const char* filename;
		const char* parameters;
		const char* currentDir;
	};

	enum class State {
		UNKNOWN,
		RUNNING,
		DEAD,
	};

	Process(Process &&) = default;

	Process(const PROCESS_INFORMATION& processInfo);
	~Process(void);

	static Process Create(const LaunchInfo& info, const Pipe& pipe);
	static Process Create(const LaunchInfo& info, const Pipe* pipe = NULL);

	BOOL WaitForProcess(DWORD dwMilliseconds = INFINITE);

	// Wait for the process to exit, returns the exit code
	DWORD WaitForExit(void);

private:
	HANDLE hStdin_r;
	HANDLE hStdin_w;

	HANDLE hStdout_r;
	HANDLE hStdout_w;

	PROCESS_INFORMATION processInfo;
};
