## Just running this code

Code to generate random strings and store them<br/>
```c
void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}


int main(int argc, char const *argv[])
{
	if (argc != 2)
		return 1;
	int N = atoi(argv[1]);

	const int key_len = 64;
	const int val_len = 255;
	char *key_data = (char *)malloc( sizeof(char) * (key_len+1));
	char *value_data = (char *)malloc( sizeof(char) * (val_len+1));

	struct Node *root = NULL; 

	for (int i=0; i<N; i++)
	{
		gen_random(key_data, key_len); 
		gen_random(value_data, val_len);
	}

	return 0;
}
```

Takes time<br/>
```bash
aryam@comp:~/Desktop/tst$ g++ a.cpp
aryam@comp:~/Desktop/tst$ ./a.out 10
aryam@comp:~/Desktop/tst$ ./a.out 100
aryam@comp:~/Desktop/tst$ ./a.out 1000
aryam@comp:~/Desktop/tst$ ./a.out 1000
aryam@comp:~/Desktop/tst$ ./a.out 10000
aryam@comp:~/Desktop/tst$ ./a.out 100000
aryam@comp:~/Desktop/tst$ ./a.out 1000000
aryam@comp:~/Desktop/tst$ ./a.out 4000000
aryam@comp:~/Desktop/tst$ time ./a.out 10000000

real	1m10.899s
user	1m10.779s
sys	0m0.064s
aryam@comp:~/Desktop/tst$ 
```

So subtract this from final

## For running program tst


tst program <br/>
```cpp
#include <stdio.h> 
#include <stdlib.h> 


struct Node 
{ 
	char data; 
	unsigned isEndOfString: 1; 
	struct Node *left, *eq, *right; 
}; 


int num_calls;


struct Node* newNode(char data) 
{ 
	num_calls++;
	struct Node* temp = (struct Node*) malloc(sizeof( struct Node )); 
	temp->data = data; 
	temp->isEndOfString = 0; 
	temp->left = temp->eq = temp->right = NULL; 
	return temp; 
} 


void insert(struct Node** root, char *word) 
{ 
	if (!(*root)) 
		*root = newNode(*word); 

	if ((*word) < (*root)->data) 
		insert(&( (*root)->left ), word); 

	else if ((*word) > (*root)->data) 
		insert(&( (*root)->right ), word); 

	else
	{ 
		if (*(word+1)) 
			insert(&( (*root)->eq ), word+1); 

		else
			(*root)->isEndOfString = 1; 
	} 
} 


void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}


int main(int argc, char const *argv[])
{
	if (argc != 2)
		return 1;
	int N = atoi(argv[1]);

	const int key_len = 64;
	// const int val_len = 255;
	char *key_data = (char *)malloc( sizeof(char) * (key_len+1));
	// char *value_data = (char *)malloc( sizeof(char) * (val_len+1));

	struct Node *root = NULL; 

	for (int i=0; i<N; i++)
	{
		gen_random(key_data, key_len); 
		// gen_random(value_data, val_len);
		insert(&root, key_data); 
	}

	printf("num_calls: %d\nsizeof node: %lu", num_calls, sizeof(Node));

	return 0; 
} 
```



Output is<br/>
```bash
aryam@comp:~/Desktop/tst$ time ./a.out 1500000
num_calls: 91496643
sizeof node: 32
real	0m15.887s
user	0m13.286s
sys	0m2.566s
```


## Memory usage
1. top command shows size ~ 3.9 GB <br/>
2. num_calls * sizeof_node = 2927892576 bytes ~ 2.7268 GB <br/>
3. back envolop calc = 64(key_len) * 32(sizeof_node) * (1.5 * 10^6)(N) = 3072000000 bytes ~ 2.861 GB<br/>

Worst case new_nodes = 64 * (1.5 * 10^6) = 96000000
Observed new_nodes = num_calls = 91496643

Which is hardly any advantage, the ratio being 1.0492188221594096.

The real advantage of trie comes in real world applications where words are not random and prefixes are much more common. There we can exploit the trie property of common matching prefix.
In our case, where the benchmark is random, there is not much matching prefixes and so, there doesn't seem much of an advantage of using tries!


