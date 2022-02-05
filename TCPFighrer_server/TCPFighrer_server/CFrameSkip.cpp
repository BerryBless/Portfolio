#include "pch.h"
#include "CFrameSkip.h"
#include "NetworkCore.h"
#include "PacketProcess.h"//g_syncCnt

extern int g_syncCnt ;
extern CTimer g_sendTPS;
extern CTimer g_recvTPS;

CFrameSkip::CFrameSkip() {
	// 초기화
	_isSkip = FALSE;
	_curTime = timeGetTime();
	_oldTime = timeGetTime();
	_deltatime = _curTime - _oldTime;
}

CFrameSkip::~CFrameSkip() {
}

BOOL CFrameSkip::FrameSkip() {
	// 현재시간
	_curTime = timeGetTime();

	// 한프레임 시간 구하기
	_deltatime = _curTime - _oldTime;

	// 네트워크 FPS 구하기
	_loopFPS.Check();
	// 한바퀴를 돌았는데 _iDelayTime 보다 작으면 그만큼 sleep

	if (_deltatime < dfDTD) {
		return FALSE;
	}
	// 한프레임 이상 차이나면 한번 스킵해주기
		// 스킵없음


		// dfDTD 이하 : sleep한 만큼 old에 더한다
		// dfDTD 초과 : 초과한 만큼 old에서 뺸다 (누적)
	_oldTime = _curTime - (_deltatime - dfDTD);

	// FPS계산
	if (_gameFPS.Check()) {
		//---------------------------
		// 모니터링 처리
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
