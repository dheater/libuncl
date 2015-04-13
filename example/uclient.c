#include <stdio.h>
#include "../include/uncl.h"
#include "utest.h"

int main()
{
    const int max_msg_size = num_transfers;
    uncl_socket_t usock;
    uncl_msg_t msg;
    char *data;
    int i;

    /* Open a port to the sever. */
    if (-1 == uncl_client(&usock, host, port)) {
        perror("uncl_server");
        return -1;
    }

    if (-1 == uncl_msg_alloc(&msg, max_msg_size)) {
        perror("uncl_msg_alloc");
        return -1;
    }

    data = uncl_msg_data(msg);
    if (NULL == data) {
        perror("uncl_msg_data");
        return -1;
    }

    for (i = 0; i < num_transfers; ++i) {
        if (-1 == uncl_msg_set_size(msg, i + 1)) {
            perror("uncl_msg_set_size");
            return -1;
        }

        uncl_msg_set_type(msg, i);
        data[i] = i;

        if (-1 == uncl_msg_write(usock, msg)) {
            perror("uncl_msg_write");
            return -1;
        }

        if (-1 == uncl_msg_read(usock, msg)) {
            perror("uncl_msg_read");
            return -1;
        }

        if ((data[i] != i) || (uncl_msg_get_type(msg) != i)) {
            fprintf(stderr, "Data transfer error occurred on message %d\n", i);
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
