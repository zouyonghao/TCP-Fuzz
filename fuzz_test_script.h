#ifndef __FUZZ_TEST_SCRIPT_H__
#define __FUZZ_TEST_SCRIPT_H__

#include "packet.h"
#include "types.h"

extern char **fuzz_scripts;

void add_char_to_fuzz_script(char content);

void add_content_to_fuzz_script(char *content);

void add_packet_to_fuzz_script(struct packet *p);

void update_fuzz_scripts();

void reinit_fuzzing_scripts();

#endif