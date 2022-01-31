/*
* �̱۽����� ����!
* ��Ƽ������ ������ TagNameã���� ������ �Ͼ�ϴ�!!
* ����

* �ð�����
void f2() {
	PRO_START(L"F2");	// �����Ұ��� PRO_START(TagName) �ֱ�
	Sleep(80);
}
void f3() {
	PRO_BEGIN(L"F3");	// PRO_BEGIN(TagName)
	Sleep(1000);
	PRO_END(L"F3");		// PRO_END(TagName)
}

* ���
	PRO_PRINT((WCHAR *)L"log.log"); // �α����Ϲ��ڿ�(WCHAR *) �����̸� 

* �ϰ���
*	TagName�� ������ TagName������ �ð�, ���� Ƚ���� �����մϴ�.
*/
#pragma once
#include <stdio.h>
#include <Windows.h>

#define CRASH() do{int* p=nullptr; *p=0; }while(0)

#define dfPROFILER // �������ϸ��� ����ʹٸ� �ּ�ó��

#ifdef dfPROFILER

// ================================
// ��Ƽ����Ʈ ���ڿ� ó�� �Լ�
// �ʿ��Ѱ͸� ����
// Strlen : ���ڿ� ����
// Strcpy : ���ڿ� ����
// Strcmp : ���ڿ� �� (������ return 0)
// ================================

int Strlen(const WCHAR *str);
void Strcpy(WCHAR *strDest, const WCHAR *strSrc);
int Strcmp(const WCHAR *str1, const WCHAR *str2);


#define TICK_TO_MICSEC 1000000	// QueryPerformanceFrequency tick�� ����ũ���� ������ �ٲߴϴ�
#define dfPROFILECOUNT 10		// �������ϸ��� ������ �ִ� ����

// ================================
// ��ũ���Լ��� ���
// �������ϸ��� ���� �������� #define dfPROFILER �ּ�ó�� �ϱ�
// ================================
#define PRO_START(TagName)	cProfiler profile (TagName)		// Ŭ������ ���� / �Ҹ��� �̿��� �������ϸ� (����)
#define PRO_BEGIN(TagName)	ProfileBegin(TagName)			// �������ϸ��� ������ ���� TagName�� �����մϴ�.
#define PRO_END(TagName)	ProfileEnd(TagName)				// �������ϸ��� ���� ���� TagName�� �����մϴ�.
#define PRO_PRINT(LogFile)	ProfileDataOutText(LogFile)		// �������ϸ� �����͸� LogFile�� ����մϴ�.


// ================================
// �������ϸ� ���� ����ü
// ================================
struct PROFILE_SAMPLE {
	WCHAR			Name[64];			// �������� ���� �̸�.

	LARGE_INTEGER	StartTime;			// �������� ���� ���� �ð�.

	__int64			TotalTime;			// ��ü ���ð� ī���� Time.	(��½� ȣ��ȸ���� ������ ��� ����)
	__int64			Min;				// �ּ� ���ð� ī���� Time.	
	__int64			Max;				// �ִ� ���ð� ī���� Time.

	__int64			Call;				// ���� ȣ�� Ƚ��.

} ;

// ================================
// ProfileBegin : name�� ���۽ð��� üũ�մϴ�
// ProfileEnd : name�� ���ð��� üũ�ϰ�, �ɸ��ð��� ����Ͽ� ��踦 ���ϴ�.
// ProfileDataOutText : fileName���� ��������� �մϴ�.
// ProfileReset : �������ϸ������͸� �ʱ�ȭ �մϴ�.
// ================================

void ProfileBegin(const WCHAR* name);
void ProfileEnd(const WCHAR* name);
void ProfileDataOutText(WCHAR* fileName);
void ProfileReset(void);
//* ������, �Ҹ��ڸ� �̿��ؼ� �ڵ����� ���� (����)
class cProfiler {
private:
	const WCHAR* tag;
public:
	cProfiler(const WCHAR* tagName);
	~cProfiler();
	
};
//*/
#else
// ================================
// �������ϸ� �۵� ����
// ================================
#define PROFILER(TagName)	// �������ϸ� ����, �������ϸ��� Ű��ʹٸ� dfPROFILER�� �����ϼ���!
#define PRO_BEGIN(TagName)	// �������ϸ� ����, �������ϸ��� Ű��ʹٸ� dfPROFILER�� �����ϼ���!
#define PRO_END(TagName)	// �������ϸ� ����, �������ϸ��� Ű��ʹٸ� dfPROFILER�� �����ϼ���!
#define PRO_PRINT(LogFile)	// �������ϸ� ����, �������ϸ��� Ű��ʹٸ� dfPROFILER�� �����ϼ���!
#endif
