#include "pch.h"
#include "ContentCore.h"
#include "CFrameSkip.h"
#include "PacketProcess.h"
#include "Sector.h"
#include "NetworkCore.h"
// =========================
// ��������
// =========================

CObjectPool<CCharacter> g_characterPool(0, true);
std::map<DWORD, CCharacter *> g_characterMap; //ĳ���ͼǸ�
extern CFrameSkip g_Timer;
extern std::queue<SOCKET> g_delResvQ; // ������ ť
int g_err = 0;	// ������ - dfDTDms���� ����
BOOL g_dummytest = false;

BOOL ContentUpdate() {
	CCharacter *pCharacter = NULL;
	DWORD now = timeGetTime();
	// DELTATIME ����
	int dt = g_Timer.DeltaTime();
	int updateCount = (dt + g_err) / dfDTD;
	g_err = (dt + g_err) % dfDTD;

	if (updateCount!=1)
		_LOG(dfLOG_LEVEL_WARNING, L"DT[%d] updateCount[%d] err[%d]", dt, updateCount, g_err);
	else
		_LOG(dfLOG_LEVEL_DEBUG, L"DT[%d] updateCount[%d] err[%d]", dt, updateCount, g_err);

	if (updateCount < 1) return FALSE;

	for (auto iter = g_characterMap.begin(); iter != g_characterMap.end(); ++iter) {
		// ĳ���� �ϳ��ϳ� ������ ó��
		pCharacter = iter->second;
		if (pCharacter == NULL) { CRASH(); }

			// ���ó��
		if (pCharacter->_chHP < 0 && !g_dummytest) {
			DisconnectClient(pCharacter);
			continue;
		}

		//  �����ð� ���ž��� ����
		if (now - pCharacter->_pSession->_LastRecvTime > dfMAX_IDLE_TIME && !g_dummytest) {
			DisconnectClient(pCharacter);
			continue;
		}

		_LOG(dfLOG_LEVEL_DEBUG, L"BEFORE UPDATE : SID[%d] ACTION[%d] POS[%d %d]", pCharacter->_SID, pCharacter->_action, pCharacter->_X, pCharacter->_Y);
		// ���� ���ۿ� ���� ó��
		switch (pCharacter->_action) {
		case dfACTION_MOVE_LL:
			// ����
			if (CanGo(pCharacter->_X - (dfMOVE_PIXEL_HORIZONTAL * updateCount),
				pCharacter->_Y)) {
				pCharacter->_X -= (dfMOVE_PIXEL_HORIZONTAL * updateCount);
			}
			break;

		case dfACTION_MOVE_LU:
			// ���� ��
			if (CanGo(pCharacter->_X - (dfMOVE_PIXEL_HORIZONTAL * updateCount),
				pCharacter->_Y - (dfMOVE_PIXEL_VERTICAL * updateCount))) {
				pCharacter->_X -= (dfMOVE_PIXEL_HORIZONTAL * updateCount);
				pCharacter->_Y -= (dfMOVE_PIXEL_VERTICAL * updateCount);
			}
			break;

		case dfACTION_MOVE_UU:
			// ��
			if (CanGo(pCharacter->_X,
				pCharacter->_Y - (dfMOVE_PIXEL_VERTICAL * updateCount))) {
				pCharacter->_Y -= (dfMOVE_PIXEL_VERTICAL * updateCount);
			}
			break;

		case dfACTION_MOVE_RU:
			// ������ ��
			if (CanGo(pCharacter->_X + (dfMOVE_PIXEL_HORIZONTAL * updateCount),
				pCharacter->_Y - (dfMOVE_PIXEL_VERTICAL * updateCount))) {
				pCharacter->_X += (dfMOVE_PIXEL_HORIZONTAL * updateCount);
				pCharacter->_Y -= (dfMOVE_PIXEL_VERTICAL * updateCount);
			}
			break;

		case dfACTION_MOVE_RR:
			// ������
			if (CanGo(pCharacter->_X + (dfMOVE_PIXEL_HORIZONTAL * updateCount),
				pCharacter->_Y)) {
				pCharacter->_X += (dfMOVE_PIXEL_HORIZONTAL * updateCount);
			}
			break;

		case dfACTION_MOVE_RD:
			// ������ �Ʒ�
			if (CanGo(pCharacter->_X + (dfMOVE_PIXEL_HORIZONTAL * updateCount),
				pCharacter->_Y + (dfMOVE_PIXEL_VERTICAL * updateCount))) {
				pCharacter->_X += (dfMOVE_PIXEL_HORIZONTAL * updateCount);
				pCharacter->_Y += (dfMOVE_PIXEL_VERTICAL * updateCount);
			}
			break;

		case dfACTION_MOVE_DD:
			// �Ʒ�
			if (CanGo(pCharacter->_X,
				pCharacter->_Y + (dfMOVE_PIXEL_VERTICAL * updateCount))) {
				pCharacter->_Y += (dfMOVE_PIXEL_VERTICAL * updateCount);
			}
			break;

		case dfACTION_MOVE_LD:
			// ���� �Ʒ�
			if (CanGo(pCharacter->_X - (dfMOVE_PIXEL_HORIZONTAL * updateCount),
				pCharacter->_Y + (dfMOVE_PIXEL_VERTICAL * updateCount))) {
				pCharacter->_X -= (dfMOVE_PIXEL_HORIZONTAL * updateCount);
				pCharacter->_Y += (dfMOVE_PIXEL_VERTICAL * updateCount);
			}
			break;
		}
		_LOG(dfLOG_LEVEL_DEBUG, L"AFTER UPDATE : SID[%d] ACTION[%d] POS[%d %d]", pCharacter->_SID, pCharacter->_action, pCharacter->_X, pCharacter->_Y);
		//---------------------------------------------------------------
		// �̵��ΰ�� ���� ������Ʈ
		//---------------------------------------------------------------
		if (dfACTION_MOVE_LL <= pCharacter->_action && pCharacter->_action <= dfACTION_MOVE_LD) {
			if (Sector_UpdateCharacter(pCharacter)) {
				ChatacterSectorUpdatePacket(pCharacter);
			}
		}
	}
	return TRUE;
}



inline BOOL CanGo(int x, int y) {
	if (dfRANGE_MOVE_LEFT > x || x > dfRANGE_MOVE_RIGHT) return FALSE;
	if (dfRANGE_MOVE_TOP > y || y > dfRANGE_MOVE_BOTTOM) return FALSE;
	return TRUE;
}


void HitScanAndSend(CCharacter *pAttacker) {
	st_PACKET_HEADER header;
	CPacket packet;
	int dmg;
	int rangeX;
	int rangeY;
	int sectorX;
	int sectorY;
	std::list<CCharacter *> *pSectorList;
	CCharacter *pCharacter;
	// --------------------------------------------
	// 	   ���� ������Ʈ
	// --------------------------------------------
	if (Sector_UpdateCharacter(pAttacker)) {
		ChatacterSectorUpdatePacket(pAttacker);
	}
	// --------------------------------------------
	// 	   �׼� ��Ŷ ������
	// --------------------------------------------
	switch (pAttacker->_action) {
	case dfACTION_ATTACK1:
		MakePacket_Action_Attack1(&header, &packet, pAttacker->_SID, pAttacker->_direction, pAttacker->_X, pAttacker->_Y);
		dmg = dfATTACK1_DAMAGE;
		rangeX = dfATTACK1_RANGE_X;
		rangeY = dfATTACK1_RANGE_Y;
		break;
	case dfACTION_ATTACK2:
		MakePacket_Action_Attack2(&header, &packet, pAttacker->_SID, pAttacker->_direction, pAttacker->_X, pAttacker->_Y);
		dmg = dfATTACK2_DAMAGE;
		rangeX = dfATTACK2_RANGE_X;
		rangeY = dfATTACK2_RANGE_Y;
		break;
	case dfACTION_ATTACK3:
		MakePacket_Action_Attack3(&header, &packet, pAttacker->_SID, pAttacker->_direction, pAttacker->_X, pAttacker->_Y);
		dmg = dfATTACK3_DAMAGE;
		rangeX = dfATTACK3_RANGE_X;
		rangeY = dfATTACK3_RANGE_Y;
		break;
	default:
		_LOG(dfLOG_LEVEL_ERROR, L"HitScanAndSend( SID[%d] Action[%d] ):: NOT ACTTECK", pAttacker->_SID, pAttacker->_action);
		return;
	}
	SendSectorAround(pAttacker, &header, &packet);
	// --------------------------------------------
	// 	   ������ ����
	// --------------------------------------------
	if (pAttacker->_direction == dfACTION_MOVE_LL) {
		// ��������

		sectorX = pAttacker->_curSecPos._X;
		sectorY = pAttacker->_curSecPos._Y;


		// �����ִ� ����
		pSectorList = &g_sector[sectorY][sectorX];
		for (auto iter = pSectorList->begin(); iter != pSectorList->end(); ++iter) {
			pCharacter = *iter;
			if ((pAttacker->_X - pCharacter->_X) <= rangeX &&
				(pAttacker->_X - pCharacter->_X) > 0 &&
				abs(pAttacker->_Y - pCharacter->_Y) <= rangeY) {
				pCharacter->_chHP -= dmg;

				MakePacket_Damage(&header, &packet, pAttacker->_SID, pCharacter->_SID, pCharacter->_chHP);
				SendSectorAround(pAttacker, &header, &packet, TRUE);
			}
		}

		// ������ ���� (���ʸ�)
		sectorX -= 1;
		sectorY -= 1;

		if (sectorX >= 0) {
			for (int i = 0; i < 3; ++i, ++sectorY) {
				if (sectorY < 0 || sectorY >= dfSECTOR_MAX_Y) {
					continue;
				}

				pSectorList = &g_sector[sectorY][sectorX];
				for (auto iter = pSectorList->begin(); iter != pSectorList->end(); ++iter) {
					pCharacter = *iter;

					if ((pAttacker->_X - pCharacter->_X) <= rangeX &&
						(pAttacker->_X - pCharacter->_X) > 0 &&
						abs(pAttacker->_Y - pCharacter->_Y) <= rangeY) {
						pCharacter->_chHP -= dmg;

						MakePacket_Damage(&header, &packet, pAttacker->_SID, pCharacter->_SID, pCharacter->_chHP);
						SendSectorAround(pAttacker, &header, &packet, TRUE);
					}
				}
			}
		}


	} else if (pAttacker->_direction == dfACTION_MOVE_RR) {
		// ����������

		sectorX = pAttacker->_curSecPos._X;
		sectorY = pAttacker->_curSecPos._Y;


		// �����ִ� ����
		pSectorList = &g_sector[sectorY][sectorX];
		for (auto iter = pSectorList->begin(); iter != pSectorList->end(); ++iter) {
			pCharacter = *iter;
			if ((pCharacter->_X - pAttacker->_X) <= rangeX &&
				(pCharacter->_X - pAttacker->_X) > 0 &&
				abs(pAttacker->_Y - pCharacter->_Y) <= rangeY) {
				pCharacter->_chHP -= dmg;

				MakePacket_Damage(&header, &packet, pAttacker->_SID, pCharacter->_SID, pCharacter->_chHP);
				SendSectorAround(pAttacker, &header, &packet, TRUE);
			}
		}

		// ������ ���� (�����ʸ�)
		sectorX += 1;
		sectorY -= 1;

		if (sectorX < dfSECTOR_MAX_X) {
			for (int i = 0; i < 3; ++i, ++sectorY) {
				if (sectorY < 0 || sectorY >= dfSECTOR_MAX_Y) {
					continue;
				}

				pSectorList = &g_sector[sectorY][sectorX];
				for (auto iter = pSectorList->begin(); iter != pSectorList->end(); ++iter) {
					pCharacter = *iter;

					if ((pCharacter->_X - pAttacker->_X) <= rangeX &&
						(pCharacter->_X - pAttacker->_X) > 0 &&
						abs(pAttacker->_Y - pCharacter->_Y) <= rangeY) {
						pCharacter->_chHP -= dmg;

						MakePacket_Damage(&header, &packet, pAttacker->_SID, pCharacter->_SID, pCharacter->_chHP);
						SendSectorAround(pAttacker, &header, &packet, TRUE);
					}
				}
			}
		}
	}
}

void DisconnectClient(CCharacter *pCharacter) {
	//---------------------------
	// ���Ϳ� ���� �޽��� �Ѹ���
	//---------------------------
	// ��Ŷ 
	st_PACKET_HEADER header;
	CPacket packet;

	MakePacket_Delete_Character(&header, &packet, pCharacter->_SID);

	SendSectorAround(pCharacter, &header, &packet);

	// ���� ���� ť�� �ֱ�
	g_delResvQ.push(pCharacter->_pSession->_sock);

}


#pragma region WRAPPING
// �ڷᱸ�� �����̳� ����
CCharacter *CreateCharacter(DWORD SID) {
	//---------------------------
	// 	   ĳ�� ����
	//---------------------------

// �̹� �ִ� �������� Ȯ��
	auto iter = g_characterMap.find(SID);
	if (iter != g_characterMap.end()) {
		_LOG(dfLOG_LEVEL_ERROR, L"////////CreateCharacter() Session that already exists..");
		return NULL;
	}

	// ������ƮǮ���� �ϳ� �̾ƿ���
	CCharacter *pCharacter = g_characterPool.Alloc();

	// �����̳ʿ� �ֱ�
	g_characterMap.insert(std::make_pair(SID, pCharacter));
	_LOG(dfLOG_LEVEL_DEBUG, L"CreateCharacter :: SID[%d] OK..", SID);
	return pCharacter;

}
CCharacter *FindCharacter(DWORD SID) {
	//---------------------------
	// 	   ĳ�� �˻�
	//---------------------------

	// �̹� �ִ� ĳ������ Ȯ��
	auto iter = g_characterMap.find(SID);
	if (iter == g_characterMap.end()) {
		// ���� NULL
		_LOG(dfLOG_LEVEL_ERROR, L"////////FindCharacter() Session not found..");
		return NULL;
	}
	// ���� ����
	_LOG(dfLOG_LEVEL_DEBUG, L"FindCharacter() OK..");
	return iter->second;
}
BOOL DeleteCharacter(DWORD SID) {
	//---------------------------
	// 	   ĳ�� ����
	//---------------------------

// �̹� �ִ� �������� Ȯ��
	auto iter = g_characterMap.find(SID);
	if (iter == g_characterMap.end()) {
		// �������� �ʴ� ���� ��������
		_LOG(dfLOG_LEVEL_ERROR, L"////////DeleteCharacter() Session not found..");
		return FALSE;
	}

	// ���Ϳ��� ����
	Sector_RemoveCharacter(iter->second);

	// ������Ʈ Ǯ�� ��ȯ
	g_characterPool.Free(iter->second);
	// �����̳ʿ��� ����
	g_characterMap.erase(iter);

	_LOG(dfLOG_LEVEL_DEBUG, L"DeleteCharacter() OK..");
	return true;
}
#pragma endregion
