﻿/*
1. 메모리할당 관리 클래스를 만듬 (이는 전역클래스)
2. new 를 오버로딩 하여 메모리 생성시 해당 정보를 저장
   (메모리 할당 소스파일,라인번호 / 할당 포인터 / 배열형태 / 등...)
3. delete 를 오버로딩하여 메모리 삭제시 오류 확인.
   (실제 할당된적이 있는 포인터인지, 배열형태는 맞는지)
4. 3번에서 문제가 없었다면 할당내역 삭제
5. 3번에서 오류가 났다면 로그저장

6. 메모리관리 클래스의 파괴자에서 아래 내용을 로그로 남김
   할당 후 파괴가 안된 메모리 로그저장


# 오류상황

NOALLOC - 메모리 해제시 할당되지 않은 포인터 입력.
ARRAY - new[] / delete[] 배열형태 생성과 삭제가 잘못됐을 때
LEAK  - 할당 후 해제되지 않은 메모리


# 로그 파일의 이름

localtime_s 함수를 사용하여 Alloc_YYYYMMDD_HHMMSS.txt 형태로 년월일시분초 파일이름


# 로그파일 내용 포멧

NOALLOC [0x00008123]
NOALLOC [0x00614134]
NOALLOC [0x00614134]
ARRAY   [0x00613ca0] [	   40] my_new\my_new.cpp : 350
LEAK    [0x00614130] [     44] my_new\my_new.cpp : 361
LEAK    [0x00613ca0] [     40] my_new\my_new.cpp : 366

- 오류상황 [할당메모리 번지] [사이즈] 파일이름 : 메모리 할당 줄


각 내용을 줄을 맞춰서 텍스트 파일에 저장.



# my_new.cpp / my_new.h  과 같이 cpp 와 h 파일로 제작.

실제 new, delete 를 사용하는 프로젝트에
"my_new.h" 를 include 하는 것으로 모든 new 와 delete 가
추적되는 코드로 바뀔 수 있도록 만들어야 함.
*/

#pragma once

// ==============================================================
//							MemProfiler
// --------------------------------------------------------------
// 메모리 누수 체크 
// new 동적할당 할때 정보를 받아서 저장
// delete 호출시 정보를 갱신
// 오류가 생기거나, 프로그램이 종료시 누수를 확인하라고 자동으로 파일출력
// ==============================================================
class MemProfiler
{
	// 싱글톤
public:
	static MemProfiler& Instance();
private:
	MemProfiler();

	// 정보를 저장할 list 구현
private:
	// 할당된 메모리가 무슨 상태인지
	enum class AllocState {
		Alloc = 0,			// 할당중
		Free,				// free완료
		Freed,				// 이미 free를 호출 했을때
		Array,				// new[] 로 호출하고 delete 로 지울때
		nArray,				// new 로 호출하고 delete[] 로 지울때
		ERROR,				// 그외 에러
	};

	// 저장할 정보들
	struct MemAllocInfo {
		void* AllocPtr;		// 할당된 곳 가르키는 포인터
		size_t Size;		// 할당된 사이즈
		int Line;			// 할당 시도한 라인
		char File[256];		// 할당 시도한 파일
		AllocState State;	// 할당된 메모리의 상태
	};


	// ==============================================================
	// 저장할 리스트
	// --------------------------------------------------------------
	// 할당을 시도할때마다 계속 꼬리에 저장
	// 할당 해지 시도시 state만 변경
	// 리스트 정보출력 메소드
	// ==============================================================
	class InfoList {
	private:
		// list 노드
		struct Node {
			MemAllocInfo Info;	// 정보
			Node* Tail;			// 다음리스트
		};

		int _count;				// 몇개를 할당했는지
		Node* _head;				// 리스트 시작지점
	public:
		// ==============================================================
		// 리스트 초기화
		// --------------------------------------------------------------
		// Head를 NULL
		// count를 0 으로 초기화
		// ==============================================================
		InfoList();

		// ==============================================================
		// 리스트 늘리기
		// --------------------------------------------------------------
		// 1. Node를 할당하고
		// 2. 정보를 복사하고
		// 3. 꼬리에추가
		//		머리가 없으면 할당한곳이 머리!
		// ==============================================================
		void Push_back(MemAllocInfo info);
		// ==============================================================
		// delete 시도!
		// --------------------------------------------------------------
		// 리스트를 순회하며
		// Info.AllocPtr == ptr 값을 찾음
		// 할당중이면 free상태 설정 / 이미 할당해지 되었으면 freed상태 설정
		// 찾지못했으면 포인터의 delete[] / delete 가 바뀐것 임으로
		// Info.AllocPtr == (int*)ptr - 1
		//		new[] 로 호출하고 delete 로 지울때
		// Info.AllocPtr == (int*)ptr + 1
		//		new 로 호출하고 delete[] 로 지울때
		// 처리한 상태를 반환하기
		// ==============================================================
		AllocState TryDelete(void* ptr);
		// ==============================================================
		// 리스트 정보 출력
		// --------------------------------------------------------------
		// Alloc_YYYYMMDD_HHMMSS.log 파일에 리스트를 순회하며 정보출력
		// ==============================================================
		void PrintInfo();
	};

private:
	InfoList _list; // 정보 저장할 리스트!

public:
	// ==============================================================
	// MemProfiler 소멸자
	// --------------------------------------------------------------
	// 소멸할떄 전체 정보 출력
	// ==============================================================
	~MemProfiler();

	// ==============================================================
	// Alloc 정보 넣어서 리스트에 넣기
	// --------------------------------------------------------------
	// MemAllocInfo 하나 만들어서
	// _list.Push_Back 하기
	// ==============================================================
	void AllocInfo(void* aPtr, size_t size, const char* file, int line);
	// ==============================================================
	// free 시도
	// --------------------------------------------------------------
	// _list.TryDelete 를 호출해서
	// TODO : 반환값으로 오류처리하기
	// ==============================================================
	void FreeInfo(void* aPtr);

};
// ==============================================================
// 오버로딩할 new
// --------------------------------------------------------------
// MemProfiler 클레스에 관리할 정보 넣기
// --------------------------------------------------------------
// - 위 방식의 new 호출 방법
// int* p = new("문자열", 숫자) int;
// char* p = new("문자열", 숫자) char[10];
// char* p = new(__FILE__, __LINE__) char[10];
// #define new new(__FILE__, __LINE__) 걸어서 그냥 new 사용하듯
// ==============================================================
void* operator new (size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);

// ==============================================================
// 오버로딩한 new에서 예외 발생시 사용할 delete
// ==============================================================
void operator delete (void* p, const char* file, int line);
void operator delete[](void* p, const char* file, int line);


// ==============================================================
// 실제 사용할 delete
// ==============================================================
void operator delete (void* p);
void operator delete[](void* p);


// ==============================================================
// 오버라이딩한 new 로 사용하게 매크로 걸어줌!
// ==============================================================
#define new new(__FILE__, __LINE__)