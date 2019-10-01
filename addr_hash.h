/*
 * addr_hash.h:
 *
 */

#ifndef __ADDR_HASH_H_ /* include guard */
#define __ADDR_HASH_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hash.h"

typedef struct {
    unsigned short int protocol;
    unsigned short int src_port;
    struct in_addr src;
    unsigned short int dst_port;
    struct in_addr dst;
} addr_pair;

typedef addr_pair key_type;      /* index into hash table */

hash_type* addr_hash_create();

#endif /* __ADDR_HASH_H_ */
