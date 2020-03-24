# SPP project.
# Implementation via modified Trie.

* **To compile & run**
```bash
g++ a.cpp
./a.out 10000000  # 10 million in 2 minutes
```


* **Notes**
	1. Searching for faster implementation of trie, found radix tree.
	1. Assume empty(0 length) values insert not allowed; if have to implement, then add back isEndOfWord which was removed for value.
	1. cstring faster than string library, benchmark proofs in internet.
	1. insert() function uses assumption that new inserts will be greater than overwrites. Else change order in which nChildren will change down the tree.
	1. Couldn't implement radix tree yes since too complex. Made a modified version of trie for now. Will later upddate to radix tree if time permits.
	1. Keep default value of keyFound in remove(d.c) false; Remove/modify keyFound if clarification of return value something else.
	1. Make sure that key sent to searchn is empty string "\0".
	1. All threads except last will handle get requests, last thread will handle rest of the requests.
---
* **Todo**
	1. Handle empty string case in a.cpp.
	1. See if remove() can be made iterative.
	1. See if can change data type of nChildren from uint64_t to uint32or16_t.
	1. See if converting Trie to class has any advantages.
	1. Remove keyFound in remove and add as class member to reduce number of parameters, if converted to class.
	1. Optimize by replacing threadCount-1 to threadCount.
	1. Add joins after creates.
	1. Remove all multithreading(*see Doubt 2*)?
	1. Use atomic array nChildren to avoid locks.
	1. In put() and putn(), use locks only at the last steps. Changes are just local.
	1. Value management system for values; since only key in trie. Try own memory management system. If not own, then replace new/delete with malloc/free. Also try searching github for some already implemented fast memory allocator.
	1. Don't use lock if |client| = 1. Probably keep some kind of atomic variable for number of characters.
	1. Sir idea(not todo but to take ideas): seperate key and value management systems. Interface between the key and value systems.
---
* **Doubt**
	1. How to allocate data in slice; or is it given to us allocated and we just have to copy?
	1. No need of multithreading? 
		1. As per specification, multiple client threads will be created.
		1. Those threads will be on different cores.
		1. Since calls are blocking, therefore one client thread can only use one call at a time.
		1. As different clients will be making calls, they will be making calls through different cores.
		1. Above points imply that multithreading in server is implicit.
