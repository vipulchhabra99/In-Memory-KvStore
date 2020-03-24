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

	printf("num_calls: %d\n sizeof node: %lu", num_calls, sizeof(Node));

	return 0; 
} 

