#include "MemProfiler.h"
#include <iostream>
#include <cstring>
#include <ctime>
#undef new

#pragma region MemProfiler

// �̱���

MemProfiler& MemProfiler::Instance() {
	static MemProfiler instance;
	return instance;
}

MemProfiler::MemProfiler() {
	// �̱��� ������
}
MemProfiler::~MemProfiler() {
	// �̱��� �Ҹ���
	this->_list.PrintInfo();
}
// ��� ���� �ֱ�!
void MemProfiler::AllocInfo(void* aPtr, size_t size, const char* file, int line) {
	MemAllocInfo info;	// MemAllocInfo ����ü
	// ����ü�� ���� �ֱ�
	info.AllocPtr = aPtr;
	info.Size = size;
	strcpy_s(info.File, strlen(file) + 1, file);
	info.Line = line;
	info.State = AllocState::Alloc;
	// ����ü�� ����Ʈ�� �ޱ�
	_list.Push_back(info);
}

#pragma region List


MemProfiler::InfoList::InfoList() {
	// ����Ʈ �ʱ�ȭ
	_head = nullptr;
	_count = 0;
}
void MemProfiler::InfoList::Push_back(MemAllocInfo info) {
	// ����Ʈ ���� �߰��ϱ�
	Node* newNode = new Node;	// �� ��� �Ҵ�
	// �� ��忡 �����ֱ�
	newNode->Info = info;		
	newNode->Tail = nullptr;
	// ó�����͸�
	if (_head == nullptr) {
		_head = newNode;
	}
	else {
		// ������ �ޱ�
		Node* cur = _head;
		while (cur->Tail != nullptr) {
			cur = cur->Tail;
		}
		cur->Tail = newNode;
	}
	_count++;
}

MemProfiler::AllocState MemProfiler::InfoList::TryDelete(void* ptr) {
	if (_head == nullptr) { printf_s("\nMemProfiler::InfoList::TryDelete :: Head == nullptr\n"); return AllocState::ERROR; }
	// ������ ��ȸ�ϸ�
	Node* itr = _head;
	while (itr != nullptr) {
		// ���� ���뿡 �°� ó��
		if (itr->Info.AllocPtr == ptr) {
			if (itr->Info.State == AllocState::Free) {
				// �̹� �ʱ�ȭ�Ȱ� �ٽ� �ʽ�ȭ �õ��Ҷ� ����
				// TODO delete�� ���ϰ� ���� ��������
				itr->Info.State = AllocState::Freed;
				return AllocState::Freed;
			}
			// ���������� ����
			itr->Info.State = AllocState::Free;
			return AllocState::Free;
		}
		else if (itr->Info.AllocPtr == (int*)ptr - 1) {
			// new[] �� ȣ���ϰ� delete �� ���ﶧ
			itr->Info.State = AllocState::Array;
			return AllocState::Array;
		}
		else if (itr->Info.AllocPtr == (int*)ptr + 1) {
			// new �� ȣ���ϰ� delete[] �� ���ﶧ
			itr->Info.State = AllocState::nArray;
			return AllocState::nArray;
		}
		itr = itr->Tail;
	}
	// ���ʿ� �̻��Ѱ� ����
	return AllocState::ERROR;
}
// ������� ���
void MemProfiler::InfoList::PrintInfo() {
	if (_head == nullptr) { printf_s("\MemProfiler::InfoList::PrintInfo :: Head == nullptr\n"); return; }

	FILE* fp = stdout;
	//* ���� �����
	// Alloc_YYYYMMDD_HHMMSS.log
	char filename[26];	// ���������� �̸�

	// �ð����ϱ�
	time_t now = time(0);
	struct tm tstruct;

	localtime_s(&tstruct, &now);
	strftime(filename, sizeof(filename), "Alloc_%Y%m%e_%H%M%S.log", &tstruct); // (Alloc_20210828_173840.log)

	// ������ ������ �ڿ� �߰�
	fopen_s(&fp, filename, "a");
	if (fp == NULL) {
		//CRASH
		int *p = nullptr;
		*p = 10;
		return;
	}
	fseek(fp, 0, SEEK_END);
	//*/
	Node* itr = _head;
	while (itr != nullptr) {
		switch (itr->Info.State)
		{
		case AllocState::Alloc:		// delete ȣ������ ����!
			fprintf_s(fp, "LEAK    [0x%p] [%6Iu] %s : %d\n", itr->Info.AllocPtr, itr->Info.Size, itr->Info.File, itr->Info.Line);
			break;
		case AllocState::Free:		// �����۵�!
			fprintf_s(fp, "NOALLOC [0x%p]\n", itr->Info.AllocPtr);
			break;
		case AllocState::Freed:		// delete 2���̻� ȣ��!
			// TODO �����޽���
			fprintf_s(fp, "Freed   [0x%p] [%6Iu] %s : %d\n", itr->Info.AllocPtr, itr->Info.Size, itr->Info.File, itr->Info.Line);
			break;
		case AllocState::Array:		// new[] �� ȣ���ϰ� delete �� ���ﶧ
			fprintf_s(fp, "ARRAY   [0x%p] [%6Iu] %s : %d\n", itr->Info.AllocPtr, itr->Info.Size, itr->Info.File, itr->Info.Line);
			break;
		case AllocState::nArray:	// new �� ȣ���ϰ� delete[] �� ���ﶧ
			fprintf_s(fp, "nArray  [0x%p] [%6Iu] %s : %d\n", itr->Info.AllocPtr, itr->Info.Size, itr->Info.File, itr->Info.Line);
			break;

		default:
			break;
		}
		itr = itr->Tail;
	}
	fclose(fp);
}
#pragma endregion
// ������� ����
void MemProfiler::FreeInfo(void* aPtr) {
	AllocState state = _list.TryDelete(aPtr);
	if (state != AllocState::Free) {
		// TODO  ��������
		Instance()._list.PrintInfo();
	}
}


#pragma endregion


#pragma region new overloading

// �����Ҵ�

// const �Ⱥ��̸� ���� (2019)
void* operator new (size_t size, const char* file, int line) {
	void* p = malloc(size);

	MemProfiler::Instance().AllocInfo(p, size, file, line);
	return p;
}

void operator delete (void* p, const char* file, int line) {}
void operator delete (void* p) {
	MemProfiler::Instance().FreeInfo(p);
	free(p);
}


// �迭�� �Ҵ�
void* operator new[](size_t size, const char* file, int line) {
	void* p = malloc(size);
	MemProfiler::Instance().AllocInfo(p, size, file, line);
	return p;
}

void operator delete[](void* p, const char* file, int line) {}
void operator delete[](void* p) {
	MemProfiler::Instance().FreeInfo(p);
	free(p);
}



#pragma endregion