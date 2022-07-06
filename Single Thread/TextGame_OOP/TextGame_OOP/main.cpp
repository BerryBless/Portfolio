#include "pch.h"
#include "CFramework.h"

int main() {
	CFramework *framework = I_FRAMEWORK;
	time_t tFbefore = 0;	// fps 타이머 과거값
	time_t tFafter;			// fps 타이머 현재값
	time_t tDelay;
	framework->Init();

	while (true) {
		framework->Update();
		if (framework->InGame() == false) {
			break;
		}
		framework->Render();

		// fps 맞추기
		tFafter = clock();
		tDelay = FPS_DELAY - (tFafter - tFbefore);
		if(tDelay > 0)
			Sleep(tDelay);
		tFbefore = tFafter;
	}

	return 0;
}