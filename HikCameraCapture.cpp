#include "HikCameraCapture.h"

#include "HikLibraries\HCNetSDK.h"
#include "HikLibraries\PlayM4.h"
#include "HikLibraries\plaympeg4.h"



struct PlayParam CameraParam = { -1, -1, -1, NULL, false };//��������Ų���,��Ҫ�ص���ʾ


////����ص� ��ƵΪYUV����(YV12)����ƵΪPCM����
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
	case NET_DVR_SYSHEAD: //ϵͳͷ

		if (!PlayM4_GetPort(&CameraParam.nPort))  //��ȡ���ſ�δʹ�õ�ͨ����
		{
			break;
		}
		//m_iPort = lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(CameraParam.nPort, STREAME_REALTIME))  //����ʵʱ������ģʽ
			{
				break;
			}

			if (!PlayM4_OpenStream(CameraParam.nPort, pBuffer, dwBufSize, 1024 * 1024)) //�����ӿ�
			{
				break;
			}
			if (CameraParam.isCallBackDisplay)//�ж��Ƿ���Ҫ�ص�������ʾ
			{
				//���ûص����벢����ʾ�������API-----PlayM4_SetDecCallBackEx
				if (!PlayM4_SetDecCallBackExMend(CameraParam.nPort, DecCBFun, NULL, NULL, (long)dwUser))
				{
					break;
				}

				if (!PlayM4_SetDisplayBuf(CameraParam.nPort, 1))//���û�������С,Ĭ��Ϊ1,1��ʾʵʱ�����,�ӳ���С
				{
					break;
				}
				if (!PlayM4_Play(CameraParam.nPort, CameraParam.hPlayWnd1)) //���ſ�ʼ
				{
					break;
				}
			}
			else
			{
				//ֻ���벻��ʾ������
				if (!PlayM4_SetDecCallBackMend(CameraParam.nPort, DecCBFun, (long)dwUser))
				{
					long dRet;
					dRet = PlayM4_GetLastError(CameraParam.nPort);
					break;
				}
				if (!PlayM4_SetDisplayBuf(CameraParam.nPort, 1))//���û�������С,Ĭ��Ϊ1,1��ʾʵʱ�����,�ӳ���С
				{
					break;
				}
				if (!PlayM4_Play(CameraParam.nPort, NULL)) //���ſ�ʼ,ע�����ڳ���ʼʱ����Ҫ��ʼ��
				{
					break;
				}
			}
		}
	case NET_DVR_STREAMDATA:   //��������
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

	pLoginInfo.bUseAsynLogin = 0;     //ͬ����¼��ʽ
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
	struPlayInfo.hPlayWnd = NULL;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo.lChannel = 1;       //Ԥ��ͨ����
	struPlayInfo.dwStreamType = 0;       //0-��������1-��������2-����3��3-����4���Դ�����
	struPlayInfo.dwLinkMode = 0;       //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP

	CameraParam.lRealHandle = NET_DVR_RealPlay_V40(CameraParam.lUserID, &struPlayInfo, g_RealDataCallBack_V40, NULL); //���һ���������û�����

}

void HikCameraCapture::HikCameraClose()
{
	if (CameraParam.lUserID >= 0)
	{
		NET_DVR_Logout(CameraParam.lUserID);
		NET_DVR_Cleanup();
	}
}