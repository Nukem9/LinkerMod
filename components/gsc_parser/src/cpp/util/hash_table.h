#pragma once
#include "../sys/sys_platform.h"
#include <stdlib.h>
#include <string>
#include "llist.h"

typedef void (*hash_table_traverse_f)(int index, const char* key, void* value);

int Str_CalcHash(const char* str);

template <typename T>
class HashTable;

template <typename T>
class HashNode
{
friend class HashTable<T>;
private:
	const char* key;
	int hash;
	
	T value;

	HashNode<T>* prev;
	HashNode<T>* next;

protected:
	//
	// Adds this node before the argument node
	//
	void AddBefore(HashNode<T>* node);

public:
	HashNode(void);
	HashNode(const HashNode& node);
	HashNode(const char* key);
	
	~HashNode(void);
	
	HashNode& operator=(const HashNode& node);
	
	int Hash(void) const;
	const char* Key(void) const;
	T& Value(void);
};

template <typename T>
HashNode<T>::HashNode(void) :  value(), prev(NULL), next(NULL)
{
}

template <typename T>
HashNode<T>::HashNode(const HashNode& node) :  value(), prev(NULL), next(NULL)
{
	this->hash = node->hash;
	this->key = strdup(node.key);
}

template <typename T>
HashNode<T>::HashNode(const char* key) :  value(), prev(NULL), next(NULL)
{
	this->key = strdup(key);
	this->hash = Str_CalcHash(key);
}

template <typename T>
HashNode<T>::~HashNode()
{
	free((void*)key);
	
	if(this->prev)
	{
		if(next)
		{
			next->prev = prev;
			prev->next = next;
		}
		else
		{
			prev->next = NULL;
		}
	}
	else if(this->next)
	{
		next->prev = NULL;
	}
	
	///delete value;
	//value = NULL;
}

template <typename T>
HashNode<T>& HashNode<T>::operator=(const HashNode& node)
{
	free(key);
	
	this->hash = node->hash;
	this->key = strdup(node.key);
}

//
// Add this before node
//
template <typename T>
void HashNode<T>::AddBefore(HashNode<T>* node)
{
	this->next = node;
	this->prev = node->prev;
	if(prev)
	{
		prev->next = this;
	} 
	node->prev = this;
}

template <typename T>
int HashNode<T>::Hash(void) const
{
	return this->hash;
}

template <typename T>
const char* HashNode<T>::Key(void) const
{
	return this->key;
}

template <typename T>
T& HashNode<T>::Value(void)
{
	return this->value;
}

template <typename T>
class HashTable
{
private:
	static const int bucketCount = 256; //bucketCount must be a power of 2
	static const int hashMask = bucketCount - 1;
	HashNode<T>** buckets;
	
public:
	HashTable(void);
	~HashTable(void);
	
	//
	// Returns a pointer to the contents of a node with a matching key
	// or NULL if no matching node is found
	//
	T* Get(const char* key);
	
	//
	// Adds a new node with the given key, and return a pointer to its value
	// or returns a ptr to the contents of an existing node if a match already exists
	//
	T* Add(const char* key);
	
	//
	// Remove a node with the given key (if it exists) from the table
	//
	void RemoveNode(const char* key);
	
	//
	// Remove all nodes
	//
	void Clear(void);
	
	//
	// Traverse the hash table calling callback on each entry
	// return the number of elements
	//
	int Traverse(hash_table_traverse_f callback);
};

template <typename T>
HashTable<T>::HashTable()
{
	this->buckets = new HashNode<T>*[bucketCount];
	memset(buckets, 0, sizeof(HashNode<T>*) * bucketCount);
}

template <typename T>
HashTable<T>::~HashTable()
{
	this->Clear();	
	delete[] buckets;
}

template <typename T>
T* HashTable<T>::Get(const char* key)
{
	int hash = Str_CalcHash(key);
	
	for(HashNode<T>* node = buckets[hash & hashMask]; node; node = node->next)
	{
		if(strcmp(node->Key(), key) == 0)
		{
			return &node->value;
		}
	}
	
	return NULL;
}

template <typename T>
T* HashTable<T>::Add(const char* key)
{
	int hash = Str_CalcHash(key);
	
	if(buckets[hash & hashMask] == NULL)
	{
		HashNode<T>* node = new HashNode<T>(key);
		buckets[hash & hashMask] = node;
		return &node->value;
	}
	
	for(HashNode<T>* node = buckets[hash & hashMask]; node; node = node->next)
	{
		if(strcmp(node->Key(), key) == 0)
		{
			return &node->value;
		}
	}
	
	HashNode<T>* head = buckets[hash & hashMask];
	HashNode<T>* node = new HashNode<T>(key);
	node->AddBefore(head);
	buckets[hash & hashMask] = node;
	return &node->value;
}

template <typename T>
void HashTable<T>::RemoveNode(const char* key)
{
	int hash = Str_CalcHash(key);

	for(HashNode<T>* node = buckets[hash & hashMask]; node; node = node->next)
	{
		if(strcmp(node->Key(), key) == 0)
		{
			delete node;
			return;
		}
	}
}

template <typename T>
void HashTable<T>::Clear(void)
{
	for(int i = 0; i < bucketCount; i++)
	{
		for(HashNode<T>* node = buckets[i]; node;)
		{
			HashNode<T>* next = node->next;
			delete node;
			node = next;
		}
		
		buckets[i] = NULL;
	}
}

template <typename T>
int HashTable<T>::Traverse(hash_table_traverse_f callback)
{
	int count = 0;
	for(int i = 0; i < bucketCount; i++)
	{
		for(HashNode<T>* node = buckets[i]; node; node = node->next)
		{
			callback(count++, node->Key(), &node->value);
		}
	}
	
	return count;
}
