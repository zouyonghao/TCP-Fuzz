#include "fuzz_test.h"
#include "fuzz_test_script.h"
#include "fuzz_test_syscall.h"
#include <error.h>

bool packetdrill = false;

bool checker = false;
bool read_result_validate_error = false;
bool is_fuzz_start = false;

int last_write_num = 0;

struct config config;

bool record_coverage = true;

struct predefined_syscall_event predefined_syscall_events[] = {
		// syscall read
		{30, run_fuzz_syscall_read_event, NULL},
		// syscall write
		{31, run_fuzz_syscall_write_event, NULL},
		// syscall close
		// {32, NULL, NULL},
		// syscall ioctl SIOCINQ
		{34, run_fuzz_syscall_ioctl_siocinq_event, NULL},
		// syscall fcntl getfl
		{36, run_fuzz_syscall_fcntl_getfl_event, NULL},
		// syscall fcntl setfl nonblock
		{37, run_fuzz_syscall_fcntl_setfl_nonblock_event, NULL},
		// syscall fcntl setfl block
		{38, run_fuzz_syscall_fcntl_setfl_block_event, NULL},
		// syscall setsockopt SOL_SOCKET SO_REUSEADDR
		{40, run_fuzz_syscall_setsockopt1_event, NULL},
		// syscall setsockopt SOL_SOCKET SO_KEEPALIVE
		{41, run_fuzz_syscall_setsockopt2_event, NULL},
		// syscall setsockopt IPPROTO_TCP TCP_KEEPIDLE
		{42, run_fuzz_syscall_setsockopt3_event, NULL},
		// syscall setsockopt IPPROTO_TCP TCP_KEEPINTVL
		{43, run_fuzz_syscall_setsockopt4_event, NULL},
		// syscall setsockopt IPPROTO_TCP TCP_KEEPCNT
		{44, run_fuzz_syscall_setsockopt5_event, NULL}};

void do_some_fuzz(int fuzz_loop) {
	// let's do some fuzzing
	is_fuzz_start = true;
	for (int i = 0; i < fuzz_loop; i++) {

		if (!config.is_no_fuzz_packet) {
			DEBUG_FUZZP("fuzz_file_index: %d\n", fuzz_file_index);
			/**
			 * NOTE: actually this packet can also have errors
			 * 		(question: so why this method called "normal"?)
			 * 		(answer: historical reason)
			 */
			inject_fuzz_normal_packet();
			/**
			 * NOTE: does error packet should increase the packet_seq?
			 */
			packet_seq += last_packet_data_length;
			update_fuzz_scripts();

			u8 tmp = read_byte();
			DEBUG_FUZZP("fuzz_file_index: %d\n", fuzz_file_index);
			if (tmp < 50) {
				// skip
			} else if (tmp < 150) {
				DEBUG_FUZZP("inject fuzz error packet\n");
				add_content_to_fuzz_script("// inject fuzz error packet\n");
				inject_fuzz_error_packet();
			} else {
				// inject duplicated packets
				DEBUG_FUZZP("inject fuzz dup packets\n");
				add_content_to_fuzz_script("// inject fuzz dup packets\n");
				inject_fuzz_normal_packet();
				inject_fuzz_normal_packet();
				inject_fuzz_normal_packet();
				inject_fuzz_normal_packet();
			}

			update_fuzz_scripts();

			if (record_coverage) {
				UpdateCoverage();
			}
			// PrintCoverage();
		}

		if (!config.is_no_fuzz_syscall) {
			for (int j = 0; j < sizeof(predefined_syscall_events) / sizeof(struct predefined_syscall_event); j++) {
				if (predefined_syscall_events[j].e == NULL) {
					continue;
				}
				DEBUGP("fuzz_file_index: %d\n", fuzz_file_index);
				u8 tmp = read_byte();
				if (tmp % 100 < 50) {
					update_state_and_event(predefined_syscall_events[j].e);
					predefined_syscall_events[j].run_fuzz_syscall_func();
					state->num_events++;
					if (record_coverage) {
						UpdateCoverage();
					}
					// PrintCoverage();
					update_fuzz_scripts();
					update_fuzz_results();
				}
			}
		}
	}
	is_fuzz_start = false;
}

void fuzz_three_way_handshake_packets() {
	DEBUG_FUZZP("%s\n", "fuzz_threee_way_handshake_packets");
	/**
	 * TODO: make the generated packet empty data
	 */
	struct packet *p;
	if (read_byte() % 100 < 50) {
		p = fuzz_to_packet(true, false);
	} else {
		p = fuzz_to_packet(true, true);
	}
	p->tcp->syn = event->event.packet->tcp->syn;
	p->tcp->ack = event->event.packet->tcp->ack;
	packet_free(event->event.packet);
#ifdef _TLDK
	char *error;
	find_tcp_timestamp(p, &error);
	if (p->tcp_ts_ecr != NULL && *(p->tcp_ts_ecr) != 0) {
		/**
		 * NOTE: RST is sent if ts ecr is not zero in Linux but TLDK.
		 * This is error 15
		 */
		printf("RST is sent if ts ecr is not zero in Linux but TLDK\n");
		exit(0);
	}
	if (p->tcp_ts_ecr != NULL &&
		p->tcp_ts_val != NULL &&
		*(p->tcp_ts_ecr) == 0 &&
		*(p->tcp_ts_val) == 0) {
		/**
		 * NOTE: packets have timestamp option 
		 *       if ts val and ecr are both zero in Linux but TLDK.
		 * This is error 19.
		 */
		printf("packets have timestamp option \n");
		printf("if ts val and ecr are both zero in Linux but TLDK\n");
		exit(0);
	}
#endif
	event->event.packet = p;
}

void print_packet(struct packet *p) {
	char *dump = NULL;
	char *error = NULL;
	packet_to_string(p, DUMP_FULL, &dump, &error);
	printf("%s\n", dump);
}

void my_run_script() {

	if (record_coverage) {
		ClearArray();
		PrintCoverage();
	}

	reinit_fuzzing_variable();
	reinit_fuzzing_results();
	reinit_fuzzing_scripts();

	char *error = NULL;

	/**
	 * NOTE: fuzz loop index for recognizing duplicate cases
	 */
	fuzz_loop_index = 0;

	// outbound event
	const int outbound_event_line_number = 20;
	struct event *outbound_event = NULL;

	// syscall close event
	const int syscall_close_event_number = 32;
	struct event *syscall_close_event = NULL;

	/**
	 * NOTE: here still run some scripts
	 *       including three-way handshake packets
	 *       which should be refactored later
	 *       to get a more general fuzzing process.
	 */
	while (1) {
		if (get_next_event(state, &error))
			die("%s", error);
		event = state->event;
		if (event == NULL)
			break;

		if (event->line_number == outbound_event_line_number) {
			outbound_event = event;
			if (!config.is_no_fuzz_receive_packets_in_background) {
				start_validate_outbound_thread(outbound_event->event.packet);
			}
			continue;
		}

		if (event->line_number == syscall_close_event_number) {
			syscall_close_event = event;
			continue;
		}

		bool get_event = false;
		for (int i = 0; i < sizeof(predefined_syscall_events) / sizeof(struct predefined_syscall_event); i++) {
			if (event->line_number == predefined_syscall_events[i].event_line_number) {
				predefined_syscall_events[i].e = event;
				get_event = true;
				break;
			}
		}
		if (get_event) {
			continue;
		}

		switch (event->type) {
			case PACKET_EVENT:
				if (event->event.packet->direction == DIRECTION_INBOUND && !config.is_no_fuzz_packet) {
					fuzz_three_way_handshake_packets();
				}
				run_local_packet_event(state, event, event->event.packet);
				if (event->event.packet->direction == DIRECTION_INBOUND) {
					printf("INBOUND: ");
					add_packet_to_fuzz_script(event->event.packet);
				}
				print_packet(event->event.packet);
				break;
			case SYSCALL_EVENT:
				run_system_call_event(state, event,
									  event->event.syscall);
				break;
			case COMMAND_EVENT:
			case CODE_EVENT:
			case INVALID_EVENT:
			case NUM_EVENT_TYPES:
				assert(!"bogus type");
				break;
				/* We omit default case so compiler catches missing values. */
		}
		state->num_events++;
	}

	do_some_fuzz(10);

	/**
	 * TODO: try to write a universal checker
	 */
	// checker = true;

	DEBUG_FUZZP("\033[33mrun_script: checking\033[0m\n");
	for (int i = 0; i < 3; i++) {
		if (!config.is_no_fuzz_packet) {
			event = new_event(PACKET_EVENT);
			event->time_type = ANY_TIME;
			// send inbound event
			event->event.packet = fuzz_to_packet(false, true);
			update_state_and_event(event);

			DEBUG_FUZZP("==========fuzz inbound packet==========\n");
			run_local_packet_event(state, event, event->event.packet);
			state->num_events++;
			add_packet_to_fuzz_script(event->event.packet);
			update_fuzz_scripts();
			free(event);

			DEBUG_FUZZP("packet_seq = %d\n", packet_seq);
		}

		if (!config.is_no_fuzz_syscall) {
			DEBUG_FUZZP("===========read syscall=========\n");
			struct event *syscall_read_event = predefined_syscall_events[0].e;
			struct expression_list *argument = syscall_read_event->event.syscall->arguments;
			for (int j = 0; j < 2; j++) {
				argument = argument->next;
			}
			int read_num = 20;
			DEBUG_FUZZP("read num is %d\n", read_num);
			argument->expression->value.num = read_num;
			syscall_read_event->event.syscall->result->value.num = read_num;
			update_state_and_event(syscall_read_event);
			usleep(20000);
			run_system_call_event(state, syscall_read_event, syscall_read_event->event.syscall);
			state->num_events++;
			add_read_to_fuzz_script(read_num);
			update_fuzz_scripts();
		}
	}

	// sleep a while to receive more packets
	// if (!config.is_no_fuzz_receive_packets_in_background) {
	// 	usleep(100000);
	// 	stop_validate_outbound_thread();
	// } else {
	// 	set_max_times_and_set_running(25);
	// 	validate_outbound_packet(outbound_event->event.packet);
	// }

	DEBUG_FUZZP("===========close syscall=========\n");
	update_state_and_event(syscall_close_event);
	run_system_call_event(state, syscall_close_event, syscall_close_event->event.syscall);
	state->num_events++;
	// UpdateCoverage();
	PrintCoverage();

	update_fuzz_scripts();
	update_fuzz_results();

	printf("Fuzz after close!\n");
	/**
	 * NOTE: Fuzz after close.
	 */
	do_some_fuzz(3);

	// sleep a while to receive more packets
	if (!config.is_no_fuzz_receive_packets_in_background) {
		usleep(1000000);
		stop_validate_outbound_thread();
	} else {
		set_max_times_and_set_running(25);
		validate_outbound_packet(outbound_event->event.packet);
	}

	DEBUG_FUZZP("fuzz_scripts is :\n");
	for (int i = 0; i <= fuzz_loop_index; i++) {
		if (fuzz_scripts[i] != NULL) {
			DEBUG_FUZZP("%s", fuzz_scripts[i]);
		}
	}
	DEBUG_FUZZP("fuzz_results is :\n");
	for (int i = 0; i <= fuzz_loop_index; i++) {
		if (fuzz_results[i] != NULL) {
			DEBUG_FUZZP("%s", fuzz_results[i]);
		}
	}

	state_free(state);

	DEBUG_FUZZP("run_script: done running\n");
}

void validate_packet(struct packet *p) {
	// int status;
	// char *dump = NULL;
	// char *error = NULL;
	/* Test a DUMP_SHORT dump */
	// status = packet_to_string(p, DUMP_FULL, &dump, &error);
	// assert(status == STATUS_OK);
	// assert(error == NULL);
	// printf("dump = '%s'\n", dump);
	// free(dump);
}

void inject_error_to_packet(struct packet *p) {
	u8 tmp = read_byte();
	char ip_error_string[50];

	if (tmp < 50) {
		ip_error_string[0] = 0;
	} else if (tmp < 100) {
		p->ipv4->version = read_byte();
		sprintf(ip_error_string, "// set ip version to 0x%02x\n", p->ipv4->version);
	}
	if (tmp < 150) {
		p->ipv4->ihl = read_byte();
		sprintf(ip_error_string, "// set ip ihl to 0x%02x\n", p->ipv4->ihl);
	} else if (tmp < 200) {
		p->ipv4->check = 0;
		sprintf(ip_error_string, "// set ip checksum to 0x%04x\n", p->ipv4->check);
	} else {
		u16 fuzz_bytes = read_byte();
		fuzz_bytes <<= 8u;
		fuzz_bytes |= read_byte();
		p->ipv4->frag_off = fuzz_bytes;
		sprintf(ip_error_string, "// set ip frag_off to 0x%04x\n", p->ipv4->frag_off);
	}

	add_content_to_fuzz_script(ip_error_string);

	tmp = read_byte();
	char tcp_error_string[50];
	if (tmp < 50) {
		tcp_error_string[0] = 0;
	} else if (tmp < 100) {
		p->tcp->seq = read_int();
		/**
		 * NOTE: as seq changed, we should make the content fit the seq
		 * @see fuzz_to_packet generate data
		 */
		u32 header_length = p->buffer_bytes - last_packet_data_length;
		u32 currentSeq = ntohl(p->tcp->seq);
		for (u32 i = 0; i < last_packet_data_length; i++) {
			p->buffer[header_length + i] = (currentSeq + i) % 10;
		}
		sprintf(tcp_error_string, "// set tcp seq to 0x%08x\n", p->tcp->seq);
	} else if (tmp < 150) {
		p->tcp->ack_seq = read_int();
		sprintf(tcp_error_string, "// set tcp ack seq to 0x%08x\n", p->tcp->ack_seq);
	} else if (tmp < 200) {
		((char *) p->tcp)[13] = read_byte();
		sprintf(tcp_error_string, "// set tcp flags to 0x%02x\n", ((u8 *) p->tcp)[13]);
	} else {
		p->tcp->doff = read_byte() & 0xffu;
		sprintf(tcp_error_string, "// set tcp offset to 0x%02x\n", p->tcp->doff);
	}

	add_content_to_fuzz_script(tcp_error_string);
}

void inject_fuzz_normal_packet() {
	event = new_event(PACKET_EVENT);
	event->time_type = ANY_TIME;
	event->event.packet = fuzz_to_packet(true, true);
	update_state_and_event(event);

	validate_packet(event->event.packet);

	if (read_byte() < 50) {
		inject_error_to_packet(event->event.packet);
	}

	run_local_packet_event(state, event, event->event.packet);
	add_packet_to_fuzz_script(event->event.packet);
	/**
	 *  NOTE: care this memory
	 */
	// free(event);
	state->num_events++;
}

void inject_fuzz_error_packet() {
	event = new_event(PACKET_EVENT);
	event->time_type = ANY_TIME;
	// send inbound event
	event->event.packet = fuzz_to_packet(true, true);
	update_state_and_event(event);

	validate_packet(event->event.packet);

	inject_error_to_packet(event->event.packet);

	print_packet(event->event.packet);
	add_packet_to_fuzz_script(event->event.packet);
	run_local_packet_event(state, event, event->event.packet);
	state->num_events++;
}

void update_state_and_event(struct event *new_event) {
	if (state->event != NULL) {
		state->script_last_time_usecs = state->event->time_usecs;
		state->last_event = state->event;
	}
	event = new_event;
	state->event = event;
	// adjust_relative_event_times(state, event);
}

void print_scripts(u32 max_index) {
	fprintf(stderr, "\nMin error sequence: %d\n", max_index);
	for (int i = 0; i <= max_index && i <= fuzz_loop_index; i++) {
		if (fuzz_scripts[i] != NULL) {
			fprintf(stderr, "%s", fuzz_scripts[i]);
		}
	}
	fflush(stderr);
}

int main(int argc, char *argv[]) {
	init_in_main();
	set_default_config(&config);
	/* Get command line options and list of test scripts. */
	char **arg = parse_command_line_options(argc, argv, &config);

	file_to_string(fuzz_file_name);

	struct script script;
	char *current_script_path;
	if (read_byte() % 100 < 50) {
		current_script_path = "./fuzz_test_pkt_accept.pkt";
	} else {
		printf("use fuzz_test_pkt_connect.pkt\n");
		current_script_path = "./fuzz_test_pkt_connect.pkt";
	}
	if (parse_script_and_set_config(argc, argv, &config, &script,
									current_script_path, NULL)) {
		exit(EXIT_FAILURE);
	}

	/**
	 * NOTE: setting this ip for multi process tests
	 * When test tldk, ip never changed
	 * When test linux, ip should be different for different process
	 */
	char *real_local_ip = strdup(config.live_local_ip_string);
	char *real_remote_ip = strdup(config.live_remote_ip_string);

	// 1. run with tldk
	strcpy(config.live_local_ip_string, "192.168.0.2");
	strcpy(config.live_remote_ip_string, "192.0.2.1");
	finalize_config(&config);

	// set_scheduling_priority();
	script_path = config.script_path;
	struct netdev *nd = so_netdev_new(&config);
	state = state_new(&config, &script, nd);
	state->so_instance = so_instance_new();
	so_instance_init(state->so_instance, &config, &script, state);
	signal(SIGPIPE, SIG_IGN); /* ignore EPIPE */
	state->live_start_time_usecs = schedule_start_time_usecs(state);

	my_run_script();
	char **step1_results = fuzz_results;
	u32 step1_last_received_ack = last_received_packet_ack_seq;
	u32 step1_last_received_ecr = last_received_packet_ecr;
	bool step1_is_received_packet_has_timestamp = is_received_packet_has_timestamp;
	DEBUG_FUZZP("Step1 finished!\n");

	if (read_result_validate_error) {
		printf("read result validate error!\n");
		abort();
	}

	// 2. run without tldk
	strcpy(config.live_local_ip_string, real_local_ip);
	strcpy(config.live_remote_ip_string, real_remote_ip);
	// Always use background thread to receive when it is Linux.
	config.is_no_fuzz_receive_packets_in_background = false;
	finalize_config(&config);
	netdev = local_netdev_new(&config);
	state = state_new(&config, &script, netdev);
	state->so_instance = NULL;
	state->live_start_time_usecs = schedule_start_time_usecs(state);

	record_coverage = false;
	my_run_script();

	if (step1_last_received_ack != last_received_packet_ack_seq) {
		printf("last received ack_seq different, step1 is %d, last is %d\n", step1_last_received_ack, last_received_packet_ack_seq);
		print_scripts(MAX_LOOP_INDEX);
		abort();
	}

	if (step1_last_received_ecr != last_received_packet_ecr) {
		printf("last received ecr different!\n");
		print_scripts(MAX_LOOP_INDEX);
		abort();
	}

	if (step1_is_received_packet_has_timestamp != is_received_packet_has_timestamp) {
		printf("One of your tests does not receive timestamp!\n");
		print_scripts(MAX_LOOP_INDEX);
		abort();
	}

	if (read_result_validate_error) {
		printf("read result validate error!\n");
		print_scripts(MAX_LOOP_INDEX);
		abort();
	}

	if (is_validate_outbound_data_error) {
		printf("outbound packet data validate error");
		print_scripts(MAX_LOOP_INDEX);
		abort();
	}

	int fuzz_test_compare_result = fuzz_test_result_compare(step1_results, fuzz_results);
	if (fuzz_test_compare_result != -1) {
		printf("TLDK differs from Linux!\n");
		printf("TLDK is \n");
		for (int i = 0; i < fuzz_loop_index; i++) {
			if (step1_results[i] != NULL) {
				printf("%s", step1_results[i]);
			}
		}
		printf("\nLinux is \n");
		for (int i = 0; i < fuzz_loop_index; i++) {
			if (fuzz_results[i] != NULL) {
				printf("%s", fuzz_results[i]);
			}
		}
		print_scripts(fuzz_test_compare_result);
		fflush(stdout);
		abort();
	}
	// print_scripts(MAX_LOOP_INDEX);

	// 3. run another script
	if (*arg != NULL) {
		current_script_path = *arg;
		if (parse_script_and_set_config(argc, argv, &config, &script,
										current_script_path, NULL))
			exit(EXIT_FAILURE);
		checker = true;
		run_script(&config, &script);
	}

	DEBUG_FUZZP("done\n");
	return 0;
}