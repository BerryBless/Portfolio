#include "CMonitorGraphUnit.h"
#include "MonitorGraphUnit.h"
#include "framework.h"


// TEMP
int CMonitorGraphUnit::_childCnt = 0;

#pragma region static method


LRESULT CALLBACK  CMonitorGraphUnit::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CMonitorGraphUnit *pThis = CMonitorGraphUnit::GetThis(hWnd);
	switch (message) {
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		// ȭ�� �����ҋ�
		if (pThis != NULL) {
			switch (pThis->_enGraphType) {
			case CMonitorGraphUnit::TYPE::LINE_SINGLE:
				pThis->Paint_LineSingle();
				break;
			}
			pThis->FlipMemDC();
		}
		break;
	case WM_SIZE:
		// ������ ũ�� �ٲ��
		if (pThis != NULL) {
			pThis->DestroyMemDC();
			pThis->CreateMemDC();
		}
		break;
	case WM_TIMER:
		pThis->ChangeBackgroundBrush();
		KillTimer(hWnd, 1);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return NULL;
}
//------------------------------------------------------
// ������ �ڵ�, this ������ ��Ī ���̺� ����.
//------------------------------------------------------
CMonitorGraphUnit *CMonitorGraphUnit::GetThis(HWND hWnd) {
	return (CMonitorGraphUnit *) GetWindowLongPtr(hWnd, 0);
}

#pragma endregion

CMonitorGraphUnit::CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, TYPE enType, const WCHAR *szTitle, int iPosX, int iPosY, int iWidth, int iHeight, int iDataMax, int iDataAlert) {
	// Ŭ���� �̸� ����
	wsprintf(_szTitle, L"%s", szTitle);

	// �ڽ�Ŭ���� WndProc���
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CMonitorGraphUnit::WndProc;
	wcex.cbClsExtra = 8;// Ŭ���� ������ ����Ʈ Ȯ��, Ŭ���� ���� ��������?
	wcex.cbWndExtra = 8;// ������ ������ ����Ʈ Ȯ��
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MONITORGRAPHUNIT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_MONITORGRAPHUNIT);
	wcex.lpszClassName = _szTitle;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);


	// �ڽ�Ŭ���� �ν��Ͻ� ����
	_hWnd = CreateWindow(_szTitle, NULL, WS_CHILD | WS_VISIBLE,
		iPosX, iPosY, iWidth, iHeight,
		hWndParent, NULL, hInstance, NULL);

	// �ν��Ͻ� ��������
	if (!_hWnd) {
		// TODO ����ó��
		int *p = nullptr;
		*p = 10;
	}
	// �θ� �ڵ� ����
	_hWndParent = hWndParent;
	_hInstance = hInstance;

	// Ÿ������
	_enGraphType = enType;

	// This������ ����
	PutThis();

	// ������ �ִ밪 ����
	_iDataMax = iDataMax;
	_iDataAlert = iDataAlert;
#pragma region Create Resource
	//------------------------------------------------------
	// ������۸� ���� ����
	//------------------------------------------------------
	CreateMemDC();

	//------------------------------------------------------
	// Font ����.
	//------------------------------------------------------
	_hFontTitle = CreateFont(dfFONT_TITLE_SIZE, 0, 0, 0, dfFONT_TITLE_WIDTH, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"���� ���");
	_hFontGrid = CreateFont(dfFONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"���� ���");

	//------------------------------------------------------
	// �� �귯�� ����  (���� ä����)
	//------------------------------------------------------
	 _BackColor = RGB(25, 25, 25);
	 _TitleColor = RGB(100, 100, 100);
	 _UIColor = RGB(0, 255, 0);

	_hBackBrush = CreateSolidBrush(_BackColor);
	_hBackBrush_Title = CreateSolidBrush(_TitleColor);


	// �� �ڽ� �������� ������  1���� ���� �ϼŵ� �˴ϴ�.
//  ���� ó������ RGB ���� ������ ���ڷ� �޾Ƽ� ��� �Ͽ�����  ����� �ʹ� �����Ͽ�����
//  �׸��� 4���� ������ ������ ���������� ���� �ֽ��ϴ�. 
// ������ �Ẹ�� �� ���� ���� �Ի� ������ �û� ����ϰ� �˴ϴ�.

	//------------------------------------------------------
	// �� ����
	//------------------------------------------------------
	BYTE byR = min(255, GetRValue(_UIColor) + 200);
	BYTE byG = min(255, GetGValue(_UIColor) + 200);
	BYTE byB = min(255, GetBValue(_UIColor) + 200);

	_hLinePen = CreatePen(PS_SOLID, 2, RGB(byR, byG, byB));

	// �� ȭ�麸��  ���� ���� ���� ǥ���ϱ� ����  RGB �� ��ҿ� + 200 �� ��, ���� ����� ��������

	byR = min(255, GetRValue(_UIColor) + 50);
	byG = min(255, GetGValue(_UIColor) + 50);
	byB = min(255, GetBValue(_UIColor) + 50);

	_hGridPen = CreatePen(PS_SOLID, 1, RGB(byR, byG, byB));

#pragma endregion

	++CMonitorGraphUnit::_childCnt;
}
CMonitorGraphUnit::~CMonitorGraphUnit() {
	DestroyMemDC();
}
//------------------------------------------------------
// ������ �ڵ�, this ������ ��Ī ���̺� ����.
//------------------------------------------------------
BOOL CMonitorGraphUnit::PutThis(void) {
	LONG_PTR p;
	p = SetWindowLongPtr(_hWnd, 0, (LONG_PTR) this);
	return p != NULL;
}

// ---------
// �����ͳֱ�
// ---------
BOOL CMonitorGraphUnit::InsertData(int iData) {
	// â ũ�⿡ ���� �������� �ִ� ����
	// �ʺ��� 1/4 �� �ְ�, x�� ������ 3
	int max_X = (int) ((float) (_iWindowWidth) * (1.0f / 4.0f));

	// ������ �ֱ�
	if (_dataList.size() > max_X) {
		_dataList.pop_front();
	}
	_dataList.push_back(iData);

	if (_iDataAlert != 0 && _iDataAlert <= iData) {
		//  ��� 1ȸ !
		PlaySound((LPCTSTR) SND_ALIAS_SYSTEMHAND, NULL, SND_ALIAS_ID | SND_ASYNC); // �Ҹ� ���
		DeleteObject(_hBackBrush);
		_hBackBrush = CreateSolidBrush(RGB(200,0,0));
		SetTimer(_hWnd, 1, dfALERT_TIME_MS, NULL);
	}

	// ȭ�鰻��
	InvalidateRect(_hWnd, NULL, FALSE);
	return true;
}


#pragma region PaintMethod

void CMonitorGraphUnit::Paint_LineSingle() {
	//�׸� �׸���
	// �׸���������
	RECT rect;
	int dataX = 0; // �����͸� �׸� x��ǥ(�ð�)
	HPEN	hOldPen = (HPEN) SelectObject(_hMemDC, _hLinePen);

	GetClientRect(_hWnd, &rect);                                 // ������ ũ�� ������
	HBRUSH hOldBrush = (HBRUSH) SelectObject(_hMemDC, _hBackBrush);
	PatBlt(_hMemDC, 0, 0, rect.right, rect.bottom, PATCOPY);  // ���� ������� (�귯���� �簢������ ����)
	SelectObject(_hMemDC, hOldBrush);
	// ---------------
	// �׸� �׸���
	// 1. ť���� ������ �̾� ���߱�
	// 2. Ÿ��Ʋ�� ���
	// 3. �׸��� ���
	// ---------------

	// 1. ť���� ������ �̾� ���߱�
	auto iter = _dataList.begin();
	MoveToEx(_hMemDC, dataX, GetDataYPos(*iter), NULL);
	while (iter != _dataList.end()) {

		LineTo(_hMemDC, dataX, GetDataYPos(*iter));
		dataX += 3;
		++iter;
	}

	SelectObject(_hMemDC, hOldPen);


	// 2. Ÿ��Ʋ�� ���
	Paint_Title();
	// 3. �׸��� ���
	Paint_Grid();

}
#pragma region UI
void CMonitorGraphUnit::Paint_Title() {
	BYTE byR = min(255, GetRValue(_BackColor) + 170);
	BYTE byG = min(255, GetGValue(_BackColor) + 170);
	BYTE byB = min(255, GetBValue(_BackColor) + 170);

	SetTextColor(_hMemDC, RGB(byR, byG, byB));

	// �̴� DC �� �ؽ�Ʈ ��½� ������ ������ �����մϴ�.
	// �� ���󺸴� ���� ������ ���� �߽��ϴ�.


	HPEN	hPen = CreatePen(PS_NULL, 0, 0);
	HPEN	hOldPen = (HPEN) SelectObject(_hMemDC, hPen);

	//..������ �η��õ� SelectObject �ؾ� �� ..
	HBRUSH hOldBrush = (HBRUSH) SelectObject(_hMemDC, _hBackBrush_Title);

	RECT rect;
	GetClientRect(_hWnd, &rect);                                 // ������ ũ�� ������


	Rectangle(_hMemDC, 0, 0, _iWindowWidth + 1, dfTITLE_RECT_HEIGHT);

	// Ractangle �簢�� �׸����  ��� �κп� 30pixel ���̷� �簢���� �׸��ϴ�.
	// Ractangle ��½� �ܰ����� Pen, ���δ� Brush �� ä�����ϴ�.

	//  �ܰ����� ���ֱ� ���ؼ� CreatePen(PS_NULL  �� Null ���� ���� ���� �Ͽ����ϴ�.
	//  �� nulll �浵 �ϳ� ����� �ΰ� ���� �ϼŵ� �˴ϴ�



	SelectObject(_hMemDC, hOldPen);
	SelectObject(_hMemDC, hOldBrush);
	DeleteObject(hPen);


	HFONT hOldFont = (HFONT) SelectObject(_hMemDC, _hFontTitle);
	TextOut(_hMemDC, 7, 3, _szTitle, (int) wcslen(_szTitle));
	SelectObject(_hMemDC, hOldFont);

}
void CMonitorGraphUnit::Paint_Grid() {
	HPEN	hOldPen = (HPEN) SelectObject(_hMemDC, _hGridPen);

	int iDataInterval = _iDataMax / 4;		// Max �����͸� �������� 4��� �غ��ҽ��ϴ�.  
						// Max �����͸� �������� 4����� �ƴ�,  �׳� ������ ũ�� �������� 4����� �ϸ� �ȵǴ��� ? ��� �Ͻ� �� ������
						//  4��� �׸��� ����  ���� ������ ��ġ�� ǥ�⸦ ���־�� �ϹǷ�  ���� �����Ͱ� �������� ���� ���Դϴ�.


	int iY = _iWindowPosY + dfTITLE_RECT_HEIGHT;


	int iIntervalY = (_iWindowHeight - dfTITLE_RECT_HEIGHT) / 4;	// ���� ȭ�� ������ ũ����� 4���


	HFONT hOldFont = (HFONT) SelectObject(_hMemDC, _hFontGrid);

	WCHAR szPoint[10];
	wsprintf(szPoint, L"%d", _iDataMax);
	//----------------------------------------------------
	// �� ��ġ�� ��ġ ���
	//----------------------------------------------------

	TextOut(_hMemDC, _iWindowPosX + 1, iY + 1, szPoint, (int) wcslen(szPoint));	// ���� ��ܺ��� ��ġ�� ���ϴ�.  ��ġ�� szPoint �� ������ �ֽ��ϴ�.  

	for (int iCnt = 3; iCnt > 0; --iCnt) {
		iY += iIntervalY;

		//----------------------------------------------------
		// ������ �� �߱�
		//----------------------------------------------------
		MoveToEx(_hMemDC, _iWindowPosX + 1, iY + 1, NULL);
		LineTo(_hMemDC, _iWindowPosX + _iWindowWidth - 1, iY + 1);

		//----------------------------------------------------
		// �� ��ġ�� ��ġ ���
		//----------------------------------------------------
		wsprintf(szPoint, L"%d", _iDataMax / (4) * (iCnt));
		TextOut(_hMemDC, _iWindowPosX + 1, iY + 1, szPoint, (int) wcslen(szPoint));
	}

	SelectObject(_hMemDC, hOldFont);
	SelectObject(_hMemDC, hOldPen);
}
#pragma endregion

int CMonitorGraphUnit::GetDataYPos(int data) {
	// �׷����� �뷫 ǥ���ϱ�
	// min( bottom-1 ,  bottom - (Height * (data/dataMax) )
	return min((_iWindowHeight + _iWindowPosY - 1), (_iWindowHeight + _iWindowPosY) -
		(int) ((double) (_iWindowHeight - dfTITLE_RECT_HEIGHT) * ((double) data / (double) _iDataMax)));
}

// 
void CMonitorGraphUnit::ChangeBackgroundBrush() {
	DeleteObject(_hBackBrush);
	_hBackBrush = CreateSolidBrush(_BackColor);
}



#pragma endregion










#pragma region MemDC
void CMonitorGraphUnit::CreateMemDC() {
	//-------------------------------------
	// �޸�DC ���� �κ�
	//-------------------------------------
	// ȭ�� ũ�� ���ϱ�
	RECT Rect;
	GetClientRect(_hWnd, &Rect);

	// ����� ����
	HDC hdc = GetDC(_hWnd);
	

	_hMemDC = CreateCompatibleDC(hdc);
	_hMemBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom);
	_hMemBitmapOld = (HBITMAP) SelectObject(_hMemDC, _hMemBitmap);
	ReleaseDC(_hWnd, hdc);

	PatBlt(_hMemDC, 0, 0, Rect.right, Rect.bottom, BLACKNESS);

	SetBkMode(_hMemDC, TRANSPARENT); // ���� �� ��� �����ϰ�
	SetROP2(_hMemDC, R2_XORPEN);	// �׷��� �׸���� xor
	// TODO ��Ʈ xor
	// ������ ũ������ ����
	_iWindowPosX = Rect.left;
	_iWindowPosY = Rect.top;
	_iWindowWidth = Rect.right - Rect.left;
	_iWindowHeight = Rect.bottom - Rect.top;
}

void CMonitorGraphUnit::FlipMemDC() {
	// ���� �����ϱ�
	PAINTSTRUCT  ps;
	RECT rect;
	HDC hdc;

	// �ø�
	GetClientRect(_hWnd, &rect);
	hdc = BeginPaint(_hWnd, &ps);
	
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, _hMemDC, 0, 0, SRCCOPY );


	EndPaint(_hWnd, &ps);
}

void CMonitorGraphUnit::DestroyMemDC() {
	//-------------------------------------
	// �޸�DC �ı� �κ�
	//-------------------------------------
	if (_hMemBitmapOld != NULL)
		SelectObject(_hMemDC, _hMemBitmapOld);
	DeleteObject(_hMemBitmap);
	DeleteObject(_hMemDC);
}
#pragma endregion


