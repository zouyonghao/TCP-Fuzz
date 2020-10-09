#include "fuzz_test_script.h"
#include "fuzz_test_helper.h"
#include "logging.h"
#include "packet_to_string.h"

char **fuzz_scripts;
char *fuzz_script;
int fuzz_script_index;

void reinit_fuzzing_scripts() {
	/**
	 * NOTE: the string for record fuzz packets and syscalls
	 */
	fuzz_script = malloc(3000);
	fuzz_script_index = 0;
	memset(fuzz_script, 0, 3000);
	
	fuzz_scripts = malloc(1000 * sizeof(char *));
	memset(fuzz_scripts, 0, 1000 * sizeof(char *));
}

void add_char_to_fuzz_script(char content) {
	fuzz_script[fuzz_script_index] = content;
	fuzz_script_index++;
}

void add_content_to_fuzz_script(char *content) {
	for (int i = 0; i < strlen(content); i++) {
		fuzz_script[fuzz_script_index] = content[i];
		fuzz_script_index++;
	}
}

void add_packet_to_fuzz_script(struct packet *p) {
	char *dump = NULL;
	char *error = NULL;
	packet_to_string(p, DUMP_SHORT, &dump, &error);

	if (error != NULL) {
		add_content_to_fuzz_script("// ");
		add_content_to_fuzz_script(error);
		fuzz_script[fuzz_script_index] = '\n';
		fuzz_script_index++;
	}

	add_content_to_fuzz_script("+0 < ");
	add_content_to_fuzz_script(dump);
	fuzz_script[fuzz_script_index] = '\n';
	fuzz_script_index++;
	DEBUG_FUZZP("dump is %s \n", dump);
	DEBUG_FUZZP("error is %s \n", error);
	free(dump);
	free(error);
}

void update_fuzz_scripts() {
	if (fuzz_script_index < 1) {
		return;
	}
	fuzz_script[fuzz_script_index] = '\0';
	DEBUG_FUZZP("fuzz_script is %s\n", fuzz_script);
	fuzz_scripts[fuzz_loop_index] = strdup(fuzz_script);
	fuzz_script_index = 0;
	fuzz_loop_index++;
}