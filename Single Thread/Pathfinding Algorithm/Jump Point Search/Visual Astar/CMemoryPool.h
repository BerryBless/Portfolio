#pragma once

/*---------------------------------------------------------------

	procademy MemoryPool.

	메모리 풀 클래스 (오브젝트 풀 / 프리리스트)
	특정 사이즈를 정하고 그 사이즈만큼 블럭을 만들어 관리한다

	- 사용법.

	procademy::CMemoryPool MemPool(300); // 한칸에 300바이트 사용가능 블럭
	DATA *pData = (DATA *)MemPool.Alloc();

	pData 사용

	MemPool.Free(pData);

----------------------------------------------------------------*/

// 일부로 크래쉬내기
#define dfCRASH() do{int *p = nullptr; *p = 10;}while(0)

class CMemoryPool {
	//////////////////////////////////////////////////////////////////////////
	// 체크섬
	// 유저영역을 가르키는 포인터의 앞 21비트를 안쓴다는것을 응용
	// 체크섬의 기초데이터(포인터)는 this의 포인터
	//////////////////////////////////////////////////////////////////////////
	enum MASK : unsigned long long{
		USEDMASK	= 0xABAB0000'00000000,// 이 오브젝트가 메모리 풀 안에 있는가?
		POINTERMASK = 0x0000FFFF'FFFFFFFF,// this포인터를 저장할곳!
	};

public:
	//////////////////////////////////////////////////////////////////////////
	//		헤더
	// 스택 방식으로 반환된 (미사용) 오브젝트 블럭을 관리.
	//
	// ----------------- <- _pFreeNode가 의미하는 주소
	// | st_BLOCK_NODE |
	// ----------------- <- Alloc() 이 리턴하는 주소
	// |               |	이 부분은 그냥 주소로 관리
	// | Alloc Memory  |	노드에 있는것이 아님,
	// |               |	노드 포인터에 노드사이즈 더하면 나오는 주소
	// |               |
	// |               |
	// ----------------- <- 오버플로 확인 8의 경계에 세우기 위해 8바이트 할당
	// |	8byte	   | 
	// -----------------
	// 
	// 체크섬의 앞1바이트의 데이터가 
	// - USEDMASK다 -> 사용중
	// - 00 이다 -> 풀에서 관리중
	// - XOR(^)연산으로 토글하면서 사용
	// 오버플로 / 언더플로 확인
	// 	   - 오버플로 : 뒤 8바이트를 0으로 초기화 하고 free할때 값이 0인지 확인
	// 	   - 언더플로 : iBlockSize가 _iBlockSize 보다 크거나, 0보다 작으면 언더플로
	//////////////////////////////////////////////////////////////////////////
	struct st_BLOCK_NODE {
		alignas(8) unsigned long long			CHECKSUM;		// 체크섬
		alignas(8) st_BLOCK_NODE				*pNEXT;			// 다음노드
		alignas(8) long long					iBlockSize;		// 사용할 사이즈
	};
private:
	st_BLOCK_NODE	*_pFreeNode;								// 메모리풀 스택의 top
	int				_iBlockSize;								// 이 풀이 관리하는 블록 사이즈
	int				_iCapacity;									// 현재 확보된 블럭수
	int				_iUseCount;									// 현재 사용중인 블럭수
public:

	//////////////////////////////////////////////////////////////////////////
	// 생성자, 파괴자.
	//
	// Parameters:	(int) 블럭의 사이즈
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMemoryPool(int _iBlockSize);

	virtual	~CMemoryPool();


	//////////////////////////////////////////////////////////////////////////
	// 블럭 하나를 할당받는다.  
	//
	// Parameters: 없음.
	// Return: (DATA *) 데이타 블럭 포인터.
	//////////////////////////////////////////////////////////////////////////
	void *Alloc(void);

	//////////////////////////////////////////////////////////////////////////
	// 사용중이던 블럭을 해제한다.
	//
	// Parameters: (DATA *) 블럭 포인터.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	bool	Free(void *pData);



	//////////////////////////////////////////////////////////////////////////
	// 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
	//
	// Parameters: 없음.
	// Return: (int) 메모리 풀 내부 전체 개수
	//////////////////////////////////////////////////////////////////////////
	int		GetCapacityCount(void) { return _iCapacity; }

	//////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 블럭 개수를 얻는다.
	//
	// Parameters: 없음.
	// Return: (int) 사용중인 블럭 개수.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseCount(void) { return _iUseCount; }

	//////////////////////////////////////////////////////////////////////////
	// 이 풀이 관리하는 사이즈를 얻는다.
	//
	// Parameters: 없음.
	// Return: (int) 관리하는 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetBlockSize(void) { return _iBlockSize; }
};
