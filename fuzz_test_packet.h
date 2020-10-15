#ifndef __FUZZ_TEST_PACKET_H__
#define __FUZZ_TEST_PACKET_H__

#include "fuzz_test_helper.h"
#include "fuzz_test_script.h"
#include "logging.h"
#include "packet_checksum.h"
#include "packet_parser.h"
#include "packet_to_string.h"
#include "run.h"
#include "script.h"
#include "socket.h"
#include "tcp_options.h"

#define MAX_OPTIONS_LENGTH 40
#define MAX_PACKET_DATA_LENGTH 10000
#define IP_HEADER_LENGTH 20
#define TCP_OFFSET_FIELD_INDEX 32

extern u32 last_packet_data_length;
extern u32 last_received_packet_ack_seq;
extern u32 last_received_packet_ecr;
extern bool is_received_packet_has_timestamp;
extern bool is_validate_outbound_data_error;

struct packet *fuzz_to_packet(bool is_fuzz_options,
							  bool is_fuzz_data);

extern u32 packet_seq;
extern u32 packet_ack_seq;
extern struct state *state;

extern u32 outbound_packet_seq;
extern u32 outbound_packet_ack_seq;

void *validate_outbound_packet(void *p);

void start_validate_outbound_thread(struct packet *packet);

void stop_validate_outbound_thread();

void set_max_times_and_set_running(int t);

#endif