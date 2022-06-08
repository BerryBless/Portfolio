#include "CLogger.h"

int main() {
	int cnt = 0;
	int dcnt = 0;
	int ecnt = 0;
	int ncnt = 0;
	CLogger::Initialize();
	CLogger::_Log(dfLOG_LEVEL_DEBUG,L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::SetDirectory(L"dir");
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::SetLogLevel(dfLOG_LEVEL_NOTICE);
	CLogger::SetDirectory(L"dir/³ëÆ¼½º");
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_DEBUG, L"dfLOG_LEVEL_DEBUG %d [%d]\n", ++cnt, ++dcnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::SetDirectory(L"dir/Er");
	CLogger::SetLogLevel(dfLOG_LEVEL_ERROR);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_ERROR, L"dfLOG_LEVEL_ERROR %d [%d]\n", ++cnt, ++ecnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);

	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"dfLOG_LEVEL_NOTICE %d [%d]dfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICEdfLOG_LEVEL_NOTICE %d [%d]\n", ++cnt, ++ncnt);
	CLogger::_Log(dfLOG_LEVEL_NOTICE, L"error? %d [%d]\n", ++cnt, ++ncnt);

	return 0;
}

