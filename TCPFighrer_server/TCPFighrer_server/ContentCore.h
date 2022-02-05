#pragma once
#include "Character.h"


//////////////////////////////////////////////////////////////////////////
// �������� ���η���
// 
// Parameters: ����
// Return: ����.
//////////////////////////////////////////////////////////////////////////
BOOL ContentUpdate();

//////////////////////////////////////////////////////////////////////////
// �ش� ��ǥ�� �� �� �ֳ�?
// 
// Parameters: ����
// Return: BOOL.
//////////////////////////////////////////////////////////////////////////
inline BOOL CanGo(int x, int y);

//////////////////////////////////////////////////////////////////////////
// �׼��� ��ó���Ϳ� ������
// pAttacker�� ��ó���Ϳ� ��Ʈ ��ĵ
// ������ ����
// 
// Parameters: CCharacter *, 
// Return: void
//////////////////////////////////////////////////////////////////////////
void HitScanAndSend(CCharacter *pAttacker);

//////////////////////////////////////////////////////////////////////////
// ĳ���Ͱ� ������������ ������ ��Ȳ���� ����
// HP�� 0�̰ų� �����ð� ������ �ְų�
// 
// Parameters: CCharacter *, 
// Return: void
//////////////////////////////////////////////////////////////////////////
void DisconnectClient(CCharacter *pCharacter);

#pragma region WRAPPING
// �ڷᱸ�� �����̳� ����
CCharacter *CreateCharacter(DWORD SID);
CCharacter *FindCharacter(DWORD SID);
BOOL DeleteCharacter(DWORD SID);
#pragma endregion


extern BOOL g_dummytest ;