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
	delete test;
	return 0;
}