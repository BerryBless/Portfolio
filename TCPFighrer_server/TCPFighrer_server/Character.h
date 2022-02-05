#pragma once
#include "Sector.h"
#include "Session.h"
struct st_SECTOR_POS;


class CCharacter {
public:
	CSession *_pSession;
	DWORD _SID;				// ���� ID

	DWORD _action;		// ���� �ൿ
	BYTE _direction;		// ���� �ٶ󺸴� ����
	BYTE _moveDirection;	// ���� �̵� ����

	// ������ǥ
	SHORT _X;				
	SHORT _Y;				

	// ����
	struct st_SECTOR_POS _curSecPos;
	struct st_SECTOR_POS _oldSecPos;

	// "�����̴��� ����"
	char _chHP;

	// ��Ʈ��Ʈ
	DWORD _heartbeat;
};

