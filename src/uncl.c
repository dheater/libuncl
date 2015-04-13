#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include "../include/uncl.h"

//#define DEBUG
#ifdef DEBUG
#define PERROR(x...) fprintf(stderr, "%s %d: ", __FILE__, __LINE__); \
    perror(x)
#define DPRINTF(x...) fprintf(stderr, x);
#else
#define DPRINTF(x...)
#define PERROR(x...)
#endif

/**
 * @internal
 * This is the structure of the message header.
 * This structure is only accessed by an opaque pointer and therefore
 * should not be referenced by external source code.
 */
struct _uncl_msg {
    uint32_t buffer_size; /**< Size of the allocated message buffer. */
    uint32_t nbytes; /**< Size of this message in bytes. */
    uint32_t msg_type; /**< User defined message type identifier. */
    uint32_t sequence_count; /**< Continuously incrementing counter
                               of packets sent. */
    char data[1]; /**< This element points the variable length data
                    that should be cast to the appropriate data
                    structure. */
};

/* The header overhead is the amount of header date minus the buffer size
 * field since we don't send that value.
 */
static const int header_overhead = 3 * sizeof(uint32_t);
static const int header_size = 4 * sizeof(uint32_t);

/**
 * @internal
 * This is the structure of the message header.
 * This structure is only accessed by an opaque pointer and therefore
 * should not be referenced by external source code.
 */
struct _uncl_socket {
    int sock; /**< Socket number. */
    struct sockaddr_in addr; /**< Socket parameters */
    uint32_t sequence_count; /**< Per socket sequence counter. */
    uint32_t current_count; /**< Next seqeunce count we expect to see. */
    int read_count; /**< Number of messages sucessfully read. */
    int write_count; /**< Number of messages sucessfully written. */
    int buffer_overflow_errors; /**< Number of buffer overflow errors. */
    int sequence_count_errors; /**< Number of sequence count errors. */
    int read_errors; /**< Number of read errors. */
    int write_errors; /**< Number of write errors. */
};

int uncl_msg_alloc(uncl_msg_t * msg, unsigned int nbytes)
{
    *msg = malloc(nbytes + header_size);
    if (NULL == *msg) {
        PERROR("malloc");
        return -1;
    }

    (*msg)->buffer_size = nbytes;
    (*msg)->nbytes = uncl_msg_set_size(*msg, 0);

    return 0;
}

void *uncl_msg_data(uncl_msg_t msg)
{
    if (NULL == msg) {
        errno = ENOMEM;
        return NULL;
    }

    if (0 == msg->buffer_size) {
        errno = EMSGSIZE;
        return NULL;
    }

    return &(msg->data);
}

void uncl_msg_set_type(uncl_msg_t msg, int msg_type)
{
    msg->msg_type = msg_type;
}

int uncl_msg_get_type(uncl_msg_t msg)
{
    return msg->msg_type;
}

int uncl_msg_set_size(uncl_msg_t msg, unsigned int data_size)
{
    if (NULL == msg) {
        errno = ENOMEM;
        return -1;
    }

    if (data_size > msg->buffer_size) {
        errno = EMSGSIZE;
        return -1;
    }

    /* We don't transfer the message buffer size so we exclude it from the
     * number of bytes.
     */
    msg->nbytes = data_size + header_overhead;

    return 0;
}

int uncl_msg_read(uncl_socket_t usock, uncl_msg_t msg)
{
    int cnt, nbytes = 0,
        remaining_msg_bytes = header_overhead + msg->buffer_size;
    /* Skip the buffer size. We don't need to transfer this. */
    char *buf = (char *) &(msg->nbytes);
    static socklen_t addr_len = sizeof(struct sockaddr_in);

    do {
        cnt = recvfrom(usock->sock, &buf[nbytes], remaining_msg_bytes, 0,
                       (struct sockaddr *) &(usock->addr), &addr_len);
        if (-1 == cnt) {
            PERROR("recvfrom");
            ++usock->read_errors;
            return -1;
        }
        nbytes += cnt;
        remaining_msg_bytes -= cnt;

        DPRINTF("Read %d bytes\n", cnt);

    } while ((nbytes < msg->nbytes) && remaining_msg_bytes);

    /* Look for buffer overflow errors. If this happens, expect the next one
     * or more message to have errors since we'll be reading in the remainder
     * of this message in addition to any other messsages that are in the queue.
     */
    if ((msg->nbytes - header_overhead) > msg->buffer_size) {
        //++usock->buffer_overflow_errors;
    }

    /* Look for message sequence count errors. If an error occurs, set the
     * current count to match the current message.
     */
    if (msg->sequence_count != ++usock->current_count) {
        usock->current_count = msg->sequence_count;
        ++usock->sequence_count_errors;
    }

    ++usock->read_count;
    return 0;
}

int uncl_msg_write(uncl_socket_t usock, uncl_msg_t msg)
{
    msg->sequence_count = ++usock->sequence_count;

    DPRINTF("Sending %d byte message\n", msg->nbytes);

    /* Notice that the data sent starts with nbytes. We skip over the buffer
     * size field because we do not need it.
     */
    if (-1 == sendto(usock->sock, &(msg->nbytes), msg->nbytes, 0,
                     (struct sockaddr *) &(usock->addr), sizeof(usock->addr))) {
        PERROR("sendto");
        ++usock->write_errors;
    }

    ++usock->write_count;
    return 0;
}

int uncl_msg_read_count(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    return (usock) ? usock->read_count : 0;
}

int uncl_msg_write_count(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    return (usock) ? usock->write_count : 0;
}

int uncl_error_count(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    if(NULL == usock) {
        return 0;
    }

    return usock->buffer_overflow_errors + usock->sequence_count_errors +
        usock->read_errors + usock->write_errors;
}

void uncl_clear_errors(uncl_socket_t usock)
{
    if(NULL == usock) {
        return;
    }

    usock->buffer_overflow_errors = 0;
    usock->sequence_count_errors = 0;
    usock->read_errors = 0;
    usock->write_errors = 0;
}

int uncl_buffer_overflow_errors(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    return (usock) ? usock->buffer_overflow_errors : 0;
}

int uncl_sequence_count_errors(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    return (usock) ? usock->sequence_count_errors : 0;
}

int uncl_read_errors(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    return (usock) ? usock->read_errors : 0;
}

int uncl_write_errors(uncl_socket_t usock)
{
    /* Just return 0 if the socket is not valid. */
    return (usock) ? usock->write_errors : 0;
}

int uncl_client(uncl_socket_t * usock, char *hostname, int port)
{
    const int one = 1;
    struct hostent *host;

    *usock = malloc(sizeof(struct _uncl_socket));
    if (NULL == *usock) {
        PERROR("malloc");
        return -1;
    }

    DPRINTF("Attempting to connect to host: %s:%d\n", hostname, port);

    /* Initialize to structure to all zeros. This includes all of our error
     * parameters.
     */
    memset(*usock, 0, sizeof(**usock));

    (*usock)->addr.sin_port = htons(port);
    if (strtol(hostname, NULL, 10) > 0) {
        /* Handle dot numbered server IP address. */
        DPRINTF("Converting numeric string\n");
        (*usock)->addr.sin_family = AF_INET;
        (*usock)->addr.sin_addr.s_addr = inet_addr(hostname);
    } else if (NULL != (host = gethostbyname(hostname))) {
        /* Named host address. */
        DPRINTF("Converting hostname\n");
        (*usock)->addr.sin_family = host->h_addrtype;
        memcpy(&((*usock)->addr.sin_addr.s_addr), host->h_addr, host->h_length);
    } else {
        herror("gethostbyname");
        return -1;
    }

    (*usock)->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == (*usock)->sock) {
        PERROR("socket");
        return -1;
    }

    DPRINTF("Socket created\n");

    if (setsockopt((*usock)->sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one,
                   sizeof(one))) {
        PERROR("setsockopt");
        return -1;
    }

    return 0;
}

int uncl_server(uncl_socket_t * usock, int port)
{
    const int one = 1;

    *usock = malloc(sizeof(struct _uncl_socket));
    if (NULL == *usock) {
        PERROR("malloc");
        return -1;
    }

    /* Initialize to structure to all zeros. This includes all of our error
     * parameters.
     */
    memset(*usock, 0, sizeof(**usock));

    (*usock)->addr.sin_port = htons(port);
    (*usock)->addr.sin_family = AF_INET;
    (*usock)->addr.sin_addr.s_addr = INADDR_ANY;

    (*usock)->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == (*usock)->sock) {
        PERROR("socket");
        return -1;
    }

    DPRINTF("Socket created on port %d\n", port);

    if (setsockopt((*usock)->sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one,
                   sizeof(one))) {
        PERROR("setsockopt");
        return -1;
    }

    if (bind((*usock)->sock, (struct sockaddr *) &((*usock)->addr),
             sizeof((*usock)->addr))) {
        PERROR("bind");
        return -1;
    }

    return 0;
}
