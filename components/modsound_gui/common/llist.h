#pragma once

template <typename T>
class LList
{
private:
	LList* head;
	
	LList* prev;
	LList* next;
	
	T* owner;
	
public:

	LList(void);
	virtual ~LList(void);
	
	bool IsEmpty(void) const;
	bool InList(void) const;
	
	int Size(void) const;
	void Clear(void);
	
	void  InsertBefore(LList<T>* node);
	void  InsertAfter(LList<T>* node);
	
	void AddToFront(LList<T>* node);
	void AddToEnd(LList<T>* node);
	
	void Remove(void);
	
	T* PrevElem(void) const;
	T* NextElem(void) const;
	
	T* Owner(void) const;
	void SetOwner(T* elem);
	
	LList<T>* HeadNode(void) const;
	LList<T>* PrevNode(void) const;
	LList<T>* NextNode(void) const;
};

template <typename T>
LList<T>::LList(void) : head(this), prev(this), next(this), owner(NULL)
{
}

template <typename T>
LList<T>::~LList(void)
{
	this->Clear();
}

template <typename T>
bool LList<T>::IsEmpty(void) const
{
	return head == head->next;
}

template <typename T>
bool LList<T>::InList(void) const
{
	return this != head;
}

//
// Returns the size of the current list (without the head)
//
template <typename T>
int LList<T>::Size(void) const
{
	int result = 0;
	for(LList<T>* node = head->next; node != head; node = node->next)
	{
		result++;
	}
	return result;
}

template <typename T>
void LList<T>::Clear(void)
{
	if(this == head) {
		while(next != this) {
			next->Remove();
		}
	} else {
		this->Remove();
	}
}

template <typename T>
void LList<T>::Remove(void)
{
	this->prev->next = this->next;
	this->next->prev = this->prev;
	
	this->next = this;
	this->prev = this;
	this->head = this;
}

template <typename T>
T* LList<T>::PrevElem(void) const
{
	return (!this->prev || this->prev == head) ? NULL : this->prev->owner;
}

template <typename T>
T* LList<T>::NextElem(void) const
{
	return (!this->next || this->next == head) ? NULL : this->next->owner;
}

template <typename T>
T*  LList<T>::Owner(void) const
{
	return this->owner;
}

template <typename T>
void  LList<T>::SetOwner(T* elem)
{
	this->owner = elem;
}

//
// Insert THIS before NODE
//
template <typename T>
void  LList<T>::InsertBefore(LList<T>* node)
{
	this->Remove();
	
	this->prev = node->prev;
	this->next = node;
	this->head = node->head;
	
	node->prev = this;
	prev->next = this;
}

//
// Insert THIS after NODE
//
template <typename T>
void  LList<T>::InsertAfter(LList<T>* node)
{
	this->Remove();
	
	this->prev = node;
	this->next = node->next;
	this->head = node->head;
	
	node->next = this;
	next->prev = this;
}

//
// Add the node to the beginning of THIS list
//
template <typename T>
void  LList<T>::AddToFront(LList<T>* node)
{
	node->InsertAfter(this->head);
}

//
// Add the node to the end of THIS list
//
template <typename T>
void  LList<T>::AddToEnd(LList<T>* node)
{
	node->InsertBefore(this->head);
}

template <typename T>
LList<T>* LList<T>::HeadNode(void) const
{
	return this->head;
}

template <typename T>
LList<T>* LList<T>::PrevNode(void) const
{
	return (this->prev == head) ? NULL : this->prev;
}

template <typename T>
LList<T>* LList<T>::NextNode(void) const
{
	return (this->next == head) ? NULL : this->next;
}
