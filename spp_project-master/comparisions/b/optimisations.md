## Optimisations in b-tree
1. Use `memmove` for moving data in array nodes b-tree
2. The big O performance ( O(logN) ) is the same for both. Moreover, if you do binary search inside each B-Tree node, you will even end up with the same number of comparisons as in a BST (it is a nice math exercise to verify this). If the B-Tree node size is sensible (1-4x cache line size), linear searching inside each node is still faster because of the hardware prefetching. You can also use SIMD instructions for comparing basic data types (e.g. integers). src: https://stackoverflow.com/questions/647537/b-tree-faster-than-avl-or-redblack-tree
3. see b vs b+
4. Store first 2 char's of each value in the node iteslf.