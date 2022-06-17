#include "Profiler.h"
#include <conio.h>

//*
void f1() {
	PRO_BEGIN(L"F1");
	Sleep(1);
	PRO_END(L"F1");
}
void f2() {
	PRO_BEGIN(L"F2");
	int sum = 0;
	for (int i = 1; i <= 100000; i++) {
		sum += i;
	}
	PRO_END(L"F2");
}
void f3() {
	PRO_BEGIN(L"F3");
	Sleep(1000);
	PRO_END(L"F3");
}
//*/
int main() {
	while (true) {
		f1();
		f2();
		f3();
		if (_kbhit()) {
			char key = _getch();
			PRO_PRINT((WCHAR*)L"log.log");
		}
	}


	return 0;
}