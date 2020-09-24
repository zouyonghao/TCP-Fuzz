#ifndef __FUZZ_TEST_HEADER__
#define __FUZZ_TEST_HEADER__

#include "packet_to_string.h"
#include "fuzz_test_helper.h"
#include "fuzz_test_result.h"

extern bool checker;
extern bool packetdrill;

extern bool read_result_validate_error;

extern char *fuzz_result;
extern unsigned int fuzz_result_index;
extern int fuzz_loop_index;
extern char **fuzz_results;
extern bool is_fuzz_start;

#endif