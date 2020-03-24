## Optimisations in b-tree
1. Use `memmove` for moving data in array nodes b-tree
2. The big O performance ( O(logN) ) is the same for both. Moreover, if you do binary search inside each B-Tree node, you will even end up with the same number of comparisons as in a BST (it is a nice math exercise to verify this). If the B-Tree node size is sensible (1-4x cache line size), linear searching inside each node is still faster because of the hardware prefetching. You can also use SIMD instructions for comparing basic data types (e.g. integers). src: https://stackoverflow.com/questions/647537/b-tree-faster-than-avl-or-redblack-tree
3. Store first 2 char's of each value in the node iteslf.
4. Use nice values for performance gains.
5. Use reader-writer lock type of something in the interface to the threads.
6. All comments starting with *OPT:* are areas for potential optimizations.
7. **Improve** multithreading.


## Todo
1. Testing insert left. Not tested after updating it to support overwrites. If not much time, then just check for the correctness of nC values.