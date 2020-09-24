#ifndef __FUZZ_TEST_HELPER_H__
#define __FUZZ_TEST_HELPER_H__

#include "types.h"
#include "stdlib.h"
#include "run.h"

#include "fuzz_test_helper.h"

extern unsigned int fuzz_file_index;
extern unsigned long fuzz_file_length;
extern char *fuzz_file_string;
extern char *fuzz_file_name;

u8 *push_data(u8 **data, u8 value, int *size, int *capacity);

u8 read_byte();

u32 read_int();

void file_to_string(const char *filename);

extern int fuzz_loop_index;

/**
 * @return index of the first different character, -1 for equal string
 */
int fuzz_test_result_compare(char **a, char **b);

struct event *new_event(enum event_t type);

#endif