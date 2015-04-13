#include <stdio.h>
#include "../include/uncl.h"
#include "utest.h"

int main()
{
    const int max_msg_size = num_transfers;
    uncl_socket_t usock;
    uncl_msg_t msg;
    int i;

    printf("hello world!\n");

    /* Create the server. */
    if (-1 == uncl_server(&usock, port)) {
        perror("uncl_server");
        return -1;
    }

    if (-1 == uncl_msg_alloc(&msg, max_msg_size)) {
        perror("uncl_msg_alloc");
        return -1;
    }

    for (i = 0; i < num_transfers; ++i) {
        if (-1 == uncl_msg_read(usock, msg)) {
            perror("uncl_msg_read");
            return -1;
        }

        if(-1 == uncl_msg_set_size(msg, i + 1)){
            perror("uncl_msg_set_size");
            return -1;
        }

        if (-1 == uncl_msg_write(usock, msg)) {
            perror("uncl_msg_write");
            return -1;
        }
    }

    printf("%d transfers completed with %d errors.\n", num_transfers,
            uncl_error_count(usock));
    printf("\tSuccessful message reads: %d\n", uncl_msg_read_count(usock));
    printf("\tSuccessful message writes: %d\n", uncl_msg_write_count(usock));
    printf("\tBuffer overflow errors: %d\n",
           uncl_buffer_overflow_errors(usock));
    printf("\tSequence count errors: %d\n", uncl_sequence_count_errors(usock));
    printf("\tRead errors: %d\n", uncl_read_errors(usock));
    printf("\tWrite errors: %d\n", uncl_write_errors(usock));

    return 0;
}
