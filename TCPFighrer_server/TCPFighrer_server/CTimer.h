#pragma once
class CTimer {
private:
	DWORD _oldTime;	// �������Žð�
	DWORD _oldCntTime;	// ����ī��Ʈ�ð�
	int _iCounter;	// ���� �Ͼ Ƚ��
	DWORD _timeTotal;	// ����
	DWORD _timeMin;	// �ð� ���� �ּڰ�
	DWORD _timeMax;	// �ð� ���� �ִ�
	DWORD _timeAvg;	// ���
	DWORD _TPS;	// TPS
	DWORD _DeltaTime;
public:
	CTimer();
	~CTimer();

	// TPS�� �����ϸ� TRUE;
	BOOL Check(BOOL bCnt = true);

	void LogInfo();
	DWORD GetTPS();
	DWORD DeltaTime();
};

