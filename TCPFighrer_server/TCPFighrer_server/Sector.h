#pragma once

// ������ǥ
struct st_SECTOR_POS {
	int _X;
	int _Y;
};

// �ֺ� ������ǥ
struct st_SECTOR_AROUND{
	int _count;
	st_SECTOR_POS _around[9];
};

class CCharacter; // ���漱��

//////////////////////////////////////////////////////////////////////////
// ĳ������ ����ǥ�� ������ǥ�� ����ؼ� �ش� ���Ϳ� ����
// 
// Parameters: CCharacter *.
// Return: ����.
//////////////////////////////////////////////////////////////////////////
void Sector_AddCharacter(CCharacter *pCharacter);

//////////////////////////////////////////////////////////////////////////
// ĳ������ ������ǥ�� �ش��ϴ� ���Ϳ��� �� ĳ���͸� ����
// 
// Parameters: CCharacter *.
// Return:  ����.
//////////////////////////////////////////////////////////////////////////
void Sector_RemoveCharacter(CCharacter *pCharacter);

//////////////////////////////////////////////////////////////////////////
// ���� ��ġ�� ���Ϳ��� �������� ���� ��ǥ�� ���Ӱ� ����
// 
// Parameters: CCharacter *.
// Return: (BOOL) ���� / ����
//////////////////////////////////////////////////////////////////////////
BOOL Sector_UpdateCharacter(CCharacter *pCharacter);


//////////////////////////////////////////////////////////////////////////
// ������ǥ �������� �ֺ� ����� ���
// 
// Parameters: int , int , st_SECTOR_AROUND *.
// Return: ����.
//////////////////////////////////////////////////////////////////////////
void GetSectorAround(int iSecX, int iSecY, st_SECTOR_AROUND *pSectorAround);

//////////////////////////////////////////////////////////////////////////
// ĳ���Ͱ� ���͸� �̵��� �� ����/ ���� ������ ���� ���
// 
// Parameters: CCharacter *,   st_SECTOR_AROUND *, st_SECTOR_AROUND *.
// Return: ����.
//////////////////////////////////////////////////////////////////////////
void GetUpdateSectorAround(CCharacter *pCharacter,   st_SECTOR_AROUND *pRemoveSector, st_SECTOR_AROUND *pAddSector);


//////////////////////////////////////////////////////////////////////////
// ���� ���Ϳ��� ������ �κп� - �ɸ��� ���� �޽���
// �̵��ϴ� ĳ���Ϳ��� �������Ϳ��� ���ܵ� �κ��� ĳ������ �����޽���
// �����߰��� ���Ϳ� - ĳ���� ���� + �̵� �޽���
// �̵��ϴ� ĳ���Ϳ��� - ���� ������ ������ ĳ���� ���� �޽���
// 
// Parameters: CCharacter *
// Return: ����.
//////////////////////////////////////////////////////////////////////////
void ChatacterSectorUpdatePacket(CCharacter *pCharacter);


extern std::list<CCharacter *> g_sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];
