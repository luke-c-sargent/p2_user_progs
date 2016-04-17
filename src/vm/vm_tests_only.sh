#!/bin/bash
make clean
make
cd build

make tests/vm/pt-grow-stack.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-grow-bad.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-big-stk-obj.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-bad-addr.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-bad-read.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-write-code.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-write-code2.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/pt-grow-stk-sc.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-linear.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-parallel.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-merge-seq.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-merge-par.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-merge-stk.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-merge-mm.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/vm/page-shuffle.result VERBOSE=1 | grep 'pass test\|FAIL test'
