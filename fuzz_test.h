#ifndef __FUZZ_TEST_H__
#define __FUZZ_TEST_H__

#include "fuzz_test_helper.h"
#include "fuzz_test_packet.h"
#include "fuzz_test_result.h"
#include "fuzz_test_script.h"
#include "signal.h"

#define MAX_LOOP_INDEX 10000000u

u64 total_write_num;

struct state *state = NULL;
struct netdev *netdev = NULL;
struct event *event = NULL;

void reinit_fuzzing_variable() {
	fuzz_loop_index = 0;
	fuzz_file_index = 0;
	packet_seq = 1;
	packet_ack_seq = 1;

	outbound_packet_seq = 1;
	outbound_packet_ack_seq = 1;
	total_write_num = 0;

	last_received_packet_ack_seq = 0;
	last_received_packet_ecr = 0;
	is_received_packet_has_timestamp = false;
}


struct predefined_syscall_event {
	int event_line_number;
	void (*run_fuzz_syscall_func)();
	struct event *e;
};


extern void init_in_main(void);
extern void start_forkserver(void);
extern void PrintCoverage(void);
extern void UpdateCoverage(void);
extern void ClearArray(void);

void update_state_and_event(struct event *new_event);
void inject_fuzz_normal_packet();
void inject_fuzz_error_packet();

#endif