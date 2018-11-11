#include "stdafx.h"
#include "ipc.h"

#include <assert.h>

// RICH TEXT TEST

// static const char* rtfTest =	"{\\rtf1 " 
//								"{\\colortbl ;\\red238\\green0\\blue0;}" 
//								"Lorem ipsum dolor sit amet " 
//								"{\\b {\\field{\\*\\fldinst{HYPERLINK \"https://www.example.com/\" }}"
//								"{\\fldrslt{\\cf1 CLICK_HERE\\cf0 }}}} " 
//								"consectetur adipiscing elit.}";

namespace IPC
{
	Connection::Connection(HWND handle) : hWnd(handle) {
		assert((hWnd != NULL && hWnd != (HWND)ULONG_MAX));
	}

	void Connection::print(const char* msg) {
		// Both of these EM_SETSEL are required to move the caret to the 
		// end of the output text
		SendMessage(hWnd, EM_SETSEL, 0, -1);
		SendMessage(hWnd, EM_SETSEL, (WPARAM)-1, -1);

		// Insert the target text at the caret location
		SendMessage(hWnd, EM_REPLACESEL, (WPARAM)false, (LPARAM)msg);
	}

	void Connection::print(const std::string& msg) {
		return print(msg.c_str());
	}

	void Connection::printf(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		const auto msg = va(fmt, args);
		va_end(args);
		return print(msg);
	}
}