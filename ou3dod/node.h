#define _POSIX_SOURCE
#include "hash_table.h"
#include "pdu.h"
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

//Different state of state machine
typedef struct
{
    int socket;
    struct sockaddr_in addr;
    socklen_t addrlen;

} socket_struct;

typedef struct
{
    char circular_buffer[20600];
    uint16_t read_index;
    uint16_t write_index;
    uint16_t last_change;
} buffer_struct;

typedef struct
{
    socket_struct *udp;
    socket_struct *connector;
    socket_struct *successor;
    socket_struct *predecessor;
    socket_struct *tracker;
    uint8_t max_span;
    uint8_t range_start;
    uint8_t range_end;
    char self_adress[16];
    sigset_t set;
    buffer_struct circ_buf;
} node_struct;

typedef struct NET_ALIVE_PDU NET_ALIVE_PDU;
typedef struct NET_GET_NODE_PDU NET_GET_NODE_PDU;
typedef struct NET_GET_NODE_RESPONSE_PDU NET_GET_NODE_RESPONSE_PDU;
typedef struct NET_JOIN_PDU NET_JOIN_PDU;
typedef struct NET_JOIN_RESPONSE_PDU NET_JOIN_RESPONSE_PDU;
typedef struct NET_CLOSE_CONNECTION_PDU NET_CLOSE_CONNECTION_PDU;
typedef struct NET_NEW_RANGE_PDU NET_NEW_RANGE_PDU;
typedef struct NET_NEW_RANGE_RESPONSE_PDU NET_NEW_RANGE_RESPONSE_PDU;
typedef struct NET_LEAVING_PDU NET_LEAVING_PDU;
typedef struct VAL_INSERT_PDU VAL_INSERT_PDU;
typedef struct VAL_REMOVE_PDU VAL_REMOVE_PDU;
typedef struct VAL_LOOKUP_PDU VAL_LOOKUP_PDU;
typedef struct VAL_LOOKUP_RESPONSE_PDU VAL_LOOKUP_RESPONSE_PDU;
typedef struct STUN_LOOKUP_PDU STUN_LOOKUP_PDU;
typedef struct STUN_RESPONSE_PDU STUN_RESPONSE_PDU;
volatile bool interrupted = false;
//setiup the table for the states
typedef enum
{
    Q1,
    Q2,
    Q3,
    Q4,
    Q5,
    Q6,
    Q7,
    Q8,
    Q9,
    Q10,
    Q11,
    Q12,
    Q13,
    Q14,
    Q15,
    Q16,
    Q17,
    Q18,
    exit_state,
    last_state
} e_system_state;

//Setup the event table
typedef enum
{
    STUN_LOOKUP_EVENT,
    STUN_RESPONSE_EVENT,
    NET_GET_NODE_RESPONSE_EMPTY_EVENT,
    NET_GET_NODE_RESPONSE_NOT_EMPTY_EVENT,
    NET_JOIN_RESPONSE_EVENT,
    KEEP_GOING_EVENT,
    VAL_INSERT_EVENT,
    LOOKUP_EVENT,
    REMOVE_EVENT,
    NET_JOIN_EVENT,
    NET_NEW_RAGE_EVENT,
    NET_LEAVING_PDU_EVENT,
    NET_CLOSE_CONNECTION_EVENT,
    SHUTDOWN_REQUESTED_EVENT,
    NODE_CONNECTED_EVENT,
    NODE_NOT_CONNECTED_EVENT,
    NET_NEW_RANGE_RESPONSE_EVENT,
    IF_NODE_EQUALS_MAX_NODE_EVENT,
    IF_NODE_NOT_EQUALS_MAX_NODE_EVENT,
    BACK_TO_DEFAULT_EVENT,
    exit_event,
    last_event
} system_event;
/*
 * Function - unblock_signals
 * 
 * Description - Unblocks incoming interrupt signals
 * 
 * Input:   
 *        *node  - The node_struct containing the corresponding sigset
 * Output:           
 */
void unblock_signals(node_struct *node);
/*
 * Function - block_signals
 * 
 * Description - blocks incoming interrupt signals
 * 
 * Input:
 *        *node  - The node_struct containing the corresponding sigset  
 * Output:
 */
void block_signals(node_struct *node);
//typedef of 2d array
typedef e_system_state (*const afEventHandler[last_state][last_event])(node_struct *node);
typedef e_system_state (*pf_event_handler)(void);
/**
 * The state handler for state Q1
 */
e_system_state Q1_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q1;
}
/**
 * The state handler for state Q2
 */
e_system_state Q2_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q2;
}
/**
 * The state handler for state Q3
 */
e_system_state Q3_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q3;
}
/**
 * The state handler for state Q4
 */
e_system_state Q4_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q4;
}
/**
 * The state handler for state Q5
 */
e_system_state Q5_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q5;
}
/**
 * The state handler for state Q6
 */
e_system_state Q6_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q6;
}
/**
 * The state handler for state Q7
 */
e_system_state Q7_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q7;
}
/**
 * The state handler for state Q8
 */
e_system_state Q8_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q8;
}
/**
 * The state handler for state Q9
 */
e_system_state Q9_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q9;
}
/**
 * The state handler for state Q10
 */
e_system_state Q10_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q10;
}
/**
 * The state handler for state Q11
 */
e_system_state Q11_state_handler(node_struct *node)
{

    return Q11;
}
/**
 * The state handler for state Q12
 */
e_system_state Q12_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q12;
}
/**
 * The state handler for state Q13
 */
e_system_state Q13_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q13;
}
/**
 * The state handler for state Q14
 */
e_system_state Q14_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q14;
}
/**
 * The state handler for state Q15
 */
e_system_state Q15_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q15;
}
/**
 * The state handler for state Q16
 */
e_system_state Q16_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q16;
}
/**
 * The state handler for state Q17
 */
e_system_state Q17_state_handler(node_struct *node)
{
    unblock_signals(node);
    if (interrupted)
    {
        interrupted = false;
        return Q10;
    }
    block_signals(node);
    return Q17;
}
/**
 * The state handler for state Q18
 */
e_system_state Q18_state_handler(node_struct *node)
{

    return Q18;
} /**
 * The state handler for state exit_state
 */
e_system_state exit_state_handler(node_struct *node)
{

    return exit_state;
}
//Initialize state table
static afEventHandler state_machine =
    {

        [Q1] = {[STUN_LOOKUP_EVENT] = Q2_state_handler,
                [KEEP_GOING_EVENT] = Q1_state_handler},

        [Q2] = {[STUN_RESPONSE_EVENT] = Q3_state_handler},

        [Q3] = {[NET_GET_NODE_RESPONSE_EMPTY_EVENT] = Q4_state_handler,
                [NET_GET_NODE_RESPONSE_NOT_EMPTY_EVENT] = Q7_state_handler},

        [Q4] = {[KEEP_GOING_EVENT] = Q6_state_handler},

        [Q5] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q6] = {[VAL_INSERT_EVENT] = Q9_state_handler,
                [LOOKUP_EVENT] = Q9_state_handler,
                [REMOVE_EVENT] = Q9_state_handler,
                [SHUTDOWN_REQUESTED_EVENT] = Q10_state_handler,
                [NET_JOIN_EVENT] = Q12_state_handler,
                [NET_NEW_RAGE_EVENT] = Q15_state_handler,
                [NET_LEAVING_PDU_EVENT] = Q16_state_handler,
                [NET_CLOSE_CONNECTION_EVENT] = Q17_state_handler,
                [BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q7] = {[NET_JOIN_RESPONSE_EVENT] = Q8_state_handler},

        [Q8] = {[KEEP_GOING_EVENT] = Q6_state_handler},

        [Q9] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q10] = {[NODE_CONNECTED_EVENT] = Q11_state_handler,
                 [exit_event] = exit_state_handler},

        [Q11] = {[NET_NEW_RANGE_RESPONSE_EVENT] = Q18_state_handler},

        [Q12] = {[NODE_NOT_CONNECTED_EVENT] = Q5_state_handler,
                 [IF_NODE_EQUALS_MAX_NODE_EVENT] = Q13_state_handler,
                 [IF_NODE_NOT_EQUALS_MAX_NODE_EVENT] = Q14_state_handler},

        [Q13] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q14] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q15] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q16] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q17] = {[BACK_TO_DEFAULT_EVENT] = Q6_state_handler},

        [Q18] = {[exit_event] = exit_state_handler},
};

/*
 * Function - print_state
 * 
 * Description - Prints the state and a indented tab
 * 
 * Input:
 *        *buf - The string to print
 * Output:
 *        
 */
void print_state(char *buf);
/*
 * Function - create_UDP_socket
 * 
 * Description - uses a socket_struct to create a UDP socket
 * 
 * Input:
 *        *ss - the socket_struct to append the UDP socket into
 * Output:
 *        
 */
void create_UDP_socket(socket_struct *ss);
/*
 * Function - send_UDP
 * 
 * Description - Send a char buffer to the predefined tracker socket
 * 
 * Input:
 *        *node - The node_struct containing the UDP socket and the tracker address
 *        *buffer - The char buffer to send
 *        nr_of_bytes - The number of bytes to transfer
 * Output:
 */
void send_UDP(node_struct *node, char *buffer, int nr_of_bytes);
/*
 * Function - recieve_UDP
 * 
 * Description - Reads data from a UDP socket 
 * 
 * Input:
 *        *node - The node_struct containing the UDP socket information
 * Output:
 *        *buffer - Containing the content of the socket
 */
char *recieve_UDP(node_struct *node);
/*
 * Function - connect_TCP_socket
 * 
 * Description - Connects a tcp socket to a ip and port
 * 
 * Input:
 *        *ss - The socket_struct containing the socket
 *        ip - The the ip to connect to 
 *        port - The port to connect to
 * Output:
 *        
 */
void connect_TCP_socket(socket_struct *socket, uint32_t ip, uint16_t port);
/*
 * Function - init_socket_struct
 * 
 * Description - Initiates the socket_struct 
 * 
 * Input:
 *        
 * Output:
 *        *ss - The initiates socket_struct
 */
socket_struct *init_socket_struct();
/*
 * Function - calloc_error
 * 
 * Description - Allocates memory the same as calloc handles errors
 * 
 * Input:
 *        size - the number of members
 *        size_of_type - size of each member
 * Output:
 *        *p - the void pointer with the allocated memory
 */
void *calloc_error(int size, ssize_t size_of_type);
/*
 * Function - read_sockets
 * 
 * Description - Read the content of the socket
 * 
 * Input:
 *        *pollfds - The data structure containing the polling requests
 *        *node - The node_struct to read to
 * Output:
 *        *buf - The read buffer 
 */
void read_sockets(struct pollfd *pollfds, node_struct *node);
/*
 * Function - handle_NET_GET_NODE_RESPONSE
 * 
 * Description - Reads the received PDU and decides if our current node is alone in the network.
 *               If the node is not alone, insert the given address and port into predecess address.
 * 
 * Input:
 *        *pdu - The NET_GET_NODE_RESPONSE struct
 *        *node - The node with the predecessor socket
 * Output:
 *        true - If the node is alone in the network
 *        false - If the node is not alone in the network 
 */
bool handle_NET_GET_NODE_RESPONSE(struct NET_GET_NODE_RESPONSE_PDU *pdu, node_struct *node);
/*
 * Function - get_STUN_RESPONSE
 * 
 * Description - Reads from the UDP socket and determines if it's a STUN_RESPOSNE_PDU
 * 
 * Input:
 *        *node - The node struct containing the UDP socket and tracker address
 * Output:
 */
void get_STUN_RESPONSE(node_struct *node);
/*
 * Function - parse_buffer
 * 
 * Description - Reads a given buffer and determine what PDU it represents by returning the next state
 * 
 * Input:
 *        *buffer - The buffer to read from
 * Output:
 *        e_system_state - The state the PDU represents 
 */
e_system_state parse_buffer(char *buffer);
/*
 * Function - send_alive
 * 
 * Description - Sends a 0 to the tracker
 * 
 * Input:
 *        *node  - The node_struct containing the udp socket and tracker information  
 * Output:
 * 
 */
void send_alive(node_struct *node);
/*
 * Function - accept_new_predecessor
 * 
 * Description - Reads the connector socket to see if there is a request to connect with a new node
 * 
 * Input:
 *         *pollfds - The data structure containing the polling requests
 *         *node - The node_struct containing the connector socket
 * Output:
 */
void accept_new_predecessor(struct pollfd *pollfds, node_struct *node);
/*
 * Function - init_node
 * 
 * Description - blocks incoming interrupt signals
 * 
 * Input:
 *        *ip - The ip for the tracker we want the node to connect to
 *        port_no - The port for the tracker we want the node to connect to
 * Output:
 *        *node - the node_struct containing all the necessary socket_struct:s and signal_struct:s
 */
node_struct *init_node(char *ip, uint16_t port_no);
/*
 * Function - connect_to_successor
 * 
 * Description - Connects our the successor socket to a new successor socket
 * 
 * Input:
 *        *node - The node_struct containing the successor socket
 * Output:
 */
void connect_to_successor(node_struct *node);
/*
 * Function - process_insert_request
 * 
 * Description - Inserts the a insert request into our hash table, or forwards it to our successor.
 * 
 * Input:
 *        *h - The hash table
 *        *buffer - The char pointer to read the pdu from
 *        *node - The node_struct
 * Output:
 */
void process_insert_request(h_table *h, char *buffer, node_struct *node);
/*
 * Function - process_removal_request
 * 
 * Description - Removes the requested ssn from our hash table, or forwards the pdu to our successor.
 * 
 * Input:
 *        *h - The hash table
 *        *buffer - The char pointer to read the pdu from
 *        *node - The node_struct
 * Output: 
 */
void process_removal_request(h_table *h, char *buffer, node_struct *node);

/*
 * Function - process_lookup_request
 * 
 * Description - Searches the node's hash table to see if the requested ssn is inside of it.
 *               If not send empty response to the client.
 * 
 * Input:
 *        *h - The hash table
 *        *buffer - The char pointer to read the pdu from
 *        *node - The node_struct
 * Output:
 *        
 */
void process_lookup_request(h_table *h, char *buffer, node_struct *node);
/*
 * Function - in_hash_range
 * 
 * Description - Determines if the given ssn is inside the nodes current hash rang
 * 
 * Input:
 *        ssn - The ssn string to check
 *        *node - The node_struct containing our hash range
 * Output:
 *        true - If the ssn is inside our hash range
 *        false - If the ssn is not inside our hash range
 */
bool in_hash_range(char ssn[SSN_LENGTH], node_struct *node);
/*
 * Function - update_hash_range
 * 
 * Description - Updates the hash range of the hash table
 * 
 * Input:
 *        *node - The node containing the hash table
 *        *pdu - The PDU containing the new hash ranges
 * Output:
 *        *pdu - The new PDU_RESPONSE with the updated hash range
 */
void update_hash_range(node_struct *node, struct NET_JOIN_RESPONSE_PDU *pdu);
/*
 * Function - print_VAL_INSERT_PDU
 * 
 * Description - Prints the VAL_INSERT_PDU
 * 
 * Input:
 *        *pdu - The PDU to print
 * Output:
 */
void print_VAL_INSERT_PDU(struct VAL_INSERT_PDU *pdu);
/*
 * Function - handler
 * 
 * Description - The signal handler
 * 
 * Input:
 *        num  - arbitary int
 * Output:
 */
void handler(int num);
/*
 * Function - check_size
 * 
 * Description - Determines the size of the incoming PDU
 * 
 * Input:
 *        *buffer - The incoming char buffer to read from
 * Output:
 *        int - The size of the pdu in int
 */
int check_size(char *buffer);
/*
 * Function - initiate_signal_handler
 * 
 * Description - Initiates the signal handler function 
 * 
 * Input:
 *        *node  - The node_struct that the corresponding set will be inserted  
 * Output:
 */
void initiate_signal_handler(node_struct *node);
/*
 * Function - transfer_values
 * 
 * Description - Transfer all the values from our hash table within the specified hash value
 *  
 * Input:
 *        low_boundry - The lower hash range to extract from the hash table
 *        upper_boundry - The upper hash range to extract from the hash table
 *        *h - The hash table
 *        x - 'p' if we want to send to our predecessor or 's' if we want to send to our successor 
 *        *node - The node_struct containing the hash table and sockets
 * Output:
 */
void transfer_values(uint8_t low_boundry, uint8_t upper_boundry, h_table *h, char x, node_struct *node);
/*
 * Function - read_from_buf
 * 
 * Description - Reads one PDU from the circular buffer
 * 
 * Input:
 *        *node - The node containing the circular buffer
 * Output:
 *        *buf - The read pdu
 *        NULL - If the pdu was not recognized
 */
char *read_from_buf(node_struct *node);
/*
 * Function - write_to_buf
 * 
 * Description - Writes to the circular buffer
 * 
 * Input:
 *        *buf - The char pointer to insert inside the circular buffer
 *        size - The number of bytes to insert into the circular buffer
 *        *node - The node_struct containing the circular buffer
 * Output:
 */
void write_to_buf(char *buf, int size, node_struct *node);
/*
 * Function - send_tcp
 * 
 * Description - Send with a tcp socket and checks the ERRNO
 * 
 * Input:
 *        *ss - The socket_struct the user want to send to
 *        *buf - The buffer to send
 *        size - Number of bytes to send
 *             
 * Output:
 *        
 */
void send_tcp(socket_struct *ss, char *buf, size_t size);
void cleanup(node_struct *node, h_table *h);