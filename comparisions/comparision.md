## Comparision for insertion
Now do see that this is just for insertion, it may be completely possible that for rest of the 4 operations, the rankings may change completely. Like I know that AVL is faster than RB in searches.<br/>

* **B**<br/>
With min_degree = 3
```bash
aryam@comp:~/Desktop/spp_proj/b$ time ./a.out 10000000

real	0m16.853s
user	0m16.332s
sys	0m0.483s
```
With min_degree = 16 *(trial-error by hand optimisation)*
```bash
aryam@comp:~/Desktop/spp_proj/b$ time ./a.out 10000000

real	0m10.314s
user	0m9.992s
sys	0m0.299s
```

* **AVL**
```bash
aryam@comp:~/Desktop/spp_proj/avl$ time ./a.out 10000000

real	0m49.381s
user	0m48.575s
sys	0m0.707s
```

* **RB**
```bash
aryam@comp:~/Desktop/spp_proj/rb$ time ./a.out 10000000

real	0m23.164s
user	0m22.644s
sys	0m0.475s
```

* **B+**
With numberOfPointers = 16
```bash
aryam@comp:~/Desktop/spp_proj/bplus$ time ./a.out 10000000

real	0m19.916s
user	0m18.877s
sys	0m0.994s
```