#include <iostream>
#include "MemProfiler.h"

class CTest {
private:
	int arr[4];
public:
	CTest() {
		printf_s("Constructor\n");
		for (int i = 0; i < 4; i++) {
			arr[i] = 0x1234578;
		}
	}
	/*~CTest() {
		printf_s("Destructor\n");
		for (int i = 0; i < 4; i++) {
			arr[i] = 0xaaaabbbb;
		}
	}*/
};

int main() {
	CTest* test = new CTest[3];
	CTest* test1 = new CTest;
	CTest* test2 = new CTest;
	CTest* test3 = new CTest;
	CTest* test4 = new CTest[3];
	CTest* test5 = new CTest;
	CTest* test6 = new CTest[3];
	CTest* test7 = new CTest;
	delete[] test2;
	delete test3;
	delete[] test4;
	delete test5;
	delete[] test6;
	return 0;
}