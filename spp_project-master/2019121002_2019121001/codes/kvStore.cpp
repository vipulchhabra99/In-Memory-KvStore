#include<iostream>
#include<cstring>
#include<stdint.h>
#include<pthread.h>
using namespace std;
#define T 4
#define KLEN 64
#define VLEN 256


struct Slice
{
	uint8_t size;
	char *data;
};


struct KeyValue
{
	char key[KLEN];
	char value[VLEN];
};


class BTreeNode
{
	private:
	
	char keys[2*T-1][KLEN];
	char values[2*T-1][VLEN];
	int t;
	BTreeNode *C[2*T];
	int nC[2*T] = {0};
	int n;
	bool leaf;

	public:

	BTreeNode(int _t, bool _leaf);
	void traverse();
	BTreeNode *search(char *k);
	KeyValue *searchn(int nk);
	int findKey(char *k);
	bool insertNonFull(char *k, char *v);
	void splitChild(int i, BTreeNode *y);
	bool remove(char *k);
	void removeFromLeaf(int idx);
	void removeFromNonLeaf(int idx);
	BTreeNode *getPred(int idx);
	BTreeNode *getSucc(int idx);
	void fill(int idx);
	void borrowFromPrev(int idx);
	void borrowFromNext(int idx);
	void merge(int idx);
	int keyCount();
	friend class BTree;
};


class BTree
{
	private:
	
	BTreeNode *root;
	int t;
	
	public:

	BTree(int _t)
	{
		root = NULL;
		t = _t;
	}


	void traverse()
	{
		if (root != NULL) root->traverse();
	}


	bool search(char *k, char *vdata, uint8_t *vsize)
	{
		if (root == NULL)
			return false;
		BTreeNode *btn = root->search(k);
		
		if (btn == NULL)
			return false;
		
		for (int i = 0; i < btn->n; ++i)
		{
			if (strcmp(btn->keys[i], k) == 0)
			{
				strcpy(vdata, btn->values[i]);
				*vsize = strlen(vdata);
				return true;
			}
		}

		return false;
	}


	bool searchn(int nk, char *kdata, uint8_t *ksize, char *vdata, uint8_t *vsize)
	{
		if (root == NULL || nk >= root->keyCount())
			return false;
		else
		{
			KeyValue *kv = root->searchn(nk);
			strcpy(kdata, kv->key);
			*ksize = strlen(kdata);
			strcpy(vdata, kv->value);
			*vsize = strlen(vdata);
			delete kv;
			return true;
		}
	}


	bool insert(char *k, char *v);


	bool remove(char *k);


	bool removen(int nk)
	{
		if (root == NULL || nk >= root->keyCount())
			return false;
		else
		{
			KeyValue *kv = root->searchn(nk);
			remove(kv->key);
			delete kv;
			return true;
		}
	}
};


class kvStore
{
	private:

	BTree *t;
	pthread_rwlock_t rwlock_id;

	public:

	kvStore(uint64_t max_entries)
	{
		t = new BTree(T);
		pthread_rwlock_init(&rwlock_id, NULL);
	}
	
	
	bool get(Slice& key, Slice& value) //returns false if key didn’t exist
	{
		value.data = new char[VLEN];
		pthread_rwlock_rdlock(&rwlock_id);
		bool ret = t->search(key.data, value.data, &value.size);
		pthread_rwlock_unlock(&rwlock_id);
		return ret;
	}

	
	bool put(Slice& key, Slice& value) // returns true if value overwritten
	{
		pthread_rwlock_wrlock(&rwlock_id);
		bool ret = t->insert(key.data, value.data);
		pthread_rwlock_unlock(&rwlock_id);
		return ret;
	}

	
	bool del(Slice& key)
	{
		pthread_rwlock_wrlock(&rwlock_id);
		bool ret = t->remove(key.data);
		pthread_rwlock_unlock(&rwlock_id);
		return ret;
	}

	
	bool get(int N,Slice& key, Slice& value) //returns Nth key-value pair
	{
		key.data = new char[KLEN];
		value.data = new char[VLEN];
		pthread_rwlock_rdlock(&rwlock_id);
		bool ret = t->searchn(N, key.data, &key.size, value.data, &value.size);
		pthread_rwlock_unlock(&rwlock_id);
		return ret;
	}

	
	bool del(int N)
	{
		pthread_rwlock_wrlock(&rwlock_id);
		bool ret = t->removen(N);
		pthread_rwlock_unlock(&rwlock_id);
		return ret;
	}
};


BTreeNode::BTreeNode(int t1, bool leaf1)
{
	t = t1;
	leaf = leaf1;
	n = 0;
}


int BTreeNode::findKey(char *k)
{
	int idx=0;
	while (idx<n && strcmp(keys[idx], k) < 0)
		++idx;
	return idx;
}


bool BTreeNode::remove(char *k)
{
	int idx = findKey(k);

	if (idx < n && strcmp(keys[idx], k) == 0)
	{
		if (leaf)
			removeFromLeaf(idx);
		else
			removeFromNonLeaf(idx);
		return true;
	}
	else
	{
		if (leaf)
		{
			return false;
		}

		bool flag = ( (idx==n)? true : false );

		if (C[idx]->n < t)
			fill(idx);

		bool key_exist;

		if (flag && idx > n)
		{
			key_exist = C[idx-1]->remove(k);
			if (key_exist)
				nC[idx-1]--;
		}
		else
		{
			key_exist = C[idx]->remove(k);
			if (key_exist)
				nC[idx]--;
		}

		return key_exist;
	}
}


void BTreeNode::removeFromLeaf (int idx)
{
	for (int i=idx+1; i<n; ++i)
	{
		strcpy(keys[i-1], keys[i]);
		strcpy(values[i-1], values[i]);
	}

	n--;
}


void BTreeNode::removeFromNonLeaf(int idx)
{
	char k[KLEN];
	strcpy(k, keys[idx]);

	if (C[idx]->n >= t)
	{
		BTreeNode *pred = getPred(idx);
		strcpy(keys[idx], pred->keys[pred->n-1]);
		strcpy(values[idx], pred->values[pred->n-1]);
		C[idx]->remove(keys[idx]);
		nC[idx]--;
	}

	else if (C[idx+1]->n >= t)
	{
		BTreeNode *succ = getSucc(idx);
		strcpy(keys[idx], succ->keys[0]);
		strcpy(values[idx], succ->values[0]);
		C[idx+1]->remove(keys[idx]);
		nC[idx+1]--;
	}

	else
	{
		merge(idx);
		C[idx]->remove(k);
		nC[idx]--;
	}
}


BTreeNode *BTreeNode::getPred(int idx)
{
	BTreeNode *cur=C[idx];
	while (!cur->leaf)
		cur = cur->C[cur->n];
	return cur;
}


BTreeNode *BTreeNode::getSucc(int idx)
{
	BTreeNode *cur = C[idx+1];
	while (!cur->leaf)
		cur = cur->C[0];
	return cur;
}


void BTreeNode::fill(int idx)
{
	if (idx!=0 && C[idx-1]->n>=t)
		borrowFromPrev(idx);

	else if (idx!=n && C[idx+1]->n>=t)
		borrowFromNext(idx);

	else
	{
		if (idx != n)
			merge(idx);
		else
			merge(idx-1);
	}
}


void BTreeNode::borrowFromPrev(int idx)
{

	BTreeNode *child=C[idx];
	BTreeNode *sibling=C[idx-1];

	for (int i=child->n-1; i>=0; --i)
	{
		strcpy(child->keys[i+1], child->keys[i]);
		strcpy(child->values[i+1], child->values[i]);
	}

	if (!child->leaf)
	{
		/*
		memmove(child->C+1, child->C, sizeof(void *)*(child->n+1));
		memmove(child->nC+1, child->nC, sizeof(int)*(child->n+1));
		*/
		for(int i=child->n; i>=0; --i)
		{
			child->C[i+1] = child->C[i];
			child->nC[i+1] = child->nC[i]; 
		}
		
	}

	strcpy(child->keys[0], keys[idx-1]);
	strcpy(child->values[0], values[idx-1]);

	if(!child->leaf)
	{
		child->C[0] = sibling->C[sibling->n];
		child->nC[0] = sibling->nC[sibling->n];
		nC[idx] += child->nC[0];
		nC[idx-1] -= child->nC[0];
	}

	strcpy(keys[idx-1], sibling->keys[sibling->n-1]);
	strcpy(values[idx-1], sibling->values[sibling->n-1]);

	child->n += 1;
	sibling->n -= 1;

	nC[idx-1]--;
	nC[idx]++;
}


void BTreeNode::borrowFromNext(int idx)
{
	BTreeNode *child=C[idx];
	BTreeNode *sibling=C[idx+1];

	strcpy(child->keys[(child->n)], keys[idx]);
	strcpy(child->values[(child->n)], values[idx]);

	if (!(child->leaf))
	{
		child->C[(child->n)+1] = sibling->C[0];
		child->nC[(child->n)+1] = sibling->nC[0];
		nC[idx] += child->nC[(child->n)+1];
		nC[idx+1] -= child->nC[(child->n)+1];
	}

	strcpy(keys[idx], sibling->keys[0]);
	strcpy(values[idx], sibling->values[0]);

	for (int i=1; i<sibling->n; ++i)
	{
		strcpy(sibling->keys[i-1], sibling->keys[i]);
		strcpy(sibling->values[i-1], sibling->values[i]);
	}

	if (!sibling->leaf)
	{
		for(int i=1; i<=sibling->n; ++i)
		{
			sibling->C[i-1] = sibling->C[i];
			sibling->nC[i-1] = sibling->nC[i];
		}
	}

	child->n += 1;
	sibling->n -= 1;

	nC[idx+1]--;
	nC[idx]++;

	return;
}


void BTreeNode::merge(int idx)
{
	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx+1];

	strcpy(child->keys[t-1], keys[idx]);
	strcpy(child->values[t-1], values[idx]);

	for (int i=0; i<sibling->n; ++i)
	{
		strcpy(child->keys[i+t], sibling->keys[i]);
		strcpy(child->values[i+t], sibling->values[i]);
	}

	if (!child->leaf)
	{
		memcpy(child->C+t, sibling->C, sizeof(void *)*(sibling->n+1));
		memcpy(child->nC+t, sibling->nC, sizeof(int)*(sibling->n+1));

		/*
		for(int i=0; i<=sibling->n; ++i)
		{
			child->C[i+t] = sibling->C[i];
			child->nC[i+t] = sibling->nC[i];
		}
		*/
		
	}

	for (int i=idx+1; i<n; ++i)
	{
		strcpy(keys[i-1], keys[i]);
		strcpy(values[i-1], values[i]);
	}

	nC[idx] += nC[idx+1] + 1;

	/*
	memmove(C+idx+1, C+idx+2, sizeof(void *) * (n-idx-1));
	memmove(nC+idx+1, nC+idx+2, sizeof(int) * (n-idx-1));
	*/
	for (int i=idx+2; i<=n; ++i)
	{
		C[i-1] = C[i];
		nC[i-1] = nC[i];
	}
	

	child->n += sibling->n+1;
	n--;

	delete(sibling);
}


bool BTree::insert(char *k, char *v)
{
	if (root)
	{
		if (root->n == 2*t-1)
		{
			BTreeNode *s = new BTreeNode(t, false);

			s->C[0] = root;
			s->nC[0] = 1;

			s->splitChild(0, root);

			root = s;

			if (strcmp(s->keys[0], k) == 0)
			{
				strcpy(s->values[0], v);
				return true;
			}

			int i = 0;
			if (strcmp(s->keys[0], k) < 0)
				i++;

			bool overwrite = s->C[i]->insertNonFull(k, v);
			if (!overwrite)
				s->nC[i]++;

			return overwrite;
		}
		else
			return root->insertNonFull(k, v);
	}
	else
	{

		root = new BTreeNode(t, true);
		strcpy(root->keys[0], k);
		strcpy(root->values[0], v);
		root->n = 1;
		return false;
	}
}


bool BTreeNode::insertNonFull(char *k, char *v)
{
	for (int i = 0; i < n; ++i)
	{
		if (strcmp(keys[i], k) == 0)
		{
			strcpy(values[i], v);
			return true;
		}
	}

	int i = n-1;

	if (leaf == true)
	{
		while (i >= 0 && strcmp(keys[i], k) > 0)
		{
			strcpy(keys[i+1], keys[i]);
			strcpy(values[i+1], values[i]);
			i--;
		}

		strcpy(keys[i+1], k);
		strcpy(values[i+1], v);
		n = n+1;
		return false;
	}
	else
	{

		while (i >= 0 && strcmp(keys[i], k) > 0)
			i--;

		if (C[i+1]->n == 2*t-1)
		{
			splitChild(i+1, C[i+1]);

			if (strcmp(keys[i+1], k) == 0)
			{
				strcpy(values[i+1], v);
				return true;
			}

			if (strcmp(keys[i+1], k) < 0)
				i++;
		}

		bool overwrite = C[i+1]->insertNonFull(k, v);
		if (!overwrite)
			nC[i+1]++;
		return overwrite;
	}
}


void BTreeNode::splitChild(int i, BTreeNode *y)
{
	BTreeNode *z = new BTreeNode(y->t, y->leaf);
	z->n = t - 1;

	for (int j = 0; j < t-1; j++)
	{
		strcpy(z->keys[j], y->keys[j+t]);
		strcpy(z->values[j], y->values[j+t]);
	}

	if (y->leaf == false)
	{
		for (int j = 0; j < t; j++)
		{
			z->C[j] = y->C[j+t];
			z->nC[j] = y->nC[j+t];
		}
	}

	y->n = t - 1;
	/*
	memmove(C+i+2, C+i+1, sizeof(void *) * (n-i));
	memmove(nC+i+2, nC+i+1, sizeof(int) * (n-i));
	*/

	
	for (int j = n; j >= i+1; j--)
	{
		C[j+1] = C[j];
		nC[j+1] = nC[j];
	}
	

	C[i+1] = z;

	nC[i] = y->keyCount();
	nC[i+1] = z->keyCount();

	for (int j = n-1; j >= i; j--)
	{
		strcpy(keys[j+1], keys[j]);
		strcpy(values[j+1], values[j]);
	}

	strcpy(keys[i], y->keys[t-1]);
	strcpy(values[i], y->values[t-1]);

	n = n + 1;
}


int BTreeNode::keyCount()
{
	uint64_t count = n;

	for (int i = 0; i <= n; ++i)
		count += nC[i];

	return count;
}


void BTreeNode::traverse()
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (leaf == false)
			C[i]->traverse();

		cout << keys[i] << " : " << values[i] << endl;
	}

	if (leaf == false)
		C[i]->traverse();
}


BTreeNode *BTreeNode::search(char *k)
{
	int i = 0;
	while (i < n && strcmp(k, keys[i]) > 0)
		i++;

	if (i < n && strcmp(keys[i], k) == 0)
		return this;

	if (leaf == true)
		return NULL;

	return C[i]->search(k);
}


bool BTree::remove(char *k)
{
	if (!root)
		return false;

	bool key_exist = root->remove(k);

	if (root->n==0)
	{
		BTreeNode *tmp = root;
		if (root->leaf)
			root = NULL;
		else
			root = root->C[0];

		delete tmp;
	}

	return key_exist;
}


KeyValue *BTreeNode::searchn(int nk)
{
	if (leaf)
	{
		KeyValue *kv = new KeyValue;
		strcpy(kv->key, keys[nk]);
		strcpy(kv->value, values[nk]);
		return kv;
	}

	else
	{
		int c = 0;
		int i;
		for (i = 0; i < n; ++i)
		{
			c += nC[i];
			if (c > nk)
				return C[i]->searchn(nk - (c - nC[i]));

			c += 1;
			if (c > nk)
			{
				KeyValue *kv = new KeyValue;
				strcpy(kv->key, keys[i]);
				strcpy(kv->value, values[i]);
				return kv;
			}
		}
		
		return C[i]->searchn(nk - c);
	}
}

