#include "fuzz_test_helper.h"

/**
 * fuzz file related
 */
unsigned int fuzz_file_index = 0;
unsigned long fuzz_file_length;
char *fuzz_file_string;
char *fuzz_file_name = "fuzz_test_input";

/**
 * fuzz loop related
 */
int fuzz_loop_index = 0;

u8 *push_data(u8 **data, u8 value, int *size, int *capacity) {
	if (*size >= *capacity) {
		(*data) = (u8 *) realloc(*data, (*capacity) * 2 * sizeof(u8));
		if ((*data) == NULL) {
			fprintf(stderr, "realloc failed!\n");
			exit(0);
		}
		(*capacity) *= 2;
	}
	(*data)[*size] = value;
	*size = *size + 1;
	return *data;
}

u8 read_byte() {
	if (fuzz_file_index >= fuzz_file_length) {
		return 0;
	}
	u8 t = fuzz_file_string[fuzz_file_index++];
	return t;
}

u32 read_int() {
	u32 result = read_byte();
	result <<= 8u;
	result |= read_byte();
	result <<= 8u;
	result |= read_byte();
	result <<= 8u;
	result |= read_byte();
	return result;
}

void file_to_string(const char *filename) {
	FILE *f = fopen(filename, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		fuzz_file_length = ftell(f);
		fseek(f, 0, SEEK_SET);
		fuzz_file_string = malloc(fuzz_file_length + 10);
		if (fuzz_file_string) {
			fread(fuzz_file_string, 1, fuzz_file_length, f);
		}
		fclose(f);
	} else {
		printf("fuzz file open fail.\n");
		exit(0);
	}
}

/**
 * @return index of the first different character, -1 for equal string
 */
int fuzz_test_result_compare(char **a, char **b) {
	for (int i = 0; i < 1000; i++) {
		if (a[i] != NULL && b[i] != NULL) {
			if (strcmp(a[i], b[i]) != 0) {
				return i;
			}
		}
		// if ((a[i] == NULL && b[i] != NULL) ||
		// 	(a[i] != NULL && b[i] == NULL)) { // Not both NULL
		// 	return i;
		// }
	}
	return -1;
}

struct event *new_event(enum event_t type) {
	struct event *e = (struct event *) calloc(1, sizeof(struct event));
	e->type = type;
	e->time_usecs_end = NO_TIME_RANGE;
	e->offset_usecs = NO_TIME_RANGE;
	return e;
}