#pragma once
#include "CRingBuffer.h"
#include <Windows.h>
class CSession {
public:
	SOCKET _sock;
	DWORD _SID; // ���� ID
	CRingBuffer _recvQ;	// ���� ť
	CRingBuffer _sendQ; // �۽� ť
	DWORD _LastRecvTime; // ���������� �۽��� �ð�
};
