#include "kvStore.cpp"


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
	uint64_t N = atoi(argv[1]);
	srand(time(NULL));
	
	const int key_len = 10;
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

	Slice ki, vi;
	ki.data = new char[1000];
	vi.data = new char[1000];
	for (int i = N-1; i > 5; --i)
	{
		if(kv.del(i))
			cout << ".";
		else
			cout << "x";
		//cout << ki.data << endl;
	}
	return 0;
}
