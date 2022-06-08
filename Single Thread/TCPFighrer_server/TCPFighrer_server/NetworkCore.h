#pragma once


#include "CRingBuffer.h"
#include "Protocol.h"
#include "Session.h"
#include "CPacket.h"
#include "Character.h"

//#define PRINTSTEP

//////////////////////////////////////////////////////////////////////////
// 리슨소켓을 바인딩하고 비동기 소켓으로 설정한다
// 
// Parameters: 없음.
// Return: (BOOL) 성공 / 실패
//////////////////////////////////////////////////////////////////////////
BOOL NetworkInitServer(); // 초기화

//////////////////////////////////////////////////////////////////////////
// 접속한 클라이언트를 63, 리슨소켓을 1로 나뉘어 select모델을 돌린다
// 
// Parameters: 없음.
// Return: (BOOL) 성공 / 실패
//////////////////////////////////////////////////////////////////////////
BOOL NetworkPorc(); // 네트워크 동작// select 모델을 돌릴 루프 



//////////////////////////////////////////////////////////////////////////
// NetworkPorc()에서 세팅한 select 셋을 select()를 호출후, 그에 맞는 처리한다 
// 
// Parameters: DWORD *, SOCKET *, FD_SET *, FD_SET *
// Return: (BOOL) 성공 / 실패
//////////////////////////////////////////////////////////////////////////
BOOL SelectProc( SOCKET *pTableSocket, FD_SET *pReadSet, FD_SET *pWriteSet);	// select()가 돌아갈 함수

//////////////////////////////////////////////////////////////////////////
// 리슨의 백로그에 뭔가 와서 accept로 빼온다!
// 
// Parameters: 없음
// Return: (BOOL) 성공 / 실패
//////////////////////////////////////////////////////////////////////////
BOOL AcceptProc();	// Accept() 처리시 처리해야할 일 (플레이어 생성 등)




// send
#pragma region Send
//////////////////////////////////////////////////////////////////////////
// send링버퍼에 있는 내용을 클라이언트의 소켓으로 보낸다
// 
// Parameters: CSession *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
BOOL SendProc(CSession *pSession);	

//////////////////////////////////////////////////////////////////////////
// pSession의 send링버퍼에 헤더 + 직렬화 패킷을 넣는다
// 
// Parameters: CSession *, st_PACKET_HEADER *, CPacket *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
void SendUnicast(CSession *pSession, st_PACKET_HEADER *pHeader, CPacket *pPacket);


//////////////////////////////////////////////////////////////////////////
// 어느 섹터에 있는 세션에 브로드 캐스팅
// 
// Parameters: CSession *, st_PACKET_HEADER *, CPacket *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
void SendSectorOne(int secX, int secY, CSession *pSessionEX, st_PACKET_HEADER *pHeader, CPacket *pPacket);


//////////////////////////////////////////////////////////////////////////
// pCharacter 주변섹터에 있는 모두에게 브로드 캐스팅 (최대 9개 영역)
// 
// Parameters: CSession *, st_PACKET_HEADER *, CPacket *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
void SendSectorAround(CCharacter *pCharacter, st_PACKET_HEADER *pHeader, CPacket *pPacket, BOOL SendMe = FALSE);


//////////////////////////////////////////////////////////////////////////
// 클라이언트 컨테이너를 순회하며 send링버퍼에 헤더 + 직렬화 패킷을 넣는다
// 
// Parameters: CSession *, st_PACKET_HEADER *, CPacket *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
void SendBroadcast(CSession *pSessionEX, st_PACKET_HEADER *pHeader, CPacket *pPacket);

#pragma endregion





// recv
#pragma region Recv
//////////////////////////////////////////////////////////////////////////
// 해당 클라이언트의 recv링버퍼에 온 내용을 넣고 RecvPacket()를 호출한다
// 
// Parameters: CSession *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
BOOL RecvProc(CSession *pSession);	

//////////////////////////////////////////////////////////////////////////
// recv링버퍼에 온 내용을 직렬화 버퍼에 넣고 컨텐츠처리 함수를 호출한다
// 
// Parameters: CSession *
// Return: 없음
//////////////////////////////////////////////////////////////////////////
int	RecvPacket(CSession *pSession);
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// 이 아이디를 가진 클라를 탐색후 삭제한다
// 
// Parameters: DWORD
// Return: 없음
//////////////////////////////////////////////////////////////////////////
BOOL Disconnect(SOCKET sock);

//////////////////////////////////////////////////////////////////////////
// 리슨소켓을 닫고 네트워크를 정리한다.
// 
// Parameters: 없음
// Return: 없음
//////////////////////////////////////////////////////////////////////////
void NetworkCloseServer(); // 정리



#pragma region WRAPPING
// 자료구조 컨테이너 래핑
CSession *CreateSession(SOCKET sock);
CSession *FindSession(SOCKET sock);
BOOL DeleteSession(SOCKET sock);

#pragma endregion


extern std::map<SOCKET, CSession *> g_sessionMap; // 세션맵
extern std::queue<SOCKET> g_delResvQ;