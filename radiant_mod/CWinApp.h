#pragma once

class CWinThread
{
public:
	void Run();
};

class CWinApp : public CWinThread
{
public:
	void Run();
};