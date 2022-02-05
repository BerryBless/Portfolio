#include "pch.h"
#include "CFrameSkip.h"
#include "NetworkCore.h"
#include "PacketProcess.h"//g_syncCnt

extern int g_syncCnt ;
extern CTimer g_sendTPS;
extern CTimer g_recvTPS;

CFrameSkip::CFrameSkip() {
	// �ʱ�ȭ
	_isSkip = FALSE;
	_curTime = timeGetTime();
	_oldTime = timeGetTime();
	_deltatime = _curTime - _oldTime;
}

CFrameSkip::~CFrameSkip() {
}

BOOL CFrameSkip::FrameSkip() {
	// ����ð�
	_curTime = timeGetTime();

	// �������� �ð� ���ϱ�
	_deltatime = _curTime - _oldTime;

	// ��Ʈ��ũ FPS ���ϱ�
	_loopFPS.Check();
	// �ѹ����� ���Ҵµ� _iDelayTime ���� ������ �׸�ŭ sleep

	if (_deltatime < dfDTD) {
		return FALSE;
	}
	// �������� �̻� ���̳��� �ѹ� ��ŵ���ֱ�
		// ��ŵ����


		// dfDTD ���� : sleep�� ��ŭ old�� ���Ѵ�
		// dfDTD �ʰ� : �ʰ��� ��ŭ old���� �A�� (����)
	_oldTime = _curTime - (_deltatime - dfDTD);

	// FPS���
	if (_gameFPS.Check()) {
		//---------------------------
		// ����͸� ó��
		//---------------------------
		_LOG(dfLOG_LEVEL_ERROR, L"LoopFPS[%d] LogicFPS [%d] SYNC COUNT[%d]"
			, _loopFPS.GetTPS(), _gameFPS.GetTPS() , g_syncCnt);
		_gameFPS.LogInfo();
		_LOG(dfLOG_LEVEL_ERROR, L"connect session [%d]", g_sessionMap.size() );
		_LOG(dfLOG_LEVEL_ERROR, L"send TPS [%d] , recv TPS [%d]\n-----------------------------------------------", g_sendTPS.GetTPS(), g_recvTPS.GetTPS());
	}


	return TRUE;
}

int CFrameSkip::GetLogicFPS() {
	return _gameFPS.GetTPS();
}

int CFrameSkip::GetNetworkFPS() {
	return _loopFPS.GetTPS();
}

DWORD CFrameSkip::DeltaTime() {
	return _gameFPS.DeltaTime();
}

DWORD CFrameSkip::Now() {
	return timeGetTime();
}
