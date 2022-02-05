#include "pch.h"
#include "NetworkCore.h"
#include "PacketProcess.h"
#include "CTimer.h"
// =========================
// �������� - ����
// g_wsaData
// g_listenAddr
// g_listenSock : ���� ����
// =========================

WSADATA g_wsaData;		// ����
SOCKADDR_IN g_listenAddr;	// ���� ���� ����
SOCKET g_listenSock;	// ���� ����

// �������� - ����
std::map<SOCKET, CSession *> g_sessionMap; // ���Ǹ�
std::queue<SOCKET> g_delResvQ; // ������ ť
extern std::list<CCharacter *> g_sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X]; // ����
int g_IDGenerator = 0;
extern int g_syncCnt;

// ����͸�
CObjectPool <CSession> g_sessionPool(0, true);
CTimer g_sendTPS;
CTimer g_recvTPS;

BOOL NetworkInitServer() {
	_LOG(dfLOG_LEVEL_ERROR, L"NetworkInitServer()..");
	int setRet;// ���� ���ῡ ������ ����

		// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &g_wsaData) != 0) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// WSAStartup() errcode[%d]\n", WSAGetLastError());
		return FALSE;
	}
	_LOG(dfLOG_LEVEL_ERROR, L"WSAStartup OK..");

	// ���� �⺻����
	memset(&g_listenAddr, 0, sizeof(g_listenAddr));
	g_listenAddr.sin_family = AF_INET;
	g_listenAddr.sin_port = htons(dfNETWORK_PORT);
	InetPton(AF_INET, L"0.0.0.0", &g_listenAddr.sin_addr);
	_LOG(dfLOG_LEVEL_ERROR, L"PORT [%d] g_listenAddr.sin_addr [%x] ", dfNETWORK_PORT, g_listenAddr.sin_addr);

	// socket()
	g_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_listenSock == INVALID_SOCKET) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// socket() errcode[%d]\n", WSAGetLastError());
		return FALSE;
	}
	_LOG(dfLOG_LEVEL_ERROR, L"g_listenSock OK [%lld]..", g_listenSock);

	// bind()
	setRet = bind(g_listenSock, (SOCKADDR *) &g_listenAddr, sizeof(g_listenAddr));
	if (setRet == SOCKET_ERROR) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// bind() errcode[%d]\n", WSAGetLastError());
		return FALSE;
	}
	_LOG(dfLOG_LEVEL_ERROR, L"bind() OK [%d] ..", setRet);

	// listen()
	setRet = listen(g_listenSock, SOMAXCONN);
	if (setRet == SOCKET_ERROR) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// listen() errcode[%d]\n", WSAGetLastError());
		return FALSE;
	}
	_LOG(dfLOG_LEVEL_ERROR, L"listen() OK [%d] ..", setRet);

	// �񵿱� Ű��
	u_long on = 1;
	setRet = ioctlsocket(g_listenSock, FIONBIO, &on);
	if (setRet == SOCKET_ERROR) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// ioctlsocket() errcode[%d]\n", WSAGetLastError());
		return FALSE;
	}
	_LOG(dfLOG_LEVEL_ERROR, L"FIONBIO ON [%d] ..", setRet);
	_LOG(dfLOG_LEVEL_ERROR, L"NetworkInitServer() OK..\n");
	return TRUE;
}

BOOL NetworkPorc() {
	_LOG(dfLOG_LEVEL_DEBUG, L"NetworkPorc()..");

	CSession *pSession;
	SOCKET UserTable_SOCKET[FD_SETSIZE];
	int iSockCnt = 0;
	//g_syncCnt = 0;

	//---------------------------
	// 64���� ������ �ϱ�!
	//---------------------------
	FD_SET ReadSet;
	FD_SET WriteSet;
	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
	_LOG(dfLOG_LEVEL_DEBUG, L"INIT SET OK..");

	//---------------------------
	// �������� �ֱ�
	//---------------------------
	FD_SET(g_listenSock, &ReadSet);
	UserTable_SOCKET[iSockCnt] = g_listenSock;
	iSockCnt++;
	_LOG(dfLOG_LEVEL_DEBUG, L"INSERT LISTEN SOCK OK..");

	//---------------------------
	// �׿� ���� �ֱ�
	//---------------------------
	_LOG(dfLOG_LEVEL_DEBUG, L"SELECT START..");
	for (auto seIter = g_sessionMap.begin(); seIter != g_sessionMap.end();) {
		pSession = seIter->second;
		++seIter;
		// ���� ���� ���
		UserTable_SOCKET[iSockCnt] = pSession->_sock;

		FD_SET(pSession->_sock, &ReadSet);
		if (pSession->_sendQ.GetUseSize() > 0)
			FD_SET(pSession->_sock, &WriteSet);

		iSockCnt++;
		//---------------------------
		// select ó�������� �ִ�ġ ����
		//---------------------------
		if (FD_SETSIZE <= iSockCnt) {
			// select()
			_LOG(dfLOG_LEVEL_DEBUG, L"CALL SelectProc() iSockCnt[%d] ", iSockCnt);
			SelectProc(UserTable_SOCKET, &ReadSet, &WriteSet);
			_LOG(dfLOG_LEVEL_DEBUG, L"SelectProc() iSockCnt[%d] OK..", iSockCnt);

			// �ʱ�ȭ
			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);
			memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
			iSockCnt = 0;
			_LOG(dfLOG_LEVEL_DEBUG, L"RESET SET OK..");

			// �������� �ֱ�
			FD_SET(g_listenSock, &ReadSet);
			UserTable_SOCKET[iSockCnt] = g_listenSock;
			iSockCnt++;
			_LOG(dfLOG_LEVEL_DEBUG, L"INSERT LISTEN SOCK OK..");
		}
	}
	//---------------------------
	// �����ִ� ���ǵ� ì���ֱ�
	//---------------------------
	if (iSockCnt > 0) {
		_LOG(dfLOG_LEVEL_DEBUG, L"CALL SelectProc() iSockCnt[%d] ", iSockCnt);
		SelectProc(UserTable_SOCKET, &ReadSet, &WriteSet);
		_LOG(dfLOG_LEVEL_DEBUG, L"SelectProc() iSockCnt[%d] OK..", iSockCnt);
	}
	_LOG(dfLOG_LEVEL_DEBUG, L"SELECT OK..");

	//---------------------------
	// ���������� �������Ұ� ��Ƶξ��ٰ� �̶� ��� ����
	//---------------------------
	while (g_delResvQ.empty() == false) {
		SOCKET delsock = g_delResvQ.front();
		g_delResvQ.pop();
		Disconnect(delsock);
	}


	return TRUE;
}

BOOL SelectProc(SOCKET *pTableSocket, FD_SET *pReadSet, FD_SET *pWriteSet) {
	if (pTableSocket == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// SelectProc : pTableSocket IS NULL.");
		CRASH();
	}

	_LOG(dfLOG_LEVEL_DEBUG, L"\nSelectProc()..\n");

	//---------------------------
	// ��������
	//---------------------------
	CSession *pSession;
	timeval timeout;

	//---------------------------
	// ���ð�����
	//---------------------------
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	//---------------------------
	// select()
	//---------------------------
	int selRet = select(0, pReadSet, pWriteSet, NULL, &timeout);
	if (selRet > 0) {
		//---------------------------
		// 0��°�� ������ ����üũ
		// ��α� ť�� ���� �Դ�! - Accept
		//---------------------------
		if (FD_ISSET(g_listenSock, pReadSet)) {
			if (AcceptProc()) {
				_LOG(dfLOG_LEVEL_DEBUG, L"Accept Ok..");
			} else {
				_LOG(dfLOG_LEVEL_ERROR, L"Accept FAIL..");
			}
		}
		//---------------------------
		// Ŭ���̾�Ʈ üũ
		//---------------------------
		for (int i = 1; i < FD_SETSIZE; i++) {
			if (pTableSocket[i] == INVALID_SOCKET) continue;

			//---------------------------
			// �б�
			//---------------------------
			if (FD_ISSET(pTableSocket[i], pReadSet)) {
				pSession = FindSession(pTableSocket[i]);
				if (pSession == NULL) {
					_LOG(dfLOG_LEVEL_ERROR, L"///// pReadSet) COULD NOT FIND SESSION");
					CRASH();
				}
				_LOG(dfLOG_LEVEL_DEBUG, L"SID[%d] CALL RECV()", pSession->_SID);
				if (RecvProc(pSession)) {
					_LOG(dfLOG_LEVEL_DEBUG, L"SID[%d] RECV() OK..", pSession->_SID);
				} else {
					_LOG(dfLOG_LEVEL_WARNING, L"///// SID[%d] RECV() ERROR", pSession->_SID);
				}
			}
			//---------------------------
			// ����
			//---------------------------
			if (FD_ISSET(pTableSocket[i], pWriteSet)) {
				pSession = FindSession(pTableSocket[i]);
				if (pSession == NULL) {
					_LOG(dfLOG_LEVEL_ERROR, L"///// pWriteSet) COULD NOT FIND SESSION");
					CRASH();
				}

				_LOG(dfLOG_LEVEL_DEBUG, L"SID[%d] CALL SEND()", pSession->_SID);
				if (SendProc(pSession)) {
					_LOG(dfLOG_LEVEL_DEBUG, L"SID[%d] SEND() OK..", pSession->_SID);
				} else {
					_LOG(dfLOG_LEVEL_WARNING, L"///// SID[%d] SEND() ERROR", pSession->_SID);
				}
			}
		}
	} else {
		//---------------------------
		// �Ұ� ����
		//---------------------------
		return FALSE;
	}

	return TRUE;
}

BOOL AcceptProc() {
	_LOG(dfLOG_LEVEL_DEBUG, L"\nAcceptProc()..");
	
	//---------------------------
	// 	   ��������
	//---------------------------
	// TODO Ŭ���̾�Ʈ
	SOCKET sock;// �ӽ� ����
	SOCKADDR_IN addr; // �ӽ� �ּ�
	int addrlen = sizeof(addr);

	//---------------------------
	// accept()
	//---------------------------
	_LOG(dfLOG_LEVEL_ERROR, L"CALL ACCEPT() :: ID [%d]", g_IDGenerator);
	sock = accept(g_listenSock, (SOCKADDR *) &addr, &addrlen);
	if (sock == INVALID_SOCKET) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// ACCEPT() errcode[%d]\n", WSAGetLastError());
		return FALSE;
	}
	//---------------------------
	//  ĳ���� ����
	//---------------------------
	_LOG(dfLOG_LEVEL_DEBUG, L"Create Session\n");
	CSession *pSession = CreateSession(sock);
	g_IDGenerator++;
	pSession->_SID = g_IDGenerator;
	pSession->_sock = sock;
	pSession->_recvQ.ClearBuffer();
	pSession->_sendQ.ClearBuffer();
	pSession->_LastRecvTime = timeGetTime();
	if (InstantiateCharacter(pSession)) {

		_LOG(dfLOG_LEVEL_DEBUG, L"Create Session OK..\n");
	} else {
		_LOG(dfLOG_LEVEL_ERROR, L"Create Session FAILE..\n");
		return FALSE;
	}
	return TRUE;
}

#pragma region SEND


BOOL SendProc(CSession *pSession) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"//////SendProc() pSession IS NULL");
		CRASH();
	}
	_LOG(dfLOG_LEVEL_DEBUG, L"\nSendProc(%d)..", pSession->_SID);
	g_sendTPS.Check();
	//---------------------------
	// 	   ��������
	//---------------------------
	int sendRet;

	//---------------------------
	// 	   send()
	//---------------------------
	sendRet = send(pSession->_sock, pSession->_sendQ.GetFrontBufferPtr(), pSession->_sendQ.DirectDequeueSize(), 0);
	if (sendRet == SOCKET_ERROR) {
		DWORD errcode = GetLastError();
		if (errcode != WSAEWOULDBLOCK) {
			_LOG(dfLOG_LEVEL_WARNING, L"////// SEND() errcode[%d]\n", errcode);
			//---------------------------
			// delqueue�� �־���ٰ� ���߿� ���� ����
			//---------------------------
			g_delResvQ.push(pSession->_sock);
			return FALSE;
		}
	}
	//---------------------------
	// 	   ������ŭ sendQ �ڷ�...
	//---------------------------
	pSession->_sendQ.MoveFront(sendRet);
	_LOG(dfLOG_LEVEL_DEBUG, L"SEND [%d byte]\n", sendRet);
	return TRUE;
}

void SendUnicast(CSession *pSession, st_PACKET_HEADER *pHeader, CPacket *pPacket) {
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// SendUnicast() :: pSession IS NULL");
		return;
	}
	//---------------------------
	// pSession�� send �����ۿ� �޽��� �ֱ�
	//---------------------------
	int enq;
	enq = pSession->_sendQ.Enqueue((char *) pHeader, sizeof(st_PACKET_HEADER));
	enq += pSession->_sendQ.Enqueue(pPacket->GetBufferPtr(), pPacket->GetDataSize());
	_LOG(dfLOG_LEVEL_DEBUG, L"[%d]._sendQ.Enqueue [%d byte] ", pSession->_SID,enq);
	_LOG(dfLOG_LEVEL_DEBUG, L"SendUnicast() OK..\n");
}

void SendSectorOne(int secX, int secY, CSession *pSessionEX, st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	std::list<CCharacter *> *pSectorList; // ���� ����Ʈ
	CSession *pSession;
	pSectorList = &g_sector[secY][secX]; // ��ǥ�� ����Ʈ ���

	// �׼����� �÷��̾� ��ȸ
	for (auto iter = pSectorList->begin(); iter != pSectorList->end(); ++iter) {
		pSession = (*iter)->_pSession;
		// ����
		if (pSession == pSessionEX) continue;
		SendUnicast(pSession, pHeader, pPacket);
	}



	_LOG(dfLOG_LEVEL_DEBUG, L"SendSectorOne(%d, %d) OK..\n", secX, secY);
}

void SendSectorAround(CCharacter *pCharacter, st_PACKET_HEADER *pHeader, CPacket *pPacket, BOOL SendMe) {
	std::list<CCharacter *> *pSectorList; // ���� ����Ʈ
	st_SECTOR_AROUND around; // �ֺ����� Ȯ��
	// �ֺ� ���
	GetSectorAround(pCharacter->_curSecPos._X, pCharacter->_curSecPos._Y, &around);
	// �ֺ� ��ȸ
	for (int i = 0; i < around._count; i++) {
		if (SendMe) {
			// ������
			SendSectorOne(around._around[i]._X, around._around[i]._Y, NULL, pHeader, pPacket);
		} else {
			// �� ����
			SendSectorOne(around._around[i]._X, around._around[i]._Y, pCharacter->_pSession, pHeader, pPacket);
		}
	}
	_LOG(dfLOG_LEVEL_DEBUG, L"SendSectorAround(sid[%d] secPos[%d, %d]) OK..\n", pCharacter->_SID, pCharacter->_curSecPos._X, pCharacter->_curSecPos._Y);
}

void SendBroadcast(CSession *pSessionEX, st_PACKET_HEADER *pHeader, CPacket *pPacket) {
	_LOG(dfLOG_LEVEL_DEBUG, L"SendBroadcast()..");
	//---------------------------
	// pSessionEX�� ������ ��� ��󿡰�
	//---------------------------
	CSession *pSession;
	for (auto iter = g_sessionMap.begin(); iter != g_sessionMap.end(); ++iter) {
		pSession = iter->second;
		if (pSession != pSessionEX)
			SendUnicast(pSession, pHeader, pPacket);
	}
}
#pragma endregion


#pragma region RECV
BOOL RecvProc(CSession *pSession) {
	_LOG(dfLOG_LEVEL_DEBUG, L"nRecvProc()..");

	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// RecvProc() :: pSession IS NULL");
		return FALSE;
	}
	g_recvTPS.Check();
	//---------------------------
	// 	   ��������
	//---------------------------
	int recvRet;

	//---------------------------
	// 	   recv()
	//---------------------------
	recvRet = recv(pSession->_sock, pSession->_recvQ.GetRearBufferPtr(), pSession->_recvQ.DirectEnqueueSize(), 0);
	if (recvRet == SOCKET_ERROR || recvRet == 0) {
		DWORD errcode = GetLastError();
		if (errcode != WSAEWOULDBLOCK) {
			_LOG(dfLOG_LEVEL_WARNING, L"////// recv() errcode[%d]\n", errcode);
			//---------------------------
			// ť�� �־���ٰ� ���߿� �����
			//---------------------------
			g_delResvQ.push(pSession->_sock);
			return FALSE;
		}
	}
	//---------------------------
	// 	  ������ŭ Rear�����
	//---------------------------
	pSession->_recvQ.MoveRear(recvRet);
	_LOG(dfLOG_LEVEL_DEBUG, L"RECV [%d byte]\n", recvRet);

	//---------------------------
	// 	  ��Ŷ ó���Լ� ȣ��
	//---------------------------
	int packRet = RecvPacket(pSession);
	if (packRet > 0) {
		_LOG(dfLOG_LEVEL_DEBUG, L"Packet Process [%d]..\n", packRet);
	} else {
		_LOG(dfLOG_LEVEL_ERROR, L"Packet Process ERROR [%d]..\n", packRet);
	}

	pSession->_LastRecvTime = timeGetTime();

	return TRUE;
}

int RecvPacket(CSession *pSession) {
	_LOG(dfLOG_LEVEL_DEBUG, L"RecvPacket()..");
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// RecvPacket() :: pSession IS NULL");
		return -1;
	}
	//---------------------------
	// ťó��
	// ��� Ȯ���ϰ�
	// ���빰 ����ȭ���۷�
	//---------------------------

	while (true) {

		st_PACKET_HEADER header; // ��������
		int iRecvQsize = pSession->_recvQ.GetUseSize();
		if (sizeof(st_PACKET_HEADER) > iRecvQsize) {
			//---------------------------
			// ������� �����־ �Ұ��� ����
			//---------------------------
			if(iRecvQsize != 0)
				_LOG(dfLOG_LEVEL_WARNING, L"_recvQ.GetUseSize() Less than the header [%d byte] :: sid[%d]\n", iRecvQsize, pSession->_SID);
			return 1;
		}

		//---------------------------
		// ��Ŷ�˻�
		// 1. �츮 ��ٰ� �´���?
		//---------------------------
		pSession->_recvQ.Peek((char *) &header, sizeof(st_PACKET_HEADER));
		_LOG(dfLOG_LEVEL_DEBUG, L"header byCode[%hhx] bySize[%hhu] byType[%hhx]\n",
			header.byCode, header.bySize, header.byType);

		if (dfPACKET_CODE != header.byCode) {
			// �̻��� ���
			_LOG(dfLOG_LEVEL_ERROR, L"Invalid header code [%hhx]\n", header.byCode);
			g_delResvQ.push(pSession->_sock); // ���� ����
			return 0xFF;
		}

		//---------------------------
		// 2. payload�� �����ŭ �Դ���?
		//---------------------------
		if (header.bySize + sizeof(st_PACKET_HEADER) > iRecvQsize) {
			// ���Դ�
			_LOG(dfLOG_LEVEL_WARNING, L"Heather that's not all here SID[%d] \n\tpacket size[%hhu byte] actual payload[%d byte]\n",pSession->_SID,
				header.bySize + (BYTE) sizeof(st_PACKET_HEADER), iRecvQsize);
			return 1;
		}

		//---------------------------
		// �ϴ� ���̷ε�� �߿���
		// ����� �˰������� ���ú�ť �ڷ� �о��ֱ�
		//---------------------------
		_LOG(dfLOG_LEVEL_DEBUG, L"recvQ USED [%d byte]\n", iRecvQsize);
		pSession->_recvQ.MoveFront(sizeof(st_PACKET_HEADER));

		//---------------------------
		// ����ȭ ���ۿ� �������ֱ�
		//---------------------------
		_LOG(dfLOG_LEVEL_DEBUG, L"Copy to serialization buffer...\n", );
		CPacket clPacket; // ����ȭ ����
		iRecvQsize = pSession->_recvQ.Dequeue(clPacket.GetBufferPtr(), header.bySize);
		if (header.bySize != iRecvQsize) {
			// ������!
			_LOG(dfLOG_LEVEL_ERROR, L"Failed to copy the size [%hhu] [%d]\n", header.byCode, iRecvQsize);
			return -1;
		}
		//---------------------------
		// �����Ϳ� ���� ���������� ũ�⸸ŭ ���� ������ ����
		//---------------------------
		clPacket.MoveWritePos(header.bySize);
		_LOG(dfLOG_LEVEL_DEBUG, L"Copy completed...\n", );


		//---------------------------
		// ��Ŷ ó���Լ� (��Ʈ��ũ - ������ �����) ȣ��
		//---------------------------
		_LOG(dfLOG_LEVEL_DEBUG, L"Call PacketProc()...\n", );
		if (PacketProc(pSession, &clPacket, header.byType) == FALSE) {
			// ��Ŷó�� ����
			_LOG(dfLOG_LEVEL_ERROR, L"PacketError ..\n", );
			return -1;
		}
		_LOG(dfLOG_LEVEL_DEBUG, L"PacketProc() OK..\n", );
	}

	return 0;
}

#pragma endregion

BOOL Disconnect(SOCKET sock) {
	if (sock == INVALID_SOCKET) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// Disconnect() :: sock IS INVALID_SOCKET");
		return FALSE;
	}

	_LOG(dfLOG_LEVEL_DEBUG, L"Disconnect()..");
	//---------------------------
	// ť�� �ִ°� �ϳ��� ���� ����
	//---------------------------
	CSession *pSession = FindSession(sock);
	if (pSession == NULL) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// Disconnect() :: pSession IS NULL");
		return FALSE;
	}
	//---------------------------
	// �������
	//---------------------------
	closesocket(pSession->_sock);
	_LOG(dfLOG_LEVEL_DEBUG, L"closesocket() OK..\n");

	//---------------------------
	// Ŭ�� �������� �����
	//---------------------------
	DeleteCharacter(pSession->_SID);
	DeleteSession(sock);
	_LOG(dfLOG_LEVEL_DEBUG, L"DeleteSession() OK..\n");

	_LOG(dfLOG_LEVEL_DEBUG, L"Disconnect() OK..\n");
	return TRUE;
}
void NetworkCloseServer() {
	//---------------------------
	// ��������
	//---------------------------
	_LOG(dfLOG_LEVEL_WARNING, L"NetworkCloseServer()..");
	WSACleanup();
	_LOG(dfLOG_LEVEL_WARNING, L"WSACleanup OK..");
	closesocket(g_listenSock);
	_LOG(dfLOG_LEVEL_WARNING, L"g_listenSock OK..");
	_LOG(dfLOG_LEVEL_WARNING, L"NetworkCloseServer() OK..\n");
}


#pragma region WRAPPING
CSession *CreateSession(SOCKET sock) {
	if (sock == INVALID_SOCKET) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// CreateSession() :: sock IS INVALID_SOCKET");
		return NULL;
	}


	//---------------------------
	// 	   ���� ����
	//---------------------------

	// �̹� �ִ� �������� Ȯ��
	auto iter = g_sessionMap.find(sock);
	if (iter != g_sessionMap.end()) {
		_LOG(dfLOG_LEVEL_ERROR, L"////////CreateSession() Session that already exists..");
		return NULL;
	}

	// ������ƮǮ���� �ϳ� �̾ƿ���
	CSession *pSession = g_sessionPool.Alloc();

	// �����̳ʿ� �ֱ�
	g_sessionMap.insert(std::make_pair(sock, pSession));
	_LOG(dfLOG_LEVEL_DEBUG, L"CreateSession() OK..");
	return pSession;
}

CSession *FindSession(SOCKET sock) {
	if (sock == INVALID_SOCKET) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// FindSession() :: sock IS INVALID_SOCKET");
		return NULL;
	}

	//---------------------------
	// 	   ���� �˻�
	//---------------------------

	// �̹� �ִ� �������� Ȯ��
	auto iter = g_sessionMap.find(sock);
	if (iter == g_sessionMap.end()) {
		// ���� NULL
		_LOG(dfLOG_LEVEL_ERROR, L"////////FindSession() Session not found..");
		return NULL;
	}
	// ���� ����
	_LOG(dfLOG_LEVEL_DEBUG, L"FindSession() OK..");
	return iter->second;
}

BOOL DeleteSession(SOCKET sock) {

	if (sock == INVALID_SOCKET) {
		_LOG(dfLOG_LEVEL_ERROR, L"////// DeleteSession() :: sock IS INVALID_SOCKET");
		return FALSE;
	}

	//---------------------------
	// 	   ���� ����
	//---------------------------

	// �̹� �ִ� �������� Ȯ��
	auto iter = g_sessionMap.find(sock);
	if (iter == g_sessionMap.end()) {
		// �������� �ʴ� ���� ��������
		_LOG(dfLOG_LEVEL_ERROR, L"////////DeleteSession() Session not found..");
		return FALSE;
	}

	// ������Ʈ Ǯ�� ��ȯ
	g_sessionPool.Free(iter->second);
	// �����̳ʿ��� ����
	g_sessionMap.erase(iter);

	_LOG(dfLOG_LEVEL_DEBUG, L"DeleteSession() OK..");
	return true;
}
#pragma endregion
