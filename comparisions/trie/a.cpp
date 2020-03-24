 /*
	SPECIFICATIONS:
	Max key size: 64 bytes
	Each key char can be (a-z) or (A-Z): Total 52 possible chars
	Max Value Size: 256 bytes, any ASCII value
	No DS/boost/STL etc Libraries to be used
	Only pthread to be used for multithreading
	Keys to be in strcmp (cstring) Order
	API calls would be blocking

	EVALUATION:
	Benchmark would first load data via put calls (10 million entries, 2 min time limit)
	Perform Single Threaded Transactions to verify kvStore functionality
	Multiple Transaction Threads with each thread calling one of the APIs
	Metrics:
		TPS (Transactions Per Second)
		Average CPU Usage
		Peak Memory Usage		
	Normalize all 3 metrics with Avg
	Score = (TPS*TPS)/(CPU*Mem)
*/

// remove later, just for testing
#include <cstdlib>
#include <ctime>
#include <iostream>
//_____

#include <pthread.h>
#include <cstring>
#include <cstdint>
#define ALPHABET_SIZE 52 
using namespace std; 


struct TrieNode
{
	struct TrieNode *children[ALPHABET_SIZE];
	uint64_t nChildren[ALPHABET_SIZE];  // Number of children.
	char *value;
};


struct Slice
{
	uint8_t size;
	char *data;
};


struct TrieNode *getNode();
bool insert(struct TrieNode *root, char *key, uint8_t ksize, char *value, uint8_t vsize);
bool search(struct TrieNode *root, char *key, uint8_t ksize, char *value, uint8_t *vsize);
bool searchn(struct TrieNode *root, uint64_t n, char *key, uint8_t *ksize, char *value, uint8_t *vsize);
static inline bool isEmpty(TrieNode *root);
void removenn(struct TrieNode *root, uint64_t n);
bool removen(struct TrieNode *root, uint64_t n);
struct TrieNode *remove(struct TrieNode *root, char *key, uint8_t ksize, bool *keyFound, int depth);


class kvStore
{
private:
	TrieNode *root;
	uint64_t max_entries;
	pthread_mutex_t lock;

public:
	kvStore(uint64_t max_entries);
	bool get(Slice &key, Slice &value); //returns false if key didn’t exist
	bool put(Slice &key, Slice &value); //returns true if value overwritten
	bool del(Slice &key);            
	bool get(int N, Slice &key, Slice &value); //returns Nth key-value pair
	bool del(int N); //free( Nth key-value pair)
};

/*
// remove later, just for testing
void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}
//______



// remove code inside main() later, just for testing
int main(int argc, char const *argv[])
{
	if (argc != 2)
		return 1;
	uint64_t N = atoi(argv[1]);
	srand(time(NULL));
	
	const int key_len = 64;
	const int val_len = 256;

	kvStore kv(N);
	struct Slice key, value;
	key.data = (char *)malloc( sizeof(char) * (key_len+1));
	value.data = (char *)malloc( sizeof(char) * (val_len+1));

	// cout << "::put::" << endl;
	// also check for overwrite cases
	for (int i=0; i<N; i++)
	{
		gen_random(key.data, key_len);
		key.size = strlen(key.data); 
		gen_random(value.data, val_len);
		value.size = strlen(value.data);
		kv.put(key, value);  // comment this if uncommenting below line
		// cout << kv.put(key, value) << "\t" << key.data << "\t" << (int)key.size << "\t" << value.data << "\t" << (int)value.size << endl; 
	}

	cout << "::get n::" << endl;
	for (int i=0; i<N; i++)
	{
		cout << kv.get(i, key, value) << "\t" << key.data << "\t" << (int)key.size << "\t" << value.data << "\t" << (int)value.size << endl; 
	}

	cout << "::del n::" << endl;
	for (int i = 0; i < N; i+=2)
	{
		cout << kv.del(i) << endl;
	}
	
	cout << "::get n::" << endl;
	for (int i=0; i<N/2+1; i++)
	{
		cout << kv.get(i, key, value) << "\t" << key.data << "\t" << (int)key.size << "\t" << value.data << "\t" << (int)value.size << endl; 
	}

	cout << "::get::" << endl;
	struct Slice value2;
	value2.data = malloc( sizeof(char) * (val_len+1));
	for (int i = 0; i < N/2; ++i)
	{
		kv.get(i, key, value);
		cout << kv.get(key, value2) << "\t" << key.data << "\t" << (int)key.size << "\t" << value2.data << "\t" << (int)value2.size << endl; 
	}

	cout << "::del::" << endl;


	return 0;
}
*/

kvStore::kvStore(uint64_t max_entries)
{
	this->max_entries = max_entries;
	this->root = getNode();
	pthread_mutex_init(&this->lock, NULL);
}


bool kvStore::get(Slice &key, Slice &value)
{
	return search(this->root, key.data, key.size, value.data, &value.size);
}


bool kvStore::put(Slice &key, Slice &value)
{
	pthread_mutex_lock (&this->lock);  // Shift this lock to data structure
	bool ret = insert(this->root, key.data, key.size, value.data, value.size);
	pthread_mutex_unlock (&this->lock);
	return ret;
}


bool kvStore::del(Slice &key)
{
	bool keyFound = false;
	pthread_mutex_lock (&this->lock);
	remove(this->root, key.data, key.size, &keyFound, 0);
	pthread_mutex_unlock (&this->lock);
	return keyFound;
}


bool kvStore::get(int N, Slice &key, Slice &value)
{
	*(key.data) = '\0';
	return searchn(this->root, N, key.data, &key.size, value.data, &value.size);	
}


bool kvStore::del(int N)
{
	pthread_mutex_lock (&this->lock);
	bool ret = removen(this->root, N);
	pthread_mutex_unlock (&this->lock);
	return ret;
}


struct TrieNode *getNode()
{ 
	struct TrieNode *pNode = (struct TrieNode *)malloc( sizeof(TrieNode) );
	memset(pNode, 0, sizeof(TrieNode));
	return pNode; 
} 


bool insert(struct TrieNode *root, char *key, uint8_t ksize, char *value, uint8_t vsize)
{ 
	struct TrieNode *pCrawl = root; 

	for (int i = 0; i < ksize; i++)
	{
		int index = key[i] > 'Z' ? key[i]-'a'+26 : key[i]-'A';
		pCrawl->nChildren[index]++;
		if (!pCrawl->children[index])
			pCrawl->children[index] = getNode(); 
		pCrawl = pCrawl->children[index]; 
	}

	if (pCrawl->value == NULL) 	// No value overwrite.
	{
		pCrawl->value = (char *)malloc( sizeof(char) * (vsize+1));
		strcpy(pCrawl->value, value);
		return false;
	}
	else  // Value overwrite.
	{
		free( pCrawl->value );
		pCrawl->value = (char *)malloc( sizeof(char) * (vsize+1));
		strcpy(pCrawl->value, value);
		pCrawl = root;
		for (int i = 0; i < ksize; i++)  // Take number of children to previous state.
		{
			int index = key[i] > 'Z' ? key[i]-'a'+26 : key[i]-'A';
			pCrawl->nChildren[index]--;
			pCrawl = pCrawl->children[index];
		}
		return true;		
	}
	// replace strncpy if errernous
	// pCrawl->value[vsize] = '\0';
} 


bool search(struct TrieNode *root, char *key, uint8_t ksize, char *value, uint8_t *vsize)  // NULL root value check
{ 
	/*
	if (!root)
		return false;
	*/
	struct TrieNode *pCrawl = root; 

	for (int i = 0; i < ksize; i++)
	{ 
		int index = key[i] > 'Z' ? key[i]-'a'+26 : key[i]-'A';
		if (!pCrawl->children[index]) 
			return false;
		pCrawl = pCrawl->children[index]; 
	} 

	if (pCrawl == NULL || pCrawl->value == NULL)
		return false;
	else
	{
		strcpy(value, pCrawl->value);	// Assuming value has enough space allocated
		*vsize = strlen(pCrawl->value);	// replace pCrawl->value with value for speed
		return true;
	}
} 


bool searchn(struct TrieNode *root, uint64_t n, char *key, uint8_t *ksize, char *value, uint8_t *vsize)
{
	/*
	if (!root)
		return false;
	*/
	if (root->value)
	{
		if (n)
			n--;
		else
		{  // found
			*ksize = strlen(key);
			*vsize = strlen(root->value);
			strcpy(value, root->value);
			return true;
		}
	}

	struct TrieNode *pCrawl = root;
	int csum = 0;	// Children sum
	for (int i = 0; i < ALPHABET_SIZE; ++i)
	{
		csum += pCrawl->nChildren[i];
		if (csum > n)
		{
			int keylen = strlen(key);
			key[keylen] = i >= 26 ? i - 26 + 'a' : i + 'A';
			key[keylen+1] = '\0';
			return searchn(root->children[i], n-csum+pCrawl->nChildren[i], key, ksize, value, vsize);
		}
	}
	return false;
}


static inline bool isEmpty(TrieNode *root) 
{
	const static char *zero[ALPHABET_SIZE] = {0};
	return !memcmp(zero, root->children, ALPHABET_SIZE * sizeof(char *));
}


void removenn(struct TrieNode *root, uint64_t n)
{
	if (root->value)
	{
		if (n)
			n--;
		else
		{
			free( root->value );
			root->value = NULL;
			return;
		}
	}

	struct TrieNode *pCrawl = root;
	int csum = 0;	// Children sum
	for (int i = 0; i < ALPHABET_SIZE; ++i)
	{
		csum += pCrawl->nChildren[i];
		if (csum > n)
		{
			root->nChildren[i]--;
			if (root->nChildren[i] == 0)
			{
				TrieNode *delptr = root->children[i];
				root->children[i] = NULL;
				while (!delptr->value)
				{
					TrieNode *temp = delptr;
					for (int j = 0; j < ALPHABET_SIZE; ++j)
						if (delptr->children[j])
							delptr = delptr->children[j];
					free( temp );
				}
				free( delptr->value );
				free( delptr );
			}		
			else
				removenn(root->children[i], n-csum+pCrawl->nChildren[i]+1);
			return;
		}
	}
}


bool removen(struct TrieNode *root, uint64_t n)
{	
	int count = 0;  // Children count.
	for (int i = 0; i < ALPHABET_SIZE; ++i)
		count += root->nChildren[i];

	if (n >= count)
		return false;
	else
	{
		removenn(root, n);
		return true;
	}
}


struct TrieNode *remove(struct TrieNode *root, char *key, uint8_t ksize, bool *keyFound, int depth = 0)
{ 
	if (!root) 
		return NULL;
	 
	else if (depth != ksize)
	{
		int index = key[depth] > 'Z' ? key[depth]-'a'+26 : key[depth]-'A';
		root->children[index] = remove(root->children[index], key, ksize, keyFound, depth + 1); 
		if (*keyFound == true)
			root->nChildren[index]--;
		if (isEmpty(root) && root->value == NULL)
		{ 
			free( (root) );
			root = NULL; 
		}
		return root; 
	}

	else  // If last character of key is being processed 
	{ 
		if (root->value) 
		{
			*keyFound = true;
			free( root->value );
			root->value = NULL;
			if (isEmpty(root))	// see whether this if has to go outside parent if, change was made to bring it inside.
			{
				free( root );
				root = NULL; 
			}
		}
		return root; 
	}
}


