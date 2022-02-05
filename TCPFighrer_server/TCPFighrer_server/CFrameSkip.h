#pragma once
#include "CTimer.h"

//	---
// �ɹ�����
// _isSkip : ������������ Render�� ��ŵ������ TRUE
// _oldTime : �����ð� + �����ð� (�����ð��� �����ϵ� �ð����� �ջ�ǰ� ��������.)
// _curTime : ����ð� (timeGetTime)
// _deltatime : _curTime -  _oldTime
// _dwSkipTime : Render�� ��ŵ�Ǿ��ٸ� ������������ _curTime
// 
// _gameFPS : ������ FPS
// _CRenderFPS : ������ FPS
// ---
// �ɹ��Լ�
// FrameSkip : �������� ��ŵ�Ǿ�� �ϴ��� Ȯ�� (��ŵ�̸� FALSE)
// GetLogicFPS : ���� FPS
// GetRenderFPS : ���� FPS


class CFrameSkip {

private:
	BOOL _isSkip;	// ������������ ��ŵ �Ǿ�����
	DWORD _oldTime;	// �����ð� + �����ð�
	DWORD _curTime;	// ����ð�
	DWORD _deltatime;	// ���� - ����(+����)

	CTimer _loopFPS;
	CTimer _gameFPS;

public:
	CFrameSkip();
	~CFrameSkip();

	BOOL FrameSkip();	// TRUE�� ����, FALSE�� ��ŵ
	int GetLogicFPS();
	int GetNetworkFPS();

	// 1�����Ӱ��� ���� ���� dfDTDms
	DWORD DeltaTime(); // ms
	DWORD Now(); // timegettime
};


