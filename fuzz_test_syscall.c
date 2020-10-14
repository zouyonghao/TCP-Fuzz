#include "fuzz_test_syscall.h"
#include "run_system_call.h"

extern struct state *state;
extern struct event *event;
extern int last_write_num;

void add_read_to_fuzz_script(int read_num) {
	char read_string[100];
	sprintf(read_string, "+0.1 read(4, ..., %d) =  %d\n", read_num, read_num);
	add_content_to_fuzz_script((char *) &read_string);
}

void add_write_to_fuzz_script(int write_num) {
	char read_string[100];
	sprintf(read_string, "+0.1 write(4, ..., %d) =  %d\n", write_num, write_num);
	add_content_to_fuzz_script((char *) &read_string);
}

void add_ioctl_siocinq_to_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0.1 ioctl(4, SIOCINQ, [0]) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}

void add_fcntl_getfl_to_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 fcntl(4, F_GETFL) = 2050\n");
	add_content_to_fuzz_script((char *) &read_string);
}

void add_fcntl_setfl_nonblock_to_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 fcntl(4, F_SETFL, O_RDWR|O_NONBLOCK) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}

void add_fcntl_setfl_block_to_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 fcntl(4, F_SETFL, O_RDWR) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}
void add_setsockopt1_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 setsockopt(4, SOL_SOCKET, SO_REUSEADDR, [1], 4) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}
void add_setsockopt2_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 setsockopt(4, SOL_SOCKET, SO_KEEPALIVE, [1], 4) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}
void add_setsockopt3_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 setsockopt(4, IPPROTO_TCP, TCP_KEEPIDLE, [5], 4) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}
void add_setsockopt4_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 setsockopt(4, IPPROTO_TCP, TCP_KEEPINTVL, [2], 4) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}
void add_setsockopt5_fuzz_script() {
	char read_string[100];
	sprintf(read_string, "+0 setsockopt(4, IPPROTO_TCP, TCP_KEEPCNT, [2], 4) = 0\n");
	add_content_to_fuzz_script((char *) &read_string);
}

// TODO: optimize these duplicated functions
void run_fuzz_syscall_setsockopt1_event() {
	DEBUG_FUZZP("===========run_fuzz_syscall_setsockopt1_event=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_setsockopt1_fuzz_script();
}

void run_fuzz_syscall_setsockopt2_event() {
	DEBUG_FUZZP("===========run_fuzz_syscall_setsockopt2_event=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_setsockopt2_fuzz_script();
}

void run_fuzz_syscall_setsockopt3_event() {
	DEBUG_FUZZP("===========run_fuzz_syscall_setsockopt3_event=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_setsockopt3_fuzz_script();
}

void run_fuzz_syscall_setsockopt4_event() {
	DEBUG_FUZZP("===========run_fuzz_syscall_setsockopt4_event=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_setsockopt4_fuzz_script();
}

void run_fuzz_syscall_setsockopt5_event() {
	DEBUG_FUZZP("===========run_fuzz_syscall_setsockopt5_event=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_setsockopt5_fuzz_script();
}

void run_fuzz_syscall_fcntl_setfl_block_event() {
	// printf("===========fcntl setfl block syscall=========\n");
	// TODO: do not set block because of inject error packet
	// run_system_call_event(state, event, event->event.syscall);
	// add_fcntl_setfl_block_to_fuzz_script();
}

void run_fuzz_syscall_fcntl_setfl_nonblock_event() {
	DEBUG_FUZZP("===========fcntl setfl nonblock syscall=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_fcntl_setfl_nonblock_to_fuzz_script();
}

void run_fuzz_syscall_fcntl_getfl_event() {
	DEBUG_FUZZP("===========fcntl getfl syscall=========\n");
	run_system_call_event(state, event, event->event.syscall);
	add_fcntl_getfl_to_fuzz_script();
}

void run_fuzz_syscall_ioctl_siocinq_event() {
	DEBUG_FUZZP("===========ioctl siocinq syscall=========\n");
	usleep(20000); // sleep 0.1 second
	run_system_call_event(state, event, event->event.syscall);
	add_ioctl_siocinq_to_fuzz_script();
}

void run_fuzz_syscall_write_event() {
	// TODO: fix write syscall seq number
	DEBUG_FUZZP("===========write syscall=========\n");
	struct expression_list *argument = event->event.syscall->arguments;
	for (int i = 0; i < 2; i++) {
		argument = argument->next;
	}
	last_write_num = read_int() % MAX_WRITE_READ_NUM;
	last_write_num = last_write_num < 0 ? -last_write_num : last_write_num;
	// printf("\033[33mwrite num is %d\033[0m\n", last_write_num);
	argument->expression->value.num = last_write_num;
	event->event.syscall->result->value.num = last_write_num;

	usleep(20000); // sleep 0.1 second
	run_system_call_event(state, event, event->event.syscall);
	add_write_to_fuzz_script(last_write_num);
}

void run_fuzz_syscall_read_event() {
	DEBUG_FUZZP("===========read syscall=========\n");
	struct expression_list *argument = event->event.syscall->arguments;

	for (int i = 0; i < 2; i++) {
		argument = argument->next;
	}
	int read_num = read_int();
	DEBUG_FUZZP("\033[33mread num is %d\033[0m\n", read_num);
	argument->expression->value.num = read_num;
	event->event.syscall->result->value.num = read_num;

	/**
	 * TODO: use packetdrill time system
	 */
	usleep(20000); // sleep 0.1 second
	run_system_call_event(state, event, event->event.syscall);
	add_read_to_fuzz_script(read_num);
}