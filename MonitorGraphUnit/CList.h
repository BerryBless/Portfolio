#pragma once
template <typename T>
class CList {
public:
	struct Node {
		T _data;
		Node *_Prev = nullptr;
		Node *_Next = nullptr;
	};

	class iterator {
	private:
		Node *_node;
		// ���Լ����� _node�� ���� �� �� ����
		friend void CList::insert(iterator iter, T data);
		friend iterator CList::erase(iterator iter);
	public:
		iterator(Node *node = nullptr) {
			//���ڷ� ���� Node �����͸� ����
			this->_node = node;
		}

		// iter++
		iterator operator ++(int) {
			iterator temp = *this;
			//���� ��带 ���� ���� �̵�
			this->_node = this->_node->_Next;
			return temp;
		}

		// ++iter
		iterator &operator++() {
			this->_node = this->_node->_Next;
			return *this;
		}

		// iter--
		iterator operator --(int) {
			iterator temp = *this;
			this->_node = this->_node->_Prev;
			return temp;
		}

		// --iter
		iterator &operator--() {
			this->_node = this->_node->_Prev;
			return *this;
		}

		T &operator *() {
			//���� ����� �����͸� ����
			return this->_node->_data;
		}
		bool operator ==(const iterator &other) {
			return _node == other._node;
		}
		bool operator !=(const iterator &other) {
			return _node != other._node;
		}
	};

public:
	CList() {
		// ������

		// ������ ����
		_head._Prev = nullptr;
		_head._Next = &_tail;

		// �Ӹ��� ����
		_tail._Prev = &_head;
		_tail._Next = nullptr;
	}
	~CList() {
		this->clear();
	}

	iterator begin() {
		//ù��° ������ ��带 ����Ű�� ���ͷ����� ����
		return iterator(_head._Next);
	}
	iterator end() {
		//Tail ��带 ����Ű��(�����Ͱ� ���� ��¥ ���� �� ���) ���ͷ����͸� ����
		//	�Ǵ� ������ ������ �� �ִ� ���ͷ����͸� ����
		return iterator(&_tail);
	}
	// �Ӹ��� �߰�
	void push_front(T data) {
		Node *pNode = new Node;
		// ������ �־��ְ�
		pNode->_data = data;

		// 1. pNode�� ���� _head
		// 2. pNode�� ������ _head._Next
		// 3. _head�� ������ pNode
		// 4. _head.������ ���� pNode
		pNode->_Prev = &_head;
		pNode->_Next = _head._Next;
		_head._Next = pNode;
		pNode->_Next->_Prev = pNode;

		++_size;
	}
	// ������ �߰�
	void push_back(T data) {
		Node *pNode = new Node;
		// ������ �־��ְ�
		pNode->_data = data;

		// 1. pNode�� ������ _tail
		// 2. pNode�� ���� _tail._Prev
		// 3. _tail.���� pNode
		// 4. pNode ���� ������ pNode
		pNode->_Next = &_tail;
		pNode->_Prev = _tail._Prev;
		_tail._Prev = pNode;
		pNode->_Prev->_Next = pNode;

		++_size;
	}
	T pop_front() {
		Node *pNode = _head._Next;
		// ������ �̾Ƶα�
		T data = pNode->_data;

		// 1. pNode�� ���� ������ pNode�� ����
		// 2. pNode�� ������ ���� pNode�� ��
		pNode->_Prev->_Next = pNode->_Next;
		pNode->_Next->_Prev = pNode->_Prev;

		--_size;
		// �Ҵ� ������ ����
		delete pNode;
		return data;
	}
	T pop_back() {
		Node *pNode = _tail._Prev;
		// ������ �̾Ƶα�
		T data = pNode->_data;

		// 1. pNode�� ���� ������ pNode�� ����
		// 2. pNode�� ������ ���� pNode�� ��
		pNode->_Prev->_Next = pNode->_Next;
		pNode->_Next->_Prev = pNode->_Prev;

		--_size;
		// �Ҵ� ������ ����
		delete pNode;
		return data;
	}
	void clear() {
		// ���ͷ����ͷ� ��ȸ�ϸ� �����
		CList<T>::iterator iter = this->begin();
		while (iter != this->end()) {
			iter = erase(iter);
		}
	}
	int size() { return _size; };
	bool empty() { return _size <= 0; };

	// sort


	void sort() {
		quick_sort(0, _size - 1);

		/* ���� ��Ʈ
		for (auto iter1 = this->begin(); iter1 != this->end(); ++iter1) {
			for (auto iter2 = iter1; ;) {
				++iter2;
				if (iter2 == this->end()) break;
				if ((*iter1 > *iter2)) {
					T temp = *iter1;
					*iter1 = *iter2;
					*iter2 = temp;
				}
			}
		}//*/
	}

	template <typename Compare>
	void sort(Compare comp) {
		/* ���� ��Ʈ
		PRO_BEGIN(L"sort");
		for (auto iter1 = this->begin(); iter1 != this->end(); ++iter1) {
			for (auto iter2 = iter1; ;) {
				++iter2;
				if (iter2 == this->end()) break;
				if (comp(*iter1, *iter2)) {
					T temp = *iter1;
					*iter1 = *iter2;
					*iter2 = temp;
				}
			}
		}
		PRO_END(L"sort");
		//*/

		quick_sort(0, _size - 1, comp);

	}

	// ����Ʈ ���� 1
	void quick_sort(int start, int end) {
		if (end <= start) return;

		T temp;

		// ����Ʈ �ε���
		int p = start;									// �ǹ�
		int l = start + 1;								// ����
		int r = end;									// ������

		// ���� ���� ���ͷ�����
		iterator pIter = begin();
		advance(pIter, start);							// �ǹ�
		iterator lIter = begin();
		advance(lIter, start + 1);						// ����
		iterator rIter = begin();
		advance(rIter, end);							// ������


		while (l <= r) {
			//while (arr[p] >= arr[l]) {
			while (*pIter >= *lIter) {
				++l;
				++lIter;
				if (l >= r) break;
			}

			//while (arr[p] <= arr[r]) {
			while (*pIter <= *rIter) {
				--r;
				--rIter;
				if (r < l) break;
			}

			if (l < r) {
				//Swap(arr[l], arr[r]);
				temp = *lIter;
				*lIter = *rIter;
				*rIter = temp;
			}
		}
		//Swap(arr[p], arr[r]);
		temp = *rIter;
		*rIter = *pIter;
		*pIter = temp;

		quick_sort(start, r);
		quick_sort(l, end);
	}



	// ����Ʈ ���� 2
	template <typename Compare>
	int quick_sort_patition(int start, int end, Compare comp) {
		int p = end;
		iterator pIter = begin();
		advance(pIter, p);

		// �����Ǿ� ���ҳ�
		int j = start;
		iterator jIter = begin();
		advance(jIter, j);

		// �����̸� ���ҳ�
		int i = start;
		iterator iIter = begin();
		advance(iIter, i);
		T temp;
		while (i < end) {
			if (comp(*iIter, *pIter)) {
				// �ǹ��� ���ϸ鼭 ������ ���ʿ� �����ϱ�
				temp = *iIter;
				*iIter = *jIter;
				*jIter = temp;

				++j;
				++jIter;
			}
			++i;
			++iIter;
		}
		// �ǹ� ����
		temp = *pIter;
		*pIter = *jIter;
		*jIter = temp;

		return j; // j�� �������� comp�� ���� �¿� �з� �Ϸ�
	}

	template <typename Compare>
	void quick_sort(int start, int end, Compare comp) {
		if (end <= start) return;


		// ����Ʈ �ε���
		int p = quick_sort_patition(start, end, comp);// �ǹ�
		quick_sort(start, p - 1, comp);
		quick_sort(p + 1, end, comp);
	}

	// uterator�� �պκп� data�߰�
	void insert(iterator iter, T data) {
		Node *pNode = new Node;
		// ������ �־��ְ�
		pNode->_data = data;

		// 1. pNode�� ������ iter
		// 2. pNode�� ���� iter�� ��
		// 3. _tail.���� pNode
		// 4. _tail.���� ������ pNode
		pNode->_Next = iter._node;
		pNode->_Prev = iter._node->_Prev;
		iter._node->_Prev = pNode;
		pNode->_Prev->_Next = pNode;

		++_size;
	}

	iterator erase(iterator iter) {
		//- ���ͷ������� �� ��带 ����.
		//- �׸��� ���� ����� ���� ��带 ī��Ű�� ���ͷ����� ����
		Node *pNode = iter._node;
		iterator next(pNode->_Next);
		// 1. pNode�� ���� ������ pNode�� ����
		// 2. pNode�� ������ ���� pNode�� ��
		pNode->_Prev->_Next = pNode->_Next;
		pNode->_Next->_Prev = pNode->_Prev;


		--_size;
		// �Ҵ� ������ ����
		delete pNode;
		return next;
	}
	void remove(T Data) {

		for (auto iter = begin(); iter != end(); ++iter) {
			if (*iter == Data)
				erase(iter);
		}
	}

	// p��Ģ�� ���� ����
	template <typename UnaryPredicate>
	void remove_if(UnaryPredicate p) {
		auto iter = begin();
		while (iter != end()) {
			if (p(*iter)) {
				iter = erase(iter);
			} else
				++iter;
		}
	}

private:
	int _size = 0;
	Node _head;
	Node _tail;
};

// ���� ���ͷ����͸� n��ŭ ������
template <typename InputIter, typename Distance >
void advance(InputIter &it, Distance n) {
	if (n < 0) {
		for (int i = 0; i < n; i++) {
			--it;
		}
	} else {
		for (int i = 0; i < n; i++) {
			++it;
		}
	}
}