
#include "fuzz_test_packet.h"

// a temp value only to record the data of last packet.
u32 last_packet_data_length;

u32 last_received_packet_ack_seq;
u32 last_received_packet_ecr;

u32 packet_seq = 1;
u32 packet_ack_seq = 1;

u32 outbound_packet_seq = 1;
u32 outbound_packet_ack_seq = 1;
bool is_validate_outbound_running = false;

bool is_validate_outbound_data_error = false;

pthread_t tid;

struct packet *fuzz_to_packet(bool is_fuzz_options,
							  bool is_fuzz_data) {
	int fuzz_data_capacity = 200;
	int fuzz_data_size = 0;

	u8 fixed_data[] = {
			// IPv4
			0x45, 0x00,
			// IP length calculated later
			0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			// ttl, protocol, checksum
			0xff, 0x06, 0x00, 0x00,

			// 0.0.0.0
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00};

	u8 *data = (u8 *) malloc(fuzz_data_capacity * sizeof(u8));

	for (int i = 0; i < sizeof(fixed_data) / sizeof(u8); i++) {
		push_data(&data, fixed_data[i], &fuzz_data_size, &fuzz_data_capacity);
	}

	// source port 16 bit
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);

	// dest port 16 bit
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);

	// seq
	/**
	 * NOTE use `htonl` may be better?
	 */
	push_data(&data, (packet_seq >> 24) & 0xFF, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, (packet_seq >> 16) & 0xFF, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, (packet_seq >> 8) & 0xFF, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, packet_seq & 0xFF, &fuzz_data_size, &fuzz_data_capacity);

	/**
	 * TODO: change ack seq as needed
	 */
	// ack seq
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x01, &fuzz_data_size, &fuzz_data_capacity);

	// offset, reserved, control flag, window size
	// offset calculated later
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x10, &fuzz_data_size, &fuzz_data_capacity);

	push_data(&data, read_byte(), &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, read_byte(), &fuzz_data_size, &fuzz_data_capacity);

	// checksum calculated later
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);

	// urgent pointer (seems not use at all)
	push_data(&data, read_byte(), &fuzz_data_size, &fuzz_data_capacity);
	push_data(&data, read_byte(), &fuzz_data_size, &fuzz_data_capacity);

	u8 tcp_header_length = 20; // TCP header with options
	u8 options_length = 0;
	if (is_fuzz_options) {
		// most 10 options
		char option_count = read_byte() % 11;
		for (int i = 0; i < option_count; i++) {
			char tmp = read_byte();
			u8 kind;
			u8 length;
			DEBUG_FUZZP("tmp in fuzz_option : %d\n", tmp);
			switch (tmp % 17) {
				case 0:
					kind = TCPOPT_EOL;
					length = 1;
					break;
				case 1:
					kind = TCPOPT_NOP;
					length = 1;
					break;
				case 2:
					kind = TCPOPT_MAXSEG;
					length = TCPOLEN_MAXSEG;
					break;
				case 3:
					kind = TCPOPT_WINDOW;
					length = TCPOLEN_WINDOW;
					break;
				case 4:
					/**
					 * TCP Sack-Permitted Option:
					 * 
					 * Kind: 4
					 * 
					 * +---------+---------+
					 * | Kind=4  | Length=2|
					 * +---------+---------+
					 */
					kind = TCPOPT_SACK_PERMITTED;
					length = TCPOLEN_SACK_PERMITTED;
					break;
				case 5:
					/** 
					 * TCP SACK Option:
					 * 
					 * Kind: 5
					 * 
					 * Length: Variable
					 * 
					 *                   +--------+--------+
					 *                   | Kind=5 | Length |
					 * +--------+--------+--------+--------+
					 * |      Left Edge of 1st Block       |
					 * +--------+--------+--------+--------+
					 * |      Right Edge of 1st Block      |
					 * +--------+--------+--------+--------+
					 * |                                   |
					 * /            . . .                  /
					 * |                                   |
					 * +--------+--------+--------+--------+
					 * |      Left Edge of nth Block       |
					 * +--------+--------+--------+--------+
					 * |      Right Edge of nth Block      |
					 * +--------+--------+--------+--------+
				 	 */
					kind = TCPOPT_SACK;
					length = (read_byte() % 4 + 1) * 8;
					break;
				case 6:
					kind = TCPOPT_TIMESTAMP;
					length = TCPOLEN_TIMESTAMP;
					break;
				case 7:
					kind = TCPOPT_FASTOPEN;
					length = read_byte() % MAX_OPTIONS_LENGTH;
					break;
				case 8:
					kind = TCPOPT_EXP;
					length = read_byte() % MAX_OPTIONS_LENGTH;
					break;
				default:
					kind = tmp;
					length = read_byte();
					/**
					 * TODO: this is bug #17 which should be fixed later.
					 */
					if (length == 0) {
						length = 2u;
					}
					break;
			}

			if (options_length + length > MAX_OPTIONS_LENGTH) {
				break;
			}

			options_length += length;
			push_data(&data, kind, &fuzz_data_size, &fuzz_data_capacity);
			if (length > 1) {
				push_data(&data, length, &fuzz_data_size, &fuzz_data_capacity);
			}

			for (int j = 0; j < length - 2; j++) {
				char tmp = read_byte();
				push_data(&data, tmp, &fuzz_data_size, &fuzz_data_capacity);
			}

			char tcp_option_string[100];
			if (DEBUG_FUZZ_LOGGING) {
				sprintf(tcp_option_string, "// fuzz_file_index is %d\n", fuzz_file_index);
				add_content_to_fuzz_script(tcp_option_string);
			}
			sprintf(tcp_option_string, "// kind is %d, length is %d\n", kind, length);
			add_content_to_fuzz_script(tcp_option_string);
		}
	}

	tcp_header_length += options_length;
	u8 padding_length = 4 - options_length % 4;
	/**
	 * NOTE: if padding_length is 4, then we should not pad
	 */
	if (padding_length < 4) {
		for (int i = 0; i < padding_length; i++) {
			// padding
			push_data(&data, 0x00, &fuzz_data_size, &fuzz_data_capacity);
			tcp_header_length++;
		}
	}

	// tcp offset, make reserved same with offset
	u8 tcp_off = tcp_header_length / 4;
	data[32] = tcp_off;
	data[32] <<= 4;
	data[32] |= tcp_off;

	// dynamic data
	if (is_fuzz_data) {
		last_packet_data_length = read_int() % MAX_PACKET_DATA_LENGTH;
	} else {
		last_packet_data_length = 0;
	}
	for (uint i = 0; i < last_packet_data_length; i++) {
		push_data(&data, (packet_seq + i) % 10, &fuzz_data_size, &fuzz_data_capacity);
	}

	// ip length
	u16 ip_length = IP_HEADER_LENGTH + tcp_header_length + last_packet_data_length;
	((struct ipv4 *) data)->tot_len = htons(ip_length);

	struct packet *packet = packet_new(fuzz_data_size);

	/* Populate and parse a packet */
	memcpy(packet->buffer, data, fuzz_data_size);
	free(data);
	char *error = NULL;
	enum packet_parse_result_t result = parse_packet(packet, fuzz_data_size, PACKET_LAYER_3_IP, &error);

	checksum_packet(packet);

	if (error)
		printf("%s\n", error);
	if (result != PACKET_OK) {
		printf("Packet ERROR!\n");
		abort();
	}
	assert(error == NULL);
	packet->direction = DIRECTION_INBOUND;
	return packet;
}

void *validate_outbound_packet(void *p) {
	struct packet *packet = (struct packet *) p;
	DEBUG_FUZZP("validate_outbound_packet\n");
	while (is_validate_outbound_running) {
		struct packet *live_packet = NULL;
		char *error = NULL;
		int live_payload_len = 0;
		struct socket *socket = NULL;

		find_or_create_socket_for_script_packet(state, packet, packet->direction, &socket, &error);
		sniff_outbound_live_packet(state, socket, &live_packet, &error);

		if (live_packet->tcp) {
			socket->last_outbound_tcp_header = *(live_packet->tcp);
			/* Rewrite TCP sequence number */
			if (tcp_convert_seq_number(socket, live_packet, &error))
				goto out;
		}
		live_payload_len = packet_payload_len(live_packet);
		DEBUGP("Sniffed packet with live_payload %d bytes\n", live_payload_len);

		// if (DEBUG_FUZZ_LOGGING) {
		packet_to_string(live_packet, DUMP_FULL, &error, &error);
		printf("outbound_packet: %s\n", error);
		// }

		u32 seq = ntohl(live_packet->tcp->seq);
		u8 *live_payload = packet_payload(live_packet);
		for (int i = 0; i < live_payload_len; i++) {
			if (live_payload[i] != (seq - 1 + i) % 10) {
				is_validate_outbound_data_error = true;
			}
		}

		last_received_packet_ack_seq = live_packet->tcp->ack_seq;
		if (live_packet->tcp_ts_ecr != NULL) {
			last_received_packet_ecr = *(live_packet->tcp_ts_ecr);
		}
	out:
		if (live_packet != NULL)
			packet_free(live_packet);
	}
	return NULL;
}

void start_validate_outbound_thread(struct packet *packet) {
	is_validate_outbound_running = true;
	if (pthread_create(&tid, NULL, validate_outbound_packet, (void *) packet) != 0) {
		die_perror("pthread_create");
	}
}

void stop_validate_outbound_thread() {
	is_validate_outbound_running = false;
	pthread_cancel(tid);
}