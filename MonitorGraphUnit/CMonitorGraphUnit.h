/*
����͸� ���α׷��� �ڽ������� Ŭ����

- �� Ŭ���� ���ο��� �ڽ� �������� ����, �ڽ� �������� ���ν���, ������ ���� ��� ������.
*/
#pragma once
#include "framework.h"
#include "CList.h"
#define dfMAXCHILD		100	// �ִ� �ڽļ�

#define dfFONT_TITLE_SIZE 15	// Ÿ��Ʋ ��Ʈ ������
#define dfFONT_TITLE_WIDTH 100	// Ÿ��Ʋ ��Ʈ �ʺ�
#define dfFONT_POINT_SIZE 15	// �׸��� ���� ��Ʈ ������
#define dfTITLE_RECT_HEIGHT 20 // Ÿ��Ʋ�� ����
#define dfALERT_TIME_MS	100		// ��� Ÿ�̸� �ð�

#define dfDATA_CPU_USED 10;
#define dfDATA_CPU_NETWORK_SEND 21;
#define dfDATA_CPU_NETWORK_RECV 22;

struct stDataQueue {
	int iID;
	int iDataType;
	CList<int> iQueue;
};


class CMonitorGraphUnit {
public:

	enum class TYPE {
		LINE_SINGLE,
		LINE_MULTI,
		NUMBER,
		ONOFF,
		PIE
	};


public:
	// �����ӟp

	// ���� �Ҹ�
	CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, TYPE enType, const WCHAR *szTitle, int iPosX, int iPosY, int iWidth, int iHeight, int iDataMax, int iDataAlert);
	~CMonitorGraphUnit();

	/////////////////////////////////////////////////////////
	// ������ ���ν���
	/////////////////////////////////////////////////////////
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/////////////////////////////////////////////////////////
	// ������ �ֱ�.
	/////////////////////////////////////////////////////////
	BOOL	InsertData(int iData);

	// ȭ�� ����
	void CreateMemDC();	// �׸� �׸� ���� ����

	// ���ۿ� �׸� �׸��� �ڵ�
	virtual void Paint_LineSingle();	// ������ �׷���
	//***
	// ���⿡ ���ϴ� �׷��� �߰� �����ڵ� �׶��׶� ¥��
	//***
	virtual void Paint_Title(); // Ÿ��Ʋ��
	virtual void Paint_Grid();	// ����

	void FlipMemDC();	// ���۱׸��� ����Ϳ� ���
	void DestroyMemDC();// ���ۻ���

	// ����
	int GetDataYPos(int data); // ���̷� y������ �ؿ��� ���� ���ϱ�
	void ChangeBackgroundBrush();

protected:

	//------------------------------------------------------
	// ������ �ڵ�, this ������ ��Ī ���̺� ����.
	//------------------------------------------------------
	BOOL				PutThis(void);
	static CMonitorGraphUnit *GetThis(HWND hWnd);


private:

	//------------------------------------------------------
	// �θ� ������ �ڵ�, �� ������ �ڵ�, �ν��Ͻ� �ڵ�
	//------------------------------------------------------
	HWND		_hWndParent;
	HWND		_hWnd;
	HINSTANCE	_hInstance;

	//------------------------------------------------------
	// ������ ��ġ,ũ��,����, �׷��� Ÿ�� ��.. �ڷ�
	//------------------------------------------------------
	// ������ Ÿ��
	TYPE		_enGraphType;

	// ����
	COLORREF	_BackColor;
	COLORREF _TitleColor;
	COLORREF _UIColor;

	// ����������
	int		_iWindowPosX;
	int		_iWindowPosY;
	int		_iWindowWidth;
	int		_iWindowHeight;

	//------------------------------------------------------
	// ���� ���۸��� �޸� DC, �޸� ��Ʈ��
	//------------------------------------------------------
	HDC		_hMemDC;
	HBITMAP	_hMemBitmap;
	HBITMAP _hMemBitmapOld;

	//------------------------------------------------------
	// ���ҽ�
	//------------------------------------------------------
	// ��Ʈ
	HFONT _hFontTitle;
	HFONT _hFontGrid;

	// �귯��
	HBRUSH _hBackBrush;
	HBRUSH _hBackBrush_Title;

	// ��
	HPEN _hLinePen;
	HPEN _hGridPen;

	//------------------------------------------------------
	// ������
	//------------------------------------------------------
	//������ ť / ����Ʈ
	CList<int> _dataList;

	static int _childCnt;// �ڽ��� ��ִ��� ī����

	int _iDataMax; // ������ �ִ� ũ��
	int		_iDataAlert;	// �溸 �︮�� ���ذ�

	WCHAR _szTitle[30];// Ÿ��Ʋ �̸�

		//------------------------------------------------------
	// ����� - 
	//
	// ������ ����ġ ���޽� ������� �Բ� �ڽ��� ������ ��� �����쿡 UM_ALERT �޽����� ������.
	//
	// _bAlertMode = TRUE �� �� �� Ÿ�̸Ӹ� ����Ͽ� �����ð� �� ����.
	//
	//  ��� ��Ȳ�� ����  �ɹ� ���� �÷��׷� ��� �ִ� ������ ... ?  
	//  �ʿ�ġ �ʴٸ� �ȸ���ŵ� �Ǹ�,  �����ϴ� ���� �ʿ��ϰ� �� ���Դϴ�.  
	//------------------------------------------------------
	BOOL		_bAlertMode;
};

/*
///////////////////////////////////////////////////////////


* �� Ŭ������ �ܺ� ����

CMonitorGraphUnit *p1;
CMonitorGraphUnit *p2;
CMonitorGraphUnit *p3;
CMonitorGraphUnit *p4;

- �ӽ÷� ������ ���� ����. ����


case WM_CREATE:
  p1 = new CMonitorGraphUnit(g_hInst, hWnd, CMonitorGraphUnit::TYPE::LINE_SINGLE, 10, 10, 200, 200);
  p2 = new CMonitorGraphUnit(g_hInst, hWnd, CMonitorGraphUnit::TYPE::LINE_SINGLE, 220, 10, 200, 200);
  p3 = new CMonitorGraphUnit(g_hInst, hWnd, CMonitorGraphUnit::TYPE::LINE_SINGLE, 430, 10, 400, 200);
  p4 = new CMonitorGraphUnit(g_hInst, hWnd, CMonitorGraphUnit::TYPE::LINE_SINGLE, 10, 220, 300, 250);
  SetTimer(hWnd, 1, 100, NULL);

- �θ� �޵��� WM_CREATE ���� �ڽ� ������ Ŭ������ ����.
- Ÿ�̸ӵ� ����.


case WM_TIMER:
	p1->InsertData(rand() % 100);
	p2->InsertData(rand() % 100);
	p3->InsertData(rand() % 100);
	p4->InsertData(rand() % 100);
	break;

- Ÿ�̸Ӵ� �����ϰ� �����͸� �����Ͽ� ��� ������� ����.




* �ڽ� ������� �����͸� ���� �� �ڽ��� InvalidateRect �� ȣ���ϰ�
* �ڽ� ������ Ŭ���� ������ ������ ���ν��� WM_PAINT ���� �׷����� �׸���.



* �ڽ� ������ �����ϴ� �ڵ�

// �θ� ������
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

// �ڽ�������
_hWnd = CreateWindow(_szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
						_iWindowPosX, _iWindowPosY, _iWindowWidth, _iWindowHeight, _hWndParent, NULL, _hInstance, NULL);


*/