#include "pch.h"
#include "CTimer.h"

CTimer::CTimer() {
	_oldTime = timeGetTime();
	_oldCntTime = timeGetTime();
	_TPS = 0;
	_iCounter = 0;

	_timeMin = 0x7fffffff;
	_timeMax = -1;
	_timeTotal = 0;
}

CTimer::~CTimer() {
}

BOOL CTimer::Check(BOOL bCnt) {
	DWORD dwCur = timeGetTime(); // ����ð�
	if (bCnt) {
		 _DeltaTime = dwCur - _oldCntTime;
		_iCounter++;

		// MAX
		if ((int) _timeMax < (int) _DeltaTime) {
			_timeMax = _DeltaTime;
		}
		// MIN
		if ((int) _timeMin > (int)_DeltaTime) {
			_timeMin = _DeltaTime;
		}
		// TOTAL
		_timeTotal += _DeltaTime;


		_oldCntTime = dwCur;
	}
	if (dwCur - _oldTime >= 1000) {
		// 1�ʸ��� ����
		_TPS = (_iCounter * 1000) / (dwCur - _oldTime);
		_oldTime = dwCur;

		// ���
		if (_iCounter > 0)
			_timeAvg = _timeTotal / _iCounter;
		// �ʱ�ȭ
		_iCounter = 0;

		return TRUE;
	}
	return FALSE;
}

void CTimer::LogInfo() {
	_LOG(dfLOG_LEVEL_ERROR, L"delta time : min [%d], MAX[%d], total[%d], avg[%d]", _timeMin, _timeMax, _timeTotal, _timeAvg);
	_timeMin = 0x7fffffff;
	_timeMax = -1;
	_timeTotal = 0;
}

DWORD CTimer::GetTPS() {
	Check(false);
	return _TPS;
}

DWORD CTimer::DeltaTime() {
	return _DeltaTime;
}
