make -C ../.. clean
make CC=clang -C ../.. -f Makefile-mtcp fuzz_test_asan_mtcp