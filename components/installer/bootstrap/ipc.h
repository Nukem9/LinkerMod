#pragma once
#include <Windows.h>
#include <string>

namespace IPC {
	//
	// Represents an RPC connection to the installer application
	//
	class Connection {
	private:
		HWND hWnd;
	public:
		Connection(HWND handle);
	
		void print(const char* msg);
		void print(const std::string& msg);

		void printf(const char* fmt, ...);
	};
}


