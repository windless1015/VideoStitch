#pragma once

#include <Windows.h>
//���Ų���
struct PlayParam
{
	long lUserID;
	long lRealHandle;
	long nPort;
	HWND hPlayWnd1;//��ʾ���
	bool isCallBackDisplay;//�Ƿ�ص����벢����ʾ,true��ʾ�ص�����ʾ,falseֻ�ص�����ʾ
};






class HikCameraCapture
{
public:
	HikCameraCapture();
	~HikCameraCapture();
public:
	bool HikCameraInit();
	bool HikCameraLogin(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort);
	void HikCameraFrameProcess();
	void HikCameraClose();

};

