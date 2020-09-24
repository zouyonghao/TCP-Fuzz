#ifndef __FUZZ_TEST_RESULT_H__
#define __FUZZ_TEST_RESULT_H__

#include "types.h"

#include "fuzz_test_helper.h"
#include "packet_to_string.h"

extern char *fuzz_result;
extern unsigned int fuzz_result_index;
extern char **fuzz_results;

void update_fuzz_results();

void add_content_to_fuzz_result(char *content);

void add_packet_dump_to_fuzz_result(struct packet *p);

void add_syscall_result_to_fuzz_result(const char *syscall_name, int actual);

void add_read_result_to_fuzz_result(char *buf, int count);

void reinit_fuzzing_results();

#endif