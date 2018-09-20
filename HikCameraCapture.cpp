#include "HikCameraCapture.h"

#include "HikLibraries\HCNetSDK.h"
#include "HikLibraries\PlayM4.h"
#include "HikLibraries\plaympeg4.h"



struct PlayParam CameraParam = { -1, -1, -1, NULL, false };//摄像机播放参数,需要回调显示


////解码回调 视频为YUV数据(YV12)，音频为PCM数据
void CALLBACK DecCBFun(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, long nReserved1, long nReserved2)
{
	if (pFrameInfo->nType == T_YV12)
	{

	}

}


void CALLBACK g_RealDataCallBack_V40(LONG playHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* dwUser)
{
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD: //系统头

		if (!PlayM4_GetPort(&CameraParam.nPort))  //获取播放库未使用的通道号
		{
			break;
		}
		//m_iPort = lPort; //第一次回调的是系统头，将获取的播放库port号赋值给全局port，下次回调数据时即使用此port号播放
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(CameraParam.nPort, STREAME_REALTIME))  //设置实时流播放模式
			{
				break;
			}

			if (!PlayM4_OpenStream(CameraParam.nPort, pBuffer, dwBufSize, 1024 * 1024)) //打开流接口
			{
				break;
			}
			if (CameraParam.isCallBackDisplay)//判断是否需要回调并且显示
			{
				//调用回调解码并且显示到界面的API-----PlayM4_SetDecCallBackEx
				if (!PlayM4_SetDecCallBackExMend(CameraParam.nPort, DecCBFun, NULL, NULL, (long)dwUser))
				{
					break;
				}

				if (!PlayM4_SetDisplayBuf(CameraParam.nPort, 1))//设置缓冲区大小,默认为1,1表示实时性最好,延迟最小
				{
					break;
				}
				if (!PlayM4_Play(CameraParam.nPort, CameraParam.hPlayWnd1)) //播放开始
				{
					break;
				}
			}
			else
			{
				//只解码不显示到界面
				if (!PlayM4_SetDecCallBackMend(CameraParam.nPort, DecCBFun, (long)dwUser))
				{
					long dRet;
					dRet = PlayM4_GetLastError(CameraParam.nPort);
					break;
				}
				if (!PlayM4_SetDisplayBuf(CameraParam.nPort, 1))//设置缓冲区大小,默认为1,1表示实时性最好,延迟最小
				{
					break;
				}
				if (!PlayM4_Play(CameraParam.nPort, NULL)) //播放开始,注意句柄在程序开始时候需要初始化
				{
					break;
				}
			}
		}
	case NET_DVR_STREAMDATA:   //码流数据
		if (dwBufSize > 0 && CameraParam.nPort != -1)
			if (!PlayM4_InputData(CameraParam.nPort, pBuffer, dwBufSize))
				break;
	}
}


HikCameraCapture::HikCameraCapture()
{
}


HikCameraCapture::~HikCameraCapture()
{
}

bool HikCameraCapture::HikCameraInit()
{
	if (NET_DVR_Init())
		return true;
	else
		return false;
}

bool HikCameraCapture::HikCameraLogin(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort)
{
	NET_DVR_USER_LOGIN_INFO pLoginInfo = { 0 };
	NET_DVR_DEVICEINFO_V40 lpDeviceInfo = { 0 };

	pLoginInfo.bUseAsynLogin = 0;     //同步登录方式
	strcpy_s(pLoginInfo.sDeviceAddress, sDeviceAddress);
	strcpy_s(pLoginInfo.sUserName, sUserName);
	strcpy_s(pLoginInfo.sPassword, sPassword);
	pLoginInfo.wPort = wPort;
	CameraParam.lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);
	if (CameraParam.lUserID < 0)
		return false;
	else
		return true;

}

void HikCameraCapture::HikCameraFrameProcess()
{
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
	struPlayInfo.lChannel = 1;       //预览通道号
	struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
	struPlayInfo.dwLinkMode = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP

	CameraParam.lRealHandle = NET_DVR_RealPlay_V40(CameraParam.lUserID, &struPlayInfo, g_RealDataCallBack_V40, NULL); //最后一个参数是用户数据

}

void HikCameraCapture::HikCameraClose()
{
	if (CameraParam.lUserID >= 0)
	{
		NET_DVR_Logout(CameraParam.lUserID);
		NET_DVR_Cleanup();
	}
}