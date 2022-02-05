#pragma once
#include <new.h>
#include <stdlib.h>
/*---------------------------------------------------------------

	procademy MemoryPool.

	�޸� Ǯ Ŭ���� (������Ʈ Ǯ / ��������Ʈ)
	Ư�� ����Ÿ(����ü,Ŭ����,����)�� ������ �Ҵ� �� ��������.

	- ����.

	procademy::CObjectPool<DATA> MemPool(300, FALSE);
	DATA *pData = MemPool.Alloc();

	pData ���

	MemPool.Free(pData);


----------------------------------------------------------------*/

// �Ϻη� ũ��������
#define dfCRASH() do{int *p = nullptr; *p = 10;}while(0)

template <class DATA>
class CObjectPool {
	//////////////////////////////////////////////////////////////////////////
	// üũ��
	// ���������� ����Ű�� �������� �� 21��Ʈ�� �Ⱦ��ٴ°��� ����
	// üũ���� ���ʵ�����(������)�� this�� ������
	//////////////////////////////////////////////////////////////////////////
	enum : unsigned long long {
		//USEDMASK	= 0x80000000'00000000,// �� ������Ʈ�� �޸� Ǯ �ȿ� �ִ°�?
		POINTERMASK = 0x0000FFFF'FFFFFFFF,// this�����͸� �����Ұ�!
	};
private:
	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ��ȯ�� (�̻��) ������Ʈ ���� ����.
	//
	// ----------------- <- _pFreeNode�� �ǹ��ϴ� �ּ�
	// | st_BLOCK_NODE |
	// ----------------- <- Alloc() �� �����ϴ� �ּ�
	// |               |	�� �κ��� �׳� �ּҷ� ����
	// | <class DATA>  |	��忡 �ִ°��� �ƴ�,
	// |               |	��� �����Ϳ� �������� ���ϸ� ������ �ּ�
	// |               |
	// |               |
	// -----------------
	// 
	// üũ���� ��1����Ʈ�� �����Ͱ� 
	// - USEDMASK�� -> �� ������Ʈ�� ������Ʈ Ǯ �ȿ� ���� 
	// - 00 �̴� -> �����
	// - XOR(^)�������� ����ϸ鼭 ���
	//////////////////////////////////////////////////////////////////////////
	struct st_BLOCK_NODE {
		alignas(8) unsigned long long	CHECKSUM;					// üũ��
		alignas(8) st_BLOCK_NODE		*pNEXT;						// �������
	};
private:
	st_BLOCK_NODE	*_pFreeNode;								// �޸�Ǯ ������ top
	int				_iCapacity;									// ���� Ȯ���� ����
	int				_iUseCount;									// ���� ������� ����
	int				_iBlockSize;								// DATA + NODE�� ������
	bool			_bPlacementNew;								// ������ ȣ�� ���� (true :ȣ��)
public:

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:	(int) �ʱ� �� ����.
	//				(bool) Alloc �� ������ / Free �� �ı��� ȣ�� ����
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CObjectPool(int iBlockNum, bool bPlacementNew = false);

	virtual	~CObjectPool();


	//////////////////////////////////////////////////////////////////////////
	// �� �ϳ��� �Ҵ�޴´�.  
	//
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	//////////////////////////////////////////////////////////////////////////
	DATA *Alloc(void);

	//////////////////////////////////////////////////////////////////////////
	// ������̴� ���� �����Ѵ�.
	//
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(DATA *pData);

	//////////////////////////////////////////////////////////////////////////
	// ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	//
	// Parameters: ����.
	// Return: (int) �޸� Ǯ ���� ��ü ����
	//////////////////////////////////////////////////////////////////////////
	int		GetCapacityCount(void) { return _iCapacity; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� �� ������ ��´�.
	//
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseCount(void) { return _iCapacity; }
};

template<class DATA>
inline CObjectPool<DATA>::CObjectPool(int iBlockNum, bool bPlacementNew) {
	// �ʱⰪ �ֱ�
	_iCapacity = iBlockNum;									// Ȯ���� ����
	_iUseCount = 0;											// ������� ��ϼ�
	_bPlacementNew = bPlacementNew;							// �����ڸ� ȣ���ϳ�?
	_iBlockSize = sizeof(DATA) + sizeof(st_BLOCK_NODE);		// �� ������ ���
	// �޸� Ȯ�� �õ�
	_pFreeNode = nullptr;
	for (int i = 0; i < iBlockNum; i++) {
		// ��� �Ѱ�
		st_BLOCK_NODE *pNode = (st_BLOCK_NODE *) malloc(_iBlockSize);
		pNode->pNEXT = _pFreeNode;									// ���� top�� ����
		pNode->CHECKSUM = (unsigned long long)this;
		//pNode->CHECKSUM ^= USEDMASK;								// �Ҵ� �Ǿ����� �ʴ�!
		_pFreeNode = pNode;									// top�� �Ǿ��� �������!
		if (_bPlacementNew == true) {
			// placement new
			new (pNode + 1) DATA();
		}
	}
}

template<class DATA>
inline CObjectPool<DATA>::~CObjectPool() {
	//  �����ϸ� ���� free���ֱ�
	st_BLOCK_NODE *pNODE;
	// ��� �ϳ��ϳ� ����
	while (_pFreeNode != nullptr) {
		pNODE = _pFreeNode;
		_pFreeNode = _pFreeNode->pNEXT;
		free(pNODE);
	}
}

template<class DATA>
inline DATA *CObjectPool<DATA>::Alloc(void) {
	// �������� ������ ������
	if (_pFreeNode == nullptr) {							// ������ �����Ҷ�
		//////////////////////////////////////////////////////////////////////////
		// �� ������ ����� ���� _pFreeNode�� ������ �ȴ�
		// ù��° ����� ���ο� _pFreeNode�� �ȴ�
		// 
		// -----------------  --->  -----------------  --->  -----------------  ---> .... 
		// | st_BLOCK_NODE |        | st_BLOCK_NODE |        | st_BLOCK_NODE |
		// -----------------        -----------------        ----------------- 
		// |               |        |               |        |               |
		// | <class DATA>  |        | <class DATA>  |        | <class DATA>  |
		// |               |        |               |        |               |
		// |               |        |               |        |               |
		// |               |        |               |        |               |
		// -----------------        -----------------        ----------------- 
		//
		//////////////////////////////////////////////////////////////////////////
		// ��� �Ѱ�
		st_BLOCK_NODE *pNode = (st_BLOCK_NODE *) malloc(_iBlockSize);
		pNode->pNEXT = _pFreeNode;									// ���� top�� ����
		pNode->CHECKSUM = (unsigned long long)this;
		//pNode->CHECKSUM ^= USEDMASK;								// �Ҵ� �Ǿ����� �ʴ�!
		_pFreeNode = pNode;											// top�� �Ǿ��� �������!
		if (_bPlacementNew == true) {
			// placement new
			new (pNode + 1) DATA();
		}
		++_iCapacity;
	}

	// ������ Pop()	
	DATA *pRet = (DATA *) (_pFreeNode + 1);						// FreeNode �ִ°� �Ҵ����ֱ� 
	// ���ÿ��� �����ٸ� ǥ��
	//_pFreeNode->CHECKSUM ^= USEDMASK;							// // �Ҵ� ������ ������ ���(OFF)
	_pFreeNode = _pFreeNode->pNEXT;								// ���� ž ����

	++_iUseCount;												// �ϳ� ��

	// ������ ȣ��
	if (_bPlacementNew == true) {
		// placement new
		new (pRet) DATA();
	}


	return pRet;
}

template<class DATA>
inline bool CObjectPool<DATA>::Free(DATA *pData) {
	if (pData == nullptr) return false;


	// ������� Ȯ��
	// �� Ǯ�� ������ üũ�ؼ� ������ "�� Ǯ�� �޸�"�� �´��� Ȯ��
	st_BLOCK_NODE *pNode = ((st_BLOCK_NODE *) pData - 1);			// ��� �����ŭ ������!

	//if (pNode->CHECKSUM & USEDMASK) {
	//	// �̹� �Ҵ� ������ ������
	//	dfCRASH();
	//	return false;
	//}
	if ((pNode->CHECKSUM & POINTERMASK) != (unsigned long long) this) {
		// �ٸ� �޸�Ǯ���� ���� ������
		dfCRASH();
	}

	// �Ҹ��� ȣ��
	if (_bPlacementNew == true) {
		pData->~DATA();
	}
	// ������ Push()
	pNode->pNEXT = _pFreeNode;									// top�� ������带 ����Ű��
	_pFreeNode = pNode;											// ���� top!

	// ������ ���Դٸ� ǥ��
	//pNode->CHECKSUM ^= USEDMASK;								// �Ҵ� ������ ������ ���(ON)

	--_iUseCount;												// �ϳ� ����

	return true;
}
