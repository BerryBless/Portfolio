#include <stdio.h>
#include <string>
#include <locale.h>
#include "CParser.h"
int main() {
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");


	int h;
	int w;
	CParser parser(L"data/map.dat");
	parser.SetNamespace(L"MAPDATA");
	parser.TryGetValue(L"iMapW", w);
	parser.TryGetValue(L"iMapH", h);

	WCHAR* _mapData = new WCHAR[w * h + h]; // w*h + null

	parser.TryGetValue(L"sMap", _mapData);
	printf_s("(h, w) = (%d, %d)\n", h, w);
	int i = 0;
	for (int x = 0; x < h; x++) {
		for (int y = 0; y < w; y++) {
			WCHAR buffer = *(_mapData + i);
			switch (buffer) {
			case L'#':
				printf_s("w");
				break;
			case L'P':
				printf_s("p");
				break;
			case L'S':
				printf_s("s");
				break;
			case L'T':
				printf_s("t");
				break;
			case L'_':
				printf_s(" ");
				break;
			default:
				printf_s("E");
				break;
			}
			++i;
		}
		printf_s("\n");
		++i;
	}


	bool bUseKinect = false;
	float fCloudNearFadeDistance;
	WCHAR str[256];
	CParser tParser(L"data/mygame.ini");
	if (tParser.SetNamespace(L"Clouds")) {
		if (tParser.TryGetValue(L"fCloudNearFadeDistance", fCloudNearFadeDistance)) {
			printf_s("%f\n", fCloudNearFadeDistance);
		}
		else {
			printf_s("not find fCloudNearFadeDistance\n");
		}


		if (tParser.TryGetValue(L"bUseKinect", bUseKinect)) {
			if (bUseKinect) {
				printf_s("bUseKinect is true\n");
			}
			else {
				printf_s("bUseKinect is false\n");
			}
		}
		else {
			printf_s("not find bUseKinect\n");
		}
	}
	else {
		printf_s("not find SetNamespace\n");
	}

	if (tParser.SetNamespace(L"Controls")) {
		if (tParser.TryGetValue(L"fCloudNearFadeDistance", fCloudNearFadeDistance)) {
			printf_s("%f\n", fCloudNearFadeDistance);
		}
		else {
			printf_s("not find fCloudNearFadeDistance\n");
		}


		if (tParser.TryGetValue(L"bUseKinect", bUseKinect)) {
			if (bUseKinect) {
				printf_s("bUseKinect is true\n");
			}
			else {
				printf_s("bUseKinect is false\n");
			}
		}
		else {
			printf_s("not find bUseKinect\n");
		}
	}
	else {
		printf_s("not find Controls\n");
	}
	return 0;
}