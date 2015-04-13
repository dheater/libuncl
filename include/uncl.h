/*
===============================================================================
                            COPYRIGHT NOTICE

    Copyright (C) 2007 Daniel L. Heater
    International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   o Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
   o Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   o The names of its contributors may not be used to endorse or promote
     products derived from this software without specific prior written
     permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

===============================================================================
*/

#ifndef _UDP_NETWORK_COMM_H_
#define _UDP_NETWORK_COMM_H_

/**
  @mainpage UDP Network Communication Library (UNCL)
  The UDP Network Communication Library (UNCL) provides some basic routines
  for message passing and error detection using UDP sockets.

  @AUTHOR Daniel L. Heater
  @VERSION 1.0
  @WARNING This implementation doesn't account for endianess.
 */

/**
 * Type for UDP network communications library messages.
 */
typedef struct _uncl_msg *uncl_msg_t;

/**
 * Type for UDP network communications library socket parameters.
 */
typedef struct _uncl_socket *uncl_socket_t;

/**
  Allocate a message buffer.
  @param msg Address of the location in which to allocate the message.
  @param nbytes Size of the buffer in bytes.
  @return 0 on success, -1 on error.
 */
extern int uncl_msg_alloc(uncl_msg_t *msg, unsigned int nbytes);

/**
 * Get a pointer to the data section of an allocated message.
 * @param msg A message that has already been allocated.
 * @return A pointer to the message data on success or NULL on failure.
 */
extern void * uncl_msg_data(uncl_msg_t msg);

/**
 * Set the message type.
 * The message type is used as a unique identifier for distingushing between
 * various message types that may be passed across the socket. The message
 * types are defined by the user.
 * @param msg Message that we will be modifiying.
 * @param msg_type The user defined message type.
 */
extern void uncl_msg_set_type(uncl_msg_t msg, int msg_type);

/**
 * Get the message type.
 * The message type is used as a unique identifier for distingushing between
 * various message types that may be passed across the socket. The message
 * types are defined by the user.
 * @param msg Message that we will be modifiying.
 * @return The user defined message type.
 */
extern int uncl_msg_get_type(uncl_msg_t msg);

/**
 * Set the message size to match the amount of data we want to send.
 * @param msg Message that we will be modifiying.
 * @param data_size Size of the data portion of the message.
 * @return 0 on success, -1 on error.
 */
extern int uncl_msg_set_size(uncl_msg_t msg, unsigned int data_size);

/**
 * Read a message from a UDP socket.
 * @param usock An initialized UDP network communications library socket.
 * @param msg Message buffer to read into.
 * @return 0 on sucess, -1 on failure.
 */
extern int uncl_msg_read(uncl_socket_t usock, uncl_msg_t msg);

/**
 * Write a message to a UDP socket.
 * @param usock An initialized UDP network communications library socket.
 * @param msg Message buffer to read into.
 * @return 0 on sucess, -1 on failure.
 */
extern int uncl_msg_write(uncl_socket_t usock, uncl_msg_t msg);

/**
 * Open a connection to a UDP server.
 * @param usock UDP network communications library socket
 * @param hostname Server hostname or IP address.
 * @param port Server port number.
 * parameters.
 * @return 0 on success, -1 on error.
 */
extern int uncl_client(uncl_socket_t * usock, char *hostname, int port);

/**
 * Create a UDP server.
 * @param usock UDP network communications library socket
 * @param port Server port number.
 * @return 0 on success, -1 on error.
 */
extern int uncl_server(uncl_socket_t * usock, int port);

/**
 * Get a count of the number of successfully read messages.
 * @param usock UDP network communications library socket.
 * @return Total number successfully read messages.
 */
extern int uncl_msg_read_count(uncl_socket_t usock);

/**
 * Get a count of the number of successfully written messages.
 * @param usock UDP network communications library socket.
 * @return Total number successfully read messages.
 */
extern int uncl_msg_write_count(uncl_socket_t usock);

/**
 * Get the number of UDP network communications library errors for this socket.
 * @param usock UDP network communications library socket.
 * @return Total number of detected errors for this socket.
 */
extern int uncl_error_count(uncl_socket_t usock);

/**
 * Clear the UDP network communications library error counts for this socket.
 * @param usock UDP network communications library socket.
 */
extern void uncl_clear_errors(uncl_socket_t usock);

/**
 * Get the number of buffer overflow errors on this socket.
 * @param usock UDP network communications library socket.
 * @return The number of buffer overflow errors on this socket.
 */
extern int uncl_buffer_overflow_errors(uncl_socket_t usock);

/**
 * Get the number of sequence count errors on this socket.
 * @param usock UDP network communications library socket.
 * @return The number of sequence count errors on this socket.
 */
extern int uncl_sequence_count_errors(uncl_socket_t usock);

/**
 * Get the number of readerrors on this socket.
 * @param usock UDP network communications library socket.
 * @return The number of read errors on this socket.
 */
extern int uncl_read_errors(uncl_socket_t usock);

/**
 * Get the number of write errors on this socket.
 * @param usock UDP network communications library socket.
 * @return The number of write errors on this socket.
 */
extern int uncl_write_errors(uncl_socket_t usock);

#endif /* _UDP_NETWORK_COMM_H_ */
