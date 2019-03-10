#!/bin/bash

./strset1 2> my1.err
echo $?
diff -s my1.err strset_test1.err
./strset2a 2> my2a.err
echo $?
diff -s my2a.err strset_test2a.err
./strset2b 2> my2b.err
echo $?
diff -s my2b.err strset_test2b.err
