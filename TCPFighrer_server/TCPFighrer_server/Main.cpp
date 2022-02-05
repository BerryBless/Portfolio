#include "pch.h"
#include "NetworkCore.h"
#include "ContentCore.h"
#include "CFrameSkip.h"
bool g_bShutdown = false;
CFrameSkip g_Timer;
extern int g_LogLevel;

void ServerControl() {
	static bool ControlMode = false;

	if (_kbhit()) {
		WCHAR wcKey = _getwch();

		// UNLOCK
		if (L'u' == wcKey || L'U' == wcKey) {
			ControlMode = true;
			wprintf_s(L"######## Server Control Unlocked\n");
			wprintf_s(L"######## \t L - Server Control Lock \n");
			wprintf_s(L"######## \t U - Server Control Unlock \n");
			wprintf_s(L"######## \t Q - Quit \n");
			wprintf_s(L"######## \t 0 - LOG_LEVEL_DEBUG \n");
			wprintf_s(L"######## \t 1 - LOG_LEVEL_WARNING \n");
			wprintf_s(L"######## \t 2 - LOG_LEVEL_ERROR \n");

		}

		// LOCK
		if (L'l' == wcKey || L'L' == wcKey) {
			ControlMode = true;
			wprintf_s(L"######## Server Control Locked\n");
		}

		// CONTROL
		// ����
		if ((L'q' == wcKey || L'Q' == wcKey) && ControlMode) {
			g_bShutdown = true;
			wprintf_s(L"######## Server Closing...\n");
		}

		// �α׷���
		if ((L'0' == wcKey) && ControlMode) {
			g_LogLevel = dfLOG_LEVEL_DEBUG;
			_LOG(dfLOG_LEVEL_ERROR, L"######## LOG_LEVEL_DEBUG");
		}
		if ((L'1' == wcKey) && ControlMode) {
			g_LogLevel = dfLOG_LEVEL_WARNING;
			_LOG(dfLOG_LEVEL_ERROR, L"######## LOG_LEVEL_WARNING");
		}
		if ((L'2' == wcKey) && ControlMode) {
			g_LogLevel = dfLOG_LEVEL_ERROR;
			_LOG(dfLOG_LEVEL_ERROR, L"######## LOG_LEVEL_ERROR");
		}

		// �����׽�Ʈ �ѱ� / ����
		if ((L'd' == wcKey || L'D' == wcKey) && ControlMode) {
			g_dummytest = !g_dummytest;
			if (g_dummytest)
				wprintf_s(L"######## Dummy on!\n");
			else
				wprintf_s(L"######## Dummy off...\n");
		}

	}

}
int main() {
	timeBeginPeriod(1);// timegettime 1�ʴ����� �����ϰ�.
	_wsetlocale(LC_ALL, L"korean");// �ѱ��� ����

	wprintf_s(L"######## \t L - Server Control Lock \n");
	wprintf_s(L"######## \t U - Server Control Unlock \n");
	wprintf_s(L"######## \t D - DUMMY TEST TOGGLE \n");
	wprintf_s(L"######## \t Q - Quit \n");
	wprintf_s(L"######## \t 0 - LOG_LEVEL_DEBUG \n");
	wprintf_s(L"######## \t 1 - LOG_LEVEL_WARNING \n");
	wprintf_s(L"######## \t 2 - LOG_LEVEL_ERROR \n");

	// ��Ʈ��ũ �ʱ�ȭ
	NetworkInitServer();
	while (!g_bShutdown) {
		_LOG(dfLOG_LEVEL_DEBUG, L"=================================================\nMAIN LOOP");
		// ���� ��Ʈ��
		ServerControl();

		// ��Ʈ��ũ��
		NetworkPorc();

		// �������� 
		if (g_Timer.FrameSkip()) {
			ContentUpdate();
		}
		_LOG(dfLOG_LEVEL_DEBUG, L"\nMAIN LOOP END\n=================================================");
	}
	// ��Ʈ��ũ ����
	NetworkCloseServer();
	return 0;
}