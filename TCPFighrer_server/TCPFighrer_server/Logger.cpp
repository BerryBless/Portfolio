#include "pch.h"
#include "Logger.h"

int g_LogLevel = dfLOG_LEVEL_WARNING;
wchar_t g_wsLogBuf[256] = {0};
char g_sFilename[256] = "_v0.3_25FPS_320320.log";
void Log(wchar_t *wsString, int iLogLevel) {

	if (g_LogLevel > iLogLevel) return;
	FILE *fp;

	// �ڿ� �߰�
	fopen_s(&fp, g_sFilename, "a");
	if (fp == NULL) {
		// ���� ����
		fopen_s(&fp, g_sFilename, "w");
	if (fp == NULL) {
		// crash
		int *p = nullptr; *p = 10;
		return;
	}
	} else {
		fseek(fp, 0, SEEK_END);
	}
	// timestemp
	time_t now = time(0);
	tm t;
	localtime_s(&t,&now);


	fwprintf_s(fp, L"[%02d:%02d:%02d] %s\n", t.tm_hour, t.tm_min, t.tm_sec,wsString);
	fwprintf_s(stdout, L"[%02d:%02d:%02d] %s\n", t.tm_hour, t.tm_min, t.tm_sec,wsString);

	fclose(fp);
}