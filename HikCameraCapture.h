#pragma once

#include <Windows.h>
//播放参数
struct PlayParam
{
	long lUserID;
	long lRealHandle;
	long nPort;
	HWND hPlayWnd1;//显示句柄
	bool isCallBackDisplay;//是否回调解码并且显示,true表示回调且显示,false只回调不显示
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

