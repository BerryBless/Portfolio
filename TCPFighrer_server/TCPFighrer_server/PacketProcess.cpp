#include "pch.h"
#include "PacketProcess.h"

extern std::list<CCharacter *> g_sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];

int g_syncCnt = 0;

BOOL PacketProc(CSession *pSession, CPacket *pPacket, BYTE	byType) {
	_LOG(dfLOG_LEVEL_DEBUG, L"PacketProc :: byType[%hhu] SID[%d]\n", byType, pSession->_SID);
	switch (byType) {
	case dfPACKET_CS_MOVE_START:
		return CS_MoveStart(pSession, pPacket);

	case dfPACKET_CS_MOVE_STOP:
		return CS_MoveStop(pSession, pPacket);

	case dfPACKET_CS_ATTACK1:
		return CS_ActionAttack_1(pSession, pPacket);

	case dfPACKET_CS_ATTACK2:
		return CS_ActionAttack_2(pSession, pPacket);

	case dfPACKET_CS_ATTACK3:
		return CS_ActionAttack_3(pSession, pPacket);

	case dfPACKET_CS_ECHO:
		return CS_ECHO(pSession, pPacket);

	default:
		return FALSE;
	}
}

//#define dfTEMP
BOOL InstantiateCharacter(CSession *pSession) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// InstantiateCharacter : pSession IS NULL.");
		CRASH();
	}

	CCharacter *pCharacter = CreateCharacter(pSession->_SID);
	if (pCharacter == NULL) {
		//ĳ���� ���� ����
		_LOG(dfLOG_LEVEL_ERROR, L"////// CreateCharacter : SID[%d] There's already an object.", pSession->_SID);
		CRASH();
		return FALSE;
	}
	// �������� 
	st_PACKET_HEADER header;
	CPacket packet;

	// �ʱⰪ �ֱ�
	pCharacter->_pSession = pSession;
	pCharacter->_SID = pSession->_SID;
	pCharacter->_chHP = 100;
#ifdef dfTEMP
	pCharacter->_X = 100;// rand() % dfRANGE_MOVE_RIGHT;
	pCharacter->_Y = 100;// rand() % dfRANGE_MOVE_BOTTOM;
#else
	pCharacter->_X = rand() % dfRANGE_MOVE_RIGHT;
	pCharacter->_Y = rand() % dfRANGE_MOVE_BOTTOM;
#endif // dfTEMP

	pCharacter->_action = dfACTION_STAND;
	pCharacter->_direction = dfPACKET_MOVE_DIR_LL;
	pCharacter->_oldSecPos._X = dfSECTOR_MAX_X + 10; // ���� �Ȱ�ġ�� �ִ밪
	pCharacter->_oldSecPos._Y = dfSECTOR_MAX_Y + 10;


	// ĳ���� ������Ŷ������
	_LOG(dfLOG_LEVEL_DEBUG, L" CreateCharacter : SID[%d]  :: Create My Character ", pSession->_SID);
	MakePacket_Create_My_Character(&header, &packet,
		pSession->_SID, pCharacter->_direction, pCharacter->_X, pCharacter->_Y, pCharacter->_chHP);
	SendUnicast(pSession, &header, &packet);



	//---------------------------------------------------------------
	// �ٸ������ ������ ������
	// ����
	//---------------------------------------------------------------
	_LOG(dfLOG_LEVEL_DEBUG, L" CreateCharacter : SID[%d]  :: Create Other Character", pSession->_SID);
	st_SECTOR_AROUND around;
	std::list<CCharacter *> *pSectorList; // ���Ϳ� �ִ� ĳ���� ����Ʈ
	CCharacter *pExistCharacter;	// ���Ϳ� �ִ� ĳ����
	Sector_AddCharacter(pCharacter);
	GetSectorAround(pCharacter->_curSecPos._X, pCharacter->_curSecPos._Y, &around);

	//---------------------------------------------------------------
	// �ٸ��𿡰� ���� �Դٰ� �˸���
	//---------------------------------------------------------------
	_LOG(dfLOG_LEVEL_DEBUG, L" CreateCharacter : SID[%d]  :: me -> other ", pSession->_SID);
	MakePacket_Create_Other_Character(&header, &packet,
		pCharacter->_SID,
		pCharacter->_direction,
		pCharacter->_X,
		pCharacter->_Y,
		pCharacter->_chHP);

	for (int i = 0; i < around._count; i++) {
		SendSectorOne(
			around._around[i]._X, around._around[i]._Y,
			pCharacter->_pSession,
			&header, &packet);
	}

	//---------------------------------------------------------------
	// ���� �ִ� �� �� �����ֱ�
	//---------------------------------------------------------------
	_LOG(dfLOG_LEVEL_DEBUG, L" CreateCharacter : SID[%d]  :: other -> me", pSession->_SID);
	for (int i = 0; i < around._count; i++) {
		// ����� ���� ����Ʈ ����
		pSectorList = &g_sector[around._around[i]._Y][around._around[i]._X];
		// �ش� ���Ϳ� ��ϵ� ĳ���͸� �̾Ƽ� ������Ŷ ������
		for (auto iter = pSectorList->begin(); iter != pSectorList->end(); ++iter) {
			pExistCharacter = *iter;

			if (pExistCharacter == pCharacter) {
				// ���� �Ⱥ�������
				continue;
			}

			MakePacket_Create_Other_Character(&header, &packet,
				pExistCharacter->_SID,
				pExistCharacter->_direction,
				pExistCharacter->_X,
				pExistCharacter->_Y,
				pExistCharacter->_chHP);

			SendUnicast(pCharacter->_pSession, &header, &packet);

			// �׳���� �̵����ϰ�� �̵���Ŷ ����
			switch (pExistCharacter->_action) {
			case dfACTION_MOVE_LL:
			case dfACTION_MOVE_LU:
			case dfACTION_MOVE_UU:
			case dfACTION_MOVE_RU:
			case dfACTION_MOVE_RR:
			case dfACTION_MOVE_RD:
			case dfACTION_MOVE_DD:
			case dfACTION_MOVE_LD:
				_LOG(dfLOG_LEVEL_DEBUG, L"ChatacterSectorUpdatePacket 4) CHARACTER_MOVE\n\tID[%d] world pos[%d, %d], action[%d], movedir [%d] dir[%d]", pExistCharacter->_SID, pExistCharacter->_X, pExistCharacter->_Y, pExistCharacter->_action, pExistCharacter->_moveDirection, pExistCharacter->_direction);

				MakePacket_Move_Start(&header, &packet,
					pExistCharacter->_SID,
					pExistCharacter->_moveDirection,
					pExistCharacter->_X,
					pExistCharacter->_Y);

				SendUnicast(pCharacter->_pSession, &header, &packet);
				break;
			}

		}

	}


	_LOG(dfLOG_LEVEL_DEBUG, L"InstantiateCharacter :: SID[%d] WORLD[%d, %d] SECTOR[%d %d] ACTION[%d]\n", pCharacter->_SID, pCharacter->_X, pCharacter->_direction, pCharacter->_curSecPos._X, pCharacter->_curSecPos._Y, pCharacter->_action);
	return TRUE;
}

#pragma region Client -> Server
//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Client -> Server
//
// �ڽ��� ĳ���� �̵����۽� �� ��Ŷ�� ������.
// �̵� �߿��� �� ��Ŷ�� ������ ������, Ű �Է��� ����Ǿ��� ��쿡��
// ������� �Ѵ�.
//
// (���� �̵��� ���� �̵� / ���� �̵��� ���� ���� �̵�... ���)
//
//	1	-	Direction	( ���� ������ �� 8���� ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
BOOL CS_MoveStart(CSession *pSession, CPacket *pPacket) {

	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_MoveStart : pSession IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_MoveStart : pPacket IS NULL.");
		CRASH();
	}


	CCharacter *pCharacter = FindCharacter(pSession->_SID);
	if (pCharacter == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_MoveStart(). FindCharacter() : SID[%d] can't find the object.", pSession->_SID);
		CRASH();
		return FALSE;
	}
	st_PACKET_HEADER header;
	CPacket packet;
	BYTE dir;
	WORD x;
	WORD y;
	*pPacket >> dir;
	*pPacket >> x;
	*pPacket >> y;
	_LOG(dfLOG_LEVEL_DEBUG, L"CS_MoveStart get dir[%d], (x, y) [%d, %d]", dir, x, y);

	//---------------------------------------------------------------
	// Ŭ�� ��ǥ ����
	//---------------------------------------------------------------
	if (abs(pCharacter->_X - x) > dfERROR_RANGE ||
		abs(pCharacter->_Y - y) > dfERROR_RANGE) {

		// ����ȭ ��Ŷ ������
		_LOG(dfLOG_LEVEL_WARNING, L"/////// SYNC ERROR _ CS_MoveStart() ::\n\tCharacter ID[%d] hp[%d]\n\tCharacter Action [%d | %d]\n\tCharacter Position (%d, %d)\n\tRecv Position (%d, %d)",
			pCharacter->_SID, pCharacter->_chHP, pCharacter->_action, pCharacter->_moveDirection, pCharacter->_X, pCharacter->_Y, x, y);

		MakePacket_Sync(&header, &packet, pCharacter->_SID, pCharacter->_X, pCharacter->_Y);
		SendUnicast(pSession, &header, &packet);

		// ��ǥ ����
		x = pCharacter->_X;
		y = pCharacter->_Y;
	}
	pCharacter->_X = x;
	pCharacter->_Y = y;
	//---------------------------------------------------------------
	// ���� �׼� == ���Ⱚ
	//---------------------------------------------------------------
	pCharacter->_action = dir;


	//---------------------------------------------------------------
	// ����ǥ�ÿ�
	//---------------------------------------------------------------
	pCharacter->_moveDirection = dir;


	//---------------------------------------------------------------
	// �ٶ� ���Ⱚ
	//---------------------------------------------------------------
	switch (dir) {
	case dfACTION_MOVE_RU:
	case dfACTION_MOVE_RR:
	case dfACTION_MOVE_RD:
		pCharacter->_direction = dfACTION_MOVE_RR;
		break;
	case dfACTION_MOVE_LL:
	case dfACTION_MOVE_LU:
	case dfACTION_MOVE_LD:
		pCharacter->_direction = dfACTION_MOVE_LL;
		break;

	default:
		break;
	}

	//---------------------------------------------------------------
	// �̵��ΰ�� ���� ������Ʈ
	//---------------------------------------------------------------
	if (Sector_UpdateCharacter(pCharacter)) {
		ChatacterSectorUpdatePacket(pCharacter);
	}


	//---------------------------------------------------------------
	// ���� ���̴� ���Ϳ��� ������
	//---------------------------------------------------------------
	MakePacket_Move_Start(&header, &packet, pCharacter->_SID, pCharacter->_moveDirection, pCharacter->_X, pCharacter->_Y);
	SendSectorAround(pCharacter, &header, &packet);
	_LOG(dfLOG_LEVEL_DEBUG, L"CS_MoveStart :: SID[%d] WORLD[%d, %d] SECTOR[%d %d] ACTION[%d]\n", pCharacter->_SID, pCharacter->_X, pCharacter->_direction, pCharacter->_curSecPos._X, pCharacter->_curSecPos._Y, pCharacter->_action);
	return TRUE;
}

//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Client -> Server
//
// �̵��� Ű���� �Է��� ��� �����Ǿ��� ��, �� ��Ŷ�� ������ �����ش�.
//
//	1	-	Direction	( ���� ������ �� ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
BOOL CS_MoveStop(CSession *pSession, CPacket *pPacket) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_MoveStop : pSession IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_MoveStop : pPacket IS NULL.");
		CRASH();
	}


	CCharacter *pCharacter = FindCharacter(pSession->_SID);
	if (pCharacter == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_MoveStop(). FindCharacter() : SID[%d] can't find the object.", pSession->_SID);
		CRASH();
		return FALSE;
	}

	st_PACKET_HEADER header;
	CPacket packet;
	BYTE dir;
	WORD x;
	WORD y;


	*pPacket >> dir;
	*pPacket >> x;
	*pPacket >> y;
	_LOG(dfLOG_LEVEL_DEBUG, L"CS_MoveStop get dir[%d], (x, y) [%d, %d]", dir, x, y);

	//---------------------------------------------------------------
	// Ŭ�� ��ǥ ����
	//---------------------------------------------------------------
	if (abs(pCharacter->_X - x) > dfERROR_RANGE ||
		abs(pCharacter->_Y - y) > dfERROR_RANGE) {

		// ����ȭ ��Ŷ ������
		_LOG(dfLOG_LEVEL_WARNING, L"/////// SYNC ERROR _ CS_MoveStop() ::\n\tCharacter ID[%d] hp[%d]\n\tCharacter Action [%d | %d]\n\tCharacter Position (%d, %d)\n\tRecv Position (%d, %d)",
			pCharacter->_SID,pCharacter->_chHP, pCharacter->_action, pCharacter->_moveDirection, pCharacter->_X, pCharacter->_Y, x, y);

		MakePacket_Sync(&header, &packet, pCharacter->_SID, pCharacter->_X, pCharacter->_Y);
		SendUnicast(pSession, &header, &packet);

		// ��ǥ ����
		x = pCharacter->_X;
		y = pCharacter->_Y;
	}
	pCharacter->_X = x;
	pCharacter->_Y = y;
	//---------------------------------------------------------------
	// �̵��ΰ�� ���� ������Ʈ
	//---------------------------------------------------------------
	if (Sector_UpdateCharacter(pCharacter)) {
		ChatacterSectorUpdatePacket(pCharacter);
	}

	pCharacter->_direction = dir;
	pCharacter->_action = dfACTION_STAND;

	MakePacket_Move_Stop(&header, &packet, pSession->_SID, dir, x, y);

	//---------------------------------------------------------------
	// ���� ���̴� ���Ϳ��� ������
	//---------------------------------------------------------------
	SendSectorAround(pCharacter, &header, &packet);

	_LOG(dfLOG_LEVEL_DEBUG, L"CS_MoveStop :: SID[%d] WORLD[%d, %d] SECTOR[%d %d] ACTION[%d]\n", pCharacter->_SID, pCharacter->_X, pCharacter->_direction, pCharacter->_curSecPos._X, pCharacter->_curSecPos._Y, pCharacter->_action);
	return TRUE;
}

//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Client -> Server
//
// ���� Ű �Է½� �� ��Ŷ�� �������� ������.
// �浹 �� �������� ���� ����� �������� �˷� �� ���̴�.
//
// ���� ���� ���۽� �ѹ��� �������� ������� �Ѵ�.
//
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y	
//
//---------------------------------------------------------------
BOOL CS_ActionAttack_1(CSession *pSession, CPacket *pPacket) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_1 : pSession IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_1 : pPacket IS NULL.");
		CRASH();
	}



	CCharacter *pAttacker = FindCharacter(pSession->_SID);
	if (pAttacker == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_1 FindCharacter : SID[%d] can't find the object.", pSession->_SID);
		CRASH();
		return FALSE;
	}
	pAttacker->_action = dfACTION_ATTACK1;

	// ��Ŷ ����� ������
	HitScanAndSend(pAttacker);
	_LOG(dfLOG_LEVEL_DEBUG, L"CS_ActionAttack_1 :: SID[%d]\n", pAttacker->_SID);
	return TRUE;
}

//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Client -> Server
//
// ���� Ű �Է½� �� ��Ŷ�� �������� ������.
// �浹 �� �������� ���� ����� �������� �˷� �� ���̴�.
//
// ���� ���� ���۽� �ѹ��� �������� ������� �Ѵ�.
//
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
BOOL CS_ActionAttack_2(CSession *pSession, CPacket *pPacket) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_2 : pSession IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_2 : pPacket IS NULL.");
		CRASH();
	}



	CCharacter *pAttacker = FindCharacter(pSession->_SID);
	if (pAttacker == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_2 FindCharacter : SID[%d] can't find the object.", pSession->_SID);
		CRASH();
		return FALSE;
	}
	pAttacker->_action = dfACTION_ATTACK2;

	// ��Ŷ ����� ������
	HitScanAndSend(pAttacker);
	_LOG(dfLOG_LEVEL_DEBUG, L"CS_ActionAttack_2 :: SID[%d]\n", pAttacker->_SID);
	return TRUE;
}

//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Client -> Server
//
// ���� Ű �Է½� �� ��Ŷ�� �������� ������.
// �浹 �� �������� ���� ����� �������� �˷� �� ���̴�.
//
// ���� ���� ���۽� �ѹ��� �������� ������� �Ѵ�.
//
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
BOOL CS_ActionAttack_3(CSession *pSession, CPacket *pPacket) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_3 : pSession IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_3 : pPacket IS NULL.");
		CRASH();
	}


	CCharacter *pAttacker = FindCharacter(pSession->_SID);
	if (pAttacker == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ActionAttack_1 FindCharacter : SID[%d] can't find the object.", pSession->_SID);
		CRASH();
		return FALSE;
	}
	pAttacker->_action = dfACTION_ATTACK3;

	// ��Ŷ ����� ������
	HitScanAndSend(pAttacker);
	_LOG(dfLOG_LEVEL_DEBUG, L"CS_ActionAttack_1 :: SID[%d]\n", pAttacker->_SID);
	return TRUE;
}

//---------------------------------------------------------------
// Echo �� ��Ŷ					Client -> Server
//
//	4	-	Time
//
//---------------------------------------------------------------
BOOL CS_ECHO(CSession *pSession, CPacket *pPacket) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ECHO : pSession IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CS_ECHO : pPacket IS NULL.");
		CRASH();
	}


	_LOG(dfLOG_LEVEL_DEBUG, L"Recv ECHO");

	DWORD time;
	*pPacket >> time;
	_LOG(dfLOG_LEVEL_DEBUG, L"Recv ECHO time[%d]", time);



	st_PACKET_HEADER header;
	CPacket packet;
	_LOG(dfLOG_LEVEL_DEBUG, L"Send ECHO");

	MakePacket_Echo(&header, &packet, time);
	_LOG(dfLOG_LEVEL_DEBUG, L"Send ECHO now[%d]", time);

	SendUnicast(pSession, &header, &packet);

	return TRUE;
}


#pragma endregion



#pragma region Server -> Client
//---------------------------------------------------------------
// Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ�		Server -> Client
//
// ������ ���ӽ� ���ʷ� �ްԵǴ� ��Ŷ���� �ڽ��� �Ҵ���� ID ��
// �ڽ��� ���� ��ġ, HP �� �ް� �ȴ�. (ó���� �ѹ� �ް� ��)
//
// �� ��Ŷ�� ������ �ڽ��� ID,X,Y,HP �� �����ϰ� ĳ���͸� �������Ѿ� �Ѵ�.
//
//	4	-	ID
//	1	-	Direction
//	2	-	X
//	2	-	Y
//	1	-	HP
//
//---------------------------------------------------------------
void MakePacket_Create_My_Character(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y, BYTE HP) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Create_My_Character : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Create_My_Character : pPacket IS NULL.");
		CRASH();
	}



	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;
	*pPacket << HP;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Create_My_Character (%d %d %d %d %d)", SID, dir, X, Y, HP);
}


//---------------------------------------------------------------
// �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ		Server -> Client
//
// ó�� ������ ���ӽ� �̹� ���ӵǾ� �ִ� ĳ���͵��� ����
// �Ǵ� �����߿� ���ӵ� Ŭ���̾�Ʈ���� ���� �� ����.
//
//
//	4	-	ID
//	1	-	Direction
//	2	-	X
//	2	-	Y
//	1	-	HP
//
//---------------------------------------------------------------
void MakePacket_Create_Other_Character(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y, BYTE HP) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Create_Other_Character : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Create_Other_Character : pPacket IS NULL.");
		CRASH();
	}

	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;
	*pPacket << HP;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Create_Other_Character (%d %d %d %d %d)", SID, dir, X, Y, HP);
}


//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ						Server -> Client
//
// ĳ������ �������� �Ǵ� ĳ���Ͱ� �׾����� ���۵�.
//
//	4	-	ID
//
//---------------------------------------------------------------
void MakePacket_Delete_Character(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Delete_Character : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Delete_Character : pPacket IS NULL.");
		CRASH();
	}

	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_DELETE_CHARACTER;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Delete_Character (%d )", SID);
}


//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Server -> Client
//
// �ٸ� ������ ĳ���� �̵��� �� ��Ŷ�� �޴´�.
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�� �̵�ó���� ���ֵ��� �Ѵ�.
//
// ��Ŷ ���� �� �ش� Ű�� ����ؼ� ���������� �����ϰ�
// �ش� �������� ��� �̵��� �ϰ� �־�߸� �Ѵ�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ �� 8���� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
void MakePacket_Move_Start(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Move_Start : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Move_Start : pPacket IS NULL.");
		CRASH();
	}

	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_MOVE_START;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Move_Start (%d %d %d %d)", SID, dir, X, Y);
}


//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Server -> Client
//
// ID �� �ش��ϴ� ĳ���Ͱ� �̵��� ������̹Ƿ�
// ĳ���͸� ã�Ƽ� �����, ��ǥ�� �Է����ְ� ���ߵ��� ó���Ѵ�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
void MakePacket_Move_Stop(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Move_Stop : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Move_Stop : pPacket IS NULL.");
		CRASH();
	}

	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_MOVE_STOP;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Move_Stop (%d %d %d %d)", SID, dir, X, Y);
}


//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Server -> Client
//
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�Ƽ� ����1�� �������� �׼��� �����ش�.
// ������ �ٸ� ��쿡�� �ش� �������� �ٲ� �� ���ش�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
void MakePacket_Action_Attack1(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Action_Attack1 : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Action_Attack1 : pPacket IS NULL.");
		CRASH();
	}

	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_ATTACK1;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Action_Attack1 (%d %d %d %d)", SID, dir, X, Y);
}


//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Server -> Client
//
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�Ƽ� ����1�� �������� �׼��� �����ش�.
// ������ �ٸ� ��쿡�� �ش� �������� �ٲ� �� ���ش�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
void MakePacket_Action_Attack2(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Action_Attack2 : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Action_Attack2 : pPacket IS NULL.");
		CRASH();
	}

	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_ATTACK2;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Action_Attack2 (%d %d %d %d)", SID, dir, X, Y);
}


//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Server -> Client
//
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�Ƽ� ����1�� �������� �׼��� �����ش�.
// ������ �ٸ� ��쿡�� �ش� �������� �ٲ� �� ���ش�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
void MakePacket_Action_Attack3(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, BYTE dir, WORD X, WORD Y) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Action_Attack3 : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Action_Attack3 : pPacket IS NULL.");
		CRASH();
	}


	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << dir;
	*pPacket << X;
	*pPacket << Y;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_ATTACK3;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Action_Attack3 (%d %d %d %d)", SID, dir, X, Y);
}


//---------------------------------------------------------------
// ĳ���� ������ ��Ŷ							Server -> Client
//
// ���ݿ� ���� ĳ������ ������ ����.
//
//	4	-	AttackID	( ������ ID )
//	4	-	DamageID	( ������ ID )
//	1	-	DamageHP	( ������ HP )
//
//---------------------------------------------------------------
void MakePacket_Damage(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD AttackID, DWORD DamageID, BYTE HP) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Damage : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Damage : pPacket IS NULL.");
		CRASH();
	}


	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << AttackID;
	*pPacket << DamageID;
	*pPacket << HP;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_DAMAGE;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Damage (%d %d %d)", AttackID, DamageID, HP);
}



//---------------------------------------------------------------
// ����ȭ�� ���� ��Ŷ					Server -> Client
//
// �����κ��� ����ȭ ��Ŷ�� ������ �ش� ĳ���͸� ã�Ƽ�
// ĳ���� ��ǥ�� �������ش�.
//
//	4	-	ID
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
void MakePacket_Sync(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD SID, WORD X, WORD Y) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Sync : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Sync : pPacket IS NULL.");
		CRASH();
	}


	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << SID;
	*pPacket << X;
	*pPacket << Y;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_SYNC;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
	_LOG(dfLOG_LEVEL_DEBUG, L"MakePacket_Sync (%d %d %d)", SID, X, Y);
	g_syncCnt++;
}

//---------------------------------------------------------------
// Echo ���� ��Ŷ				Server -> Client
//
//	4	-	Time
//
//---------------------------------------------------------------

void MakePacket_Echo(st_PACKET_HEADER *pHeader, CPacket *pPacket, DWORD dwTime) {
	if (pHeader == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Echo : pHeader IS NULL.");
		CRASH();
	}
	if (pPacket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// MakePacket_Echo : pPacket IS NULL.");
		CRASH();
	}



	// ��Ŷ �ʱ�ȭ
	pPacket->Clear();

	// ��Ŷ �����
	*pPacket << dwTime;

	// ��� �����
	pHeader->byCode = dfPACKET_CODE;
	pHeader->byType = dfPACKET_SC_ECHO;
	pHeader->bySize = (BYTE) pPacket->GetDataSize();
}

#pragma endregion
