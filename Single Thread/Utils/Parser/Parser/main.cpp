#include <stdio.h>
#include <string>
#include <locale.h>
#include "CParser.h"
int main() {
	CParser parser(L"data/map.dat");
	parser.SetNamespace(L"MAPDATA");
	int h;
	int w;
	parser.TryGetValue(L"iMapW", w);
	parser.TryGetValue(L"iMapH", h);
	WCHAR *_mapData = new WCHAR[w * h + h];
	parser.TryGetValue(L"sMap", _mapData);
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


	CParser tParser(L"data/mygame.ini");
	WCHAR str[256];
	tParser.SetNamespace(L"String");
	tParser.TryGetValue(L"sStringTest", str);
	wprintf_s(L"%s", str);
	return 0;
}