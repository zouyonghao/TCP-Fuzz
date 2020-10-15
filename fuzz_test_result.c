#include "fuzz_test_result.h"

extern bool checker;
extern bool packetdrill;
extern bool is_fuzz_start;
extern int fuzz_loop_index;

/**
 *	fuzz_result
 *		loop_index -> fuzz_result
 */
char *fuzz_result;
unsigned int fuzz_result_index;
char **fuzz_results;

void reinit_fuzzing_results() {
	/**
	 * NOTE: the string for compare result
	 */
	fuzz_result_index = 0;
	fuzz_result = malloc(30000);
	memset(fuzz_result, 0, 30000);
	
	fuzz_results = malloc(1000 * sizeof(char *));
	memset(fuzz_results, 0, 1000 * sizeof(char *));
}

void update_fuzz_results() {
	if (fuzz_result_index < 1) {
		return;
	}
	fuzz_result[fuzz_result_index] = '\0';
	fuzz_results[fuzz_loop_index] = strdup(fuzz_result);
	fuzz_result_index = 0;
}

void add_content_to_fuzz_result(char *content) {
	if (!is_fuzz_start) {
		return;
	}
	for (int i = 0; i < strlen(content); i++) {
		fuzz_result[fuzz_result_index] = content[i];
		fuzz_result_index++;
	}
}

void add_packet_dump_to_fuzz_result(struct packet *p) {
	if (!is_fuzz_start) {
		return;
	}
	if (fuzz_result == NULL) {
		return;
	}
	if (checker && !packetdrill) {
		return;
	}
	char *dump = NULL;
	char *error = NULL;
	packet_to_string(p, DUMP_SHORT, &dump, &error);
	char *sub = strstr(dump, "win");
	if (sub == NULL) {
		sub = "";
	}
	for (int i = 0; i < strlen(dump) - strlen(sub); i++) {
		fuzz_result[fuzz_result_index] = dump[i];
		fuzz_result_index++;
	}
	fuzz_result[fuzz_result_index] = '\n';
	fuzz_result_index++;
	free(dump);
	free(error);
}

void add_syscall_result_to_fuzz_result(const char *syscall_name, int actual) {
	if (!is_fuzz_start) {
		return;
	}
	if (fuzz_result == NULL) {
		return;
	}
	if (checker && !packetdrill) {
		return;
	}

	if (strcmp(syscall_name, "read") == 0 ||
		strcmp(syscall_name, "write") == 0 ||
		strcmp(syscall_name, "ioctl") == 0 ||
		strcmp(syscall_name, "fcntl") == 0 ||
		strcmp(syscall_name, "setsockopt") == 0) {
		char actual_string[100];
		sprintf(actual_string, "%s : %d\n", syscall_name, actual);
		add_content_to_fuzz_result(actual_string);
		// sprintf(actual_string, "fuzz_loop_index : %d\n", fuzz_loop_index);
		// add_content_to_fuzz_result(actual_string);
	}
}

void add_read_result_to_fuzz_result(char *buf, int count) {
	if (!is_fuzz_start) {
		return;
	}
	if (fuzz_result == NULL) {
		return;
	}
	if (checker && !packetdrill) {
		return;
	}
	if (count < 1) {
		return;
	}
	int i = 0;
	add_content_to_fuzz_result("read_result :");
	for (i = 0; i < count; i++) {
		fuzz_result[fuzz_result_index] = buf[i] + '0';
		fuzz_result_index++;
	}
	fuzz_result[fuzz_result_index++] = '\n';
	// update_fuzz_results();
}