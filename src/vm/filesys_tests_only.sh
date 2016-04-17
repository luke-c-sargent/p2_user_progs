#!/bin/bash
make clean
make
cd build

make tests/filesys/base/lg-create.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/lg-full.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/lg-random.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/lg-seq-block.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/lg-seq-random.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/sm-create.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/sm-full.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/sm-random.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/sm-seq-block.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/sm-seq-random.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/syn-read.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/syn-remove.result VERBOSE=1 | grep 'pass test\|FAIL test'
make tests/filesys/base/syn-write.result VERBOSE=1 | grep 'pass test\|FAIL test'
