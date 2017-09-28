#pragma once

class CCamWnd : public CWnd
{
private:
	// Do not allow this class to be instanced
	CCamWnd() = delete;
	~CCamWnd() = delete;

public:
	char _pad[0x1C];
	float cameraOrigin[3];
	float cameraAngles[3];

	static CCamWnd * __stdcall ctor(CCamWnd *This);
	static CCamWnd *(__stdcall *ctor_o)(CCamWnd *This);
	static CCamWnd *ActiveWindow;

	void Cam_MouseControl(float dtime);
};
STATIC_ASSERT_OFFSET(CCamWnd, cameraOrigin, 0x70);
STATIC_ASSERT_OFFSET(CCamWnd, cameraAngles, 0x7C);