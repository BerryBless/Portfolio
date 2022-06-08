#include "CObjectPool.hpp"
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include "CCrashDump.h"

#define dfTEST_THREAD 12
#define dfTEST_THREAD_POOLING_CNT 100000
#define dfTEST_MAX_POOLING_NODE (dfTEST_THREAD * dfTEST_THREAD_POOLING_CNT)

struct TEST_NODE {
	UINT64 _data;
	UINT64 _check;
};

CLF_ObjectPool<TEST_NODE> g_lfpool;

unsigned int __stdcall PoolingTestThread(LPVOID arg);
TEST_NODE *g_testnodes[dfTEST_MAX_POOLING_NODE] = {0};

int main() {
	CLogger::Initialize();
	CLogger::SetDirectory(L"LOG");

	HANDLE hThreads[dfTEST_THREAD];


	for (int i = 0; i < dfTEST_MAX_POOLING_NODE; i++) {
		g_testnodes[i] = g_lfpool.Alloc();
	}


	for (int i = 0; i < dfTEST_MAX_POOLING_NODE; i++) {
		g_lfpool.Free(g_testnodes[i]);
	}
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"Capacity[%d] UseCount[%d]\n", g_lfpool.GetCapacityCount(), g_lfpool.GetUseCount());



	for (int i = 0; i < dfTEST_THREAD; i++) {
		hThreads[i] = (HANDLE) _beginthreadex(nullptr, 0, PoolingTestThread, 0, 0, nullptr);
	}


	while (true) {
		Sleep(1000);
		CLogger::_Log(dfLOG_LEVEL_NOTICE,L"Capacity[%d] UseCount[%d]\n", g_lfpool.GetCapacityCount(), g_lfpool.GetUseCount());
	}
	return 0;
}

unsigned int __stdcall PoolingTestThread(LPVOID arg) {
	INT64 loopcnt = 0;
	DWORD ID = GetCurrentThreadId();
	DWORD tID = GetCurrentThreadId();
	TEST_NODE *testnodes[dfTEST_THREAD_POOLING_CNT] = {0};
	while (true) {
		for (int i = 0; i < dfTEST_THREAD_POOLING_CNT; i++) {
			testnodes[i] = g_lfpool.Alloc();
			testnodes[i]->_data = ID;
			testnodes[i]->_check = 10000;
		}

		for (int i = 0; i < dfTEST_THREAD_POOLING_CNT; i++) {
			if (testnodes[i]->_data != ID)
				CRASH();
			testnodes[i]->_check += ID;
		}

		for (int i = 0; i < dfTEST_THREAD_POOLING_CNT; i++) {
			if (testnodes[i]->_check != (10000 + ID))
				CRASH();
		}

		for (int i = 0; i < dfTEST_THREAD_POOLING_CNT; i++) {
			g_lfpool.Free(testnodes[i]);
		}

		int spin = 0;
		while (spin < 300) {
			Sleep(0);
			YieldProcessor();
			++spin;
		}
		if (g_lfpool.GetUseCount() == 0) {
			//printf_s("/////////USE COUNT is 0\n");
			if (g_lfpool.GetCapacityCount() != dfTEST_MAX_POOLING_NODE)
				CRASH();
		}
	}

	return 0;
}
