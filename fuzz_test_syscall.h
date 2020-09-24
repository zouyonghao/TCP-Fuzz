#ifndef __FUZZ_TEST_SYSCALL_H
#define __FUZZ_TEST_SYSCALL_H

#include "logging.h"
#include "parse.h"

#include "fuzz_test_helper.h"

/**
 * NOTE: limit the write num, because the send buffer may differ for different implementations.
 */
#define MAX_WRITE_READ_NUM 3000

void run_fuzz_syscall_read_event();
void run_fuzz_syscall_write_event();
void run_fuzz_syscall_ioctl_siocinq_event();
void run_fuzz_syscall_fcntl_getfl_event();
void run_fuzz_syscall_fcntl_setfl_nonblock_event();
void run_fuzz_syscall_fcntl_setfl_block_event();
void run_fuzz_syscall_setsockopt1_event();
void run_fuzz_syscall_setsockopt2_event();
void run_fuzz_syscall_setsockopt3_event();
void run_fuzz_syscall_setsockopt4_event();
void run_fuzz_syscall_setsockopt5_event();

void add_read_to_fuzz_script(int read_num);

void add_content_to_fuzz_script(char *content);

#endif