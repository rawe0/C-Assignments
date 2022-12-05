/*
* Computer Networks.
* Autumn 2021
* Assignment 3
* File:         node.c
* Description:  A node implementation for the course Computer Network 5DV213
*               This node implements a hashtable and works with other nodes to 
*               distribute information among the nodes depending on the calculated
*               hash values, a specific node is forwarded the entry via a socket from
*               another node.
*
* Author:       Rasmus Welander
* CS username:  c19rwr@cs.umu.se
*
* Author:       Marcus Rost
* CS username:  bio16mrt@cs.umu.se
*
* Date:         2021-10-28
*/
#include "node.h"
#define _POSIX_SOURCE
#include "hash_table.h"
#include "serialize.h"

#define MAX_CLIENTS 4

static const int BUFSIZE = 20000;

int main(int argc, char *argv[])
{

    e_system_state e_next_state = Q1;
    system_event new_event = KEEP_GOING_EVENT;

    h_table *hash_table;
    //If the program was not used correctly
    if (argc != 3)
    {
        fprintf(stderr, "Parameter(s) :<Server Address> <Server Port>\n");
        exit(1);
    }
    char ip[16];
    strcpy(ip, argv[1]);
    int port_no = atoi(argv[2]);

    //Initiates the node_struct
    node_struct *node = init_node(ip, port_no);
    //Always block signals when inside a state
    block_signals(node);

    //Initiates the poll file descriptors
    struct pollfd pollfds[MAX_CLIENTS];
    memset(pollfds, 0, sizeof(pollfds));
    pollfds[0].fd = node->connector->socket;
    pollfds[0].events |= POLLIN;
    pollfds[1].fd = node->udp->socket;
    pollfds[1].events |= POLLIN;
    pollfds[3].fd = node->successor->socket;
    pollfds[3].events |= POLLIN;
    //Will use this char pointer to transfer small bits of data between states
    char *recieved_data;
    //Start the state machine
    while (1)
    {
        if (state_machine[e_next_state][new_event] != NULL)
        {
            e_next_state = (*state_machine[e_next_state][new_event])(node);
            switch (e_next_state)
            {
            /** State Q1
             *  In this state the node sends a STUN_LOOKUP_PDU to the tracker
             */
            case Q1:
            {
                print_state("Q1");
                printf("Node started sending STUN_LOOKUP to tracker v4(%s:%d)\n",
                       inet_ntoa(node->tracker->addr.sin_addr), ntohs(node->tracker->addr.sin_port));

                struct STUN_LOOKUP_PDU pdu_sl;
                pdu_sl.type = STUN_LOOKUP;
                recieved_data = seralize_STUN_LOOKUP_PDU(&pdu_sl);
                send_UDP(node, recieved_data, 1);
                new_event = STUN_LOOKUP_EVENT;
                free(recieved_data);
            }
            break;
            /** State Q2
             *  In this state the node receives a STUN_RESPONSE_PDU from the tracker.
             */
            case Q2:
            {
                print_state("Q2");
                get_STUN_RESPONSE(node);
                new_event = STUN_RESPONSE_EVENT;
            }
            break;
            /** State Q3
             *  In this state the node  got a STUN_RESPONSE_PDU from the 
             *  tracker and determines if it is alone in the network.
             */
            case Q3:
            {

                print_state("Q3");
                struct NET_GET_NODE_PDU pdu_ngn;
                pdu_ngn.type = NET_GET_NODE;
                char *data = seralize_NET_GET_NODE_PDU(&pdu_ngn);
                send_UDP(node, data, 1);
                free(data);
                struct NET_GET_NODE_RESPONSE_PDU *pdu_gnr;
                recieved_data = recieve_UDP(node);
                pdu_gnr = deserialize_NET_GET_NODE_RESPONSE_PDU(recieved_data);
                free(recieved_data);
                if (handle_NET_GET_NODE_RESPONSE(pdu_gnr, node))
                {
                    new_event = NET_GET_NODE_RESPONSE_EMPTY_EVENT;
                }
                else
                {
                    new_event = NET_GET_NODE_RESPONSE_NOT_EMPTY_EVENT;
                }
                free(pdu_gnr);
            }
            break;
            /** State Q4
             *  In this state the node was the first node trying to join the network and
             *  can therefore create its hash table
             */
            case Q4:
            {
                print_state("Q4");
                printf("\n");
                hash_table = h_table_empty(node->range_start, node->range_end);
                send_alive(node);
                new_event = KEEP_GOING_EVENT;
            }
            break;
            /** State Q5
             *  In this state the node got a NET_JOIN_PDU and it was alone inside the network, they connect to 
             *  each other.
             */
            case Q5:
            {
                print_state("Q5");
                struct NET_JOIN_PDU *pdu = deserialize_NET_JOIN_PDU(recieved_data);
                node->successor->addr.sin_addr.s_addr = htonl(pdu->src_address);
                node->successor->addr.sin_port = htons(pdu->src_port);
                connect_to_successor(node);
                printf("Connected to new succcesor V4(%s:%u)\n",
                       inet_ntoa(node->successor->addr.sin_addr), htons(node->successor->addr.sin_port));
                //Set the poll file descriptor #3 to successor socket
                pollfds[3].fd = node->successor->socket;

                struct NET_JOIN_RESPONSE_PDU *pdu_njr = malloc_error(sizeof(pdu_njr));

                update_hash_range(node, pdu_njr);
                pdu_njr->next_address = ntohl(node->connector->addr.sin_addr.s_addr);
                pdu_njr->next_port = ntohs(node->connector->addr.sin_port);

                char *buf = seralize_NET_JOIN_RESPONSE_PDU(pdu_njr);
                send_tcp(node->successor, buf, 9);
                transfer_values(pdu_njr->range_start, pdu_njr->range_end, hash_table, 's', node);
                accept_new_predecessor(pollfds, node);
                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q6
             *  In this state the node is fully connected and will continuously check the poll struct to see if
             *  a socket got something to read from.
             */
            case Q6:
            {
                int poll_result = 0;
                //If the circular buffer is readable
                if (node->circ_buf.read_index != node->circ_buf.write_index)
                {
                    //free(recieved_data);
                    recieved_data = read_from_buf(node);
                    if (recieved_data != NULL)
                    {
                        new_event = parse_buffer(recieved_data);
                    }
                    else
                    {
                        new_event = BACK_TO_DEFAULT_EVENT;
                    }
                    break;
                }

                printf("[Q6]  ");
                printf("(%d entries stored)\n", h_table_size(hash_table));
                poll_result = poll(pollfds, 4, 5000);
                //If there is something to read
                if (poll_result > 0)
                {
                    read_sockets(pollfds, node);
                }
                else if (node->range_start == 0)
                {
                    //Only one node in the system send NET_ALIVE_PDU
                    send_alive(node);
                }
                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q7
             *  In this state the node is trying to enter the network but determined that it was not alone,
             *  it have to send a NET_GET_NODE_RESPONSE and accept a new predecessor
             */
            case Q7:
            {
                print_state("Q7");
                printf("I am not the first node, sending NET_JOIN to V4(%s:%d)\n",
                       inet_ntoa(node->predecessor->addr.sin_addr), ntohs(node->predecessor->addr.sin_port));
                struct NET_JOIN_PDU pdu_nj;
                pdu_nj.type = NET_JOIN;
                pdu_nj.src_address = inet_addr(inet_ntoa(node->connector->addr.sin_addr));
                pdu_nj.src_address = ntohl(pdu_nj.src_address);
                pdu_nj.src_port = ntohs(node->connector->addr.sin_port);
                //Empty values so that the receiver understand that we want to join the network
                pdu_nj.max_span = 0;
                pdu_nj.max_address = 0;
                pdu_nj.max_port = 0;
                char *data_to_send = seralize_NET_JOIN_PDU(&pdu_nj);
                if (sendto(node->udp->socket, data_to_send, 14, 0,
                           (struct sockaddr *)&node->predecessor->addr, sizeof(node->predecessor->addr)) == -1)
                {
                    perror("Sendto:");
                    exit(1);
                }
                free(data_to_send);

                accept_new_predecessor(pollfds, node);

                int poll_result = 0;
                while (1)
                {
                    poll_result = poll(pollfds, 4, 1000);
                    if (poll_result > 0)
                    {
                        read_sockets(pollfds, node);
                        
                        recieved_data = read_from_buf(node);
                        //Only break if we received the right PDU
                        if ((uint8_t)recieved_data[0] == NET_JOIN_RESPONSE)
                        {
                            break;
                        }
                    }
                }
                new_event = NET_JOIN_RESPONSE_EVENT;
            }
            break;
            /** State Q8
             *  In this state the node got a NET_JOIN_RESPONSE_PDU and can connect to the new successor
             */
            case Q8:
            {
                print_state("Q8");
                struct NET_JOIN_RESPONSE_PDU *pdu_nrj = deserialize_NET_JOIN_RESPONSE_PDU(recieved_data);
                free(recieved_data);
                node->range_start = pdu_nrj->range_start;
                node->range_end = pdu_nrj->range_end;
                node->max_span = pdu_nrj->range_end - pdu_nrj->range_start;
                hash_table = h_table_empty(0, 255);
                printf("Made a new hash table with range %u-%u\n", pdu_nrj->range_start, pdu_nrj->range_end);
                node->successor->addr.sin_family = AF_INET;
                node->successor->addr.sin_addr.s_addr = htonl(pdu_nrj->next_address);
                node->successor->addr.sin_port = htons(pdu_nrj->next_port);

                printf("    Connecting to successor V4(%s:%u)\n",
                       inet_ntoa(node->successor->addr.sin_addr), pdu_nrj->next_port);
                free(pdu_nrj);
                connect_to_successor(node);
                printf("    Connected to new succcesor V4(%s:%u)\n",
                       inet_ntoa(node->successor->addr.sin_addr), htons(node->successor->addr.sin_port));
                //Set poll file descriport #3 to successor
                pollfds[3].fd = node->successor->socket;

                new_event = KEEP_GOING_EVENT;
            }
            break;
            /** State Q9
             *  In this state the node got a VAL_INSERT_PDU, VAL_REMOVE_PDU or a VAL_LOOKUP_PDU and 
             *  must determine which one it was and appropriately handle the PDU. 
             */
            case Q9:
            {
                print_state("Q9");
                switch ((uint8_t)recieved_data[0])
                {
                case 100:
                    process_insert_request(hash_table, recieved_data, node);
                    break;
                case 101:
                    process_removal_request(hash_table, recieved_data, node);
                    break;
                case 102:
                    process_lookup_request(hash_table, recieved_data, node);
                    break;
                }
                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q10
             *  In this state the node got a signal to exit the network and must determine if it is alone in the 
             *  network or if it is connected
             */
            case Q10:
            {
                print_state("Q10");
                if (node->max_span == 255)
                {
                    printf("I am the last node\n");
                    new_event = exit_event;
                }
                else
                {
                    printf("\n");
                    new_event = NODE_CONNECTED_EVENT;
                }
            }
            break;
            /** State Q11
             *  In this state the nod wants to leave the network, but it's connected to other nodes
             *  so it have to send a NET_NEW_RAGE_PDU to its successor or predecessor
             */
            case Q11:
            {
                print_state("Q11");
                struct NET_NEW_RANGE_PDU pdu;
                pdu.type = NET_NEW_RANGE;
                pdu.range_start = node->range_start;
                pdu.range_end = node->range_end;
                char* data_to_send = seralize_NET_NEW_RANGE_PDU(&pdu);
                if (node->range_start == 0)
                {
                    printf("Sending NET_NEW_RANGE to successor\n");
                    send_tcp(node->successor, data_to_send, sizeof(pdu));
                }
                else
                {
                    printf("Sending NET_NEW_RANGE to predecessor\n");
                    send_tcp(node->predecessor, data_to_send, sizeof(pdu));
                }
                free(data_to_send);

                int poll_result = 0;
                while (1)
                {
                    poll_result = poll(pollfds, 4, 1000);
                    if (poll_result > 0)
                    {
                        read_sockets(pollfds, node);
                    }
                    recieved_data = read_from_buf(node);
                    if (recieved_data != NULL)
                    {
                        if ((uint8_t)recieved_data[0] == NET_NEW_RANGE_RESPONSE)
                        {
                            //Only break if the node recieve the right data
                            free(recieved_data);
                            break;
                        }
                        else if ((uint8_t)recieved_data[0] == VAL_INSERT)
                        {
                            printf("Not accepting VAL_INSERT at the moment, forwarding to successor.\n");
                            VAL_INSERT_PDU *pdu = deserialize_VAL_INSERT_PDU(recieved_data);
                            send(node->successor->socket,
                                 seralize_VAL_INSERT_PDU, 15 + pdu->email_length + pdu->name_length, 0);
                        }
                    }
                }

                new_event = NET_NEW_RANGE_RESPONSE_EVENT;
            }
            break;
            /** State Q12
             *  In this state the node received a NET_JOIN_PDU and must determine if this is the first time 
             *  the node tries to enter the network, or if the node have the maxium span or 
             *  if we are not the maximum node
             */
            case Q12:
            {
                printf("Q12\n");
                struct NET_JOIN_PDU *pdu_njQ12 = deserialize_NET_JOIN_PDU(recieved_data);
                if (node->max_span == 255)
                {
                    printf("    I am alone! Moving to [Q5]\n");
                    new_event = NODE_NOT_CONNECTED_EVENT;
                }
                else if (pdu_njQ12->max_port == htons(node->connector->addr.sin_port))
                {
                    printf("    I am the node with the maximum span! (%u)\n", node->max_span);
                    new_event = IF_NODE_EQUALS_MAX_NODE_EVENT;
                }
                else if (pdu_njQ12->max_span < node->max_span)
                {
                    pdu_njQ12->max_span = node->max_span;
                    pdu_njQ12->max_address = htonl(inet_addr(node->self_adress));
                    pdu_njQ12->max_port = ntohs(node->connector->addr.sin_port);
                    printf("    Updating max fields\n");
                    recieved_data = seralize_NET_JOIN_PDU(pdu_njQ12);
                    new_event = IF_NODE_NOT_EQUALS_MAX_NODE_EVENT;
                }
                else
                {
                    new_event = IF_NODE_NOT_EQUALS_MAX_NODE_EVENT;
                }
            }
            break;
            /** State Q13
             *  In this state the node received a NET_JOIN_PDU and determined that it was the maximum node
             */
            case Q13:
            {
                print_state("Q13");
                struct NET_JOIN_RESPONSE_PDU *pdu_njr = malloc_error(sizeof(pdu_njr));
                struct NET_CLOSE_CONNECTION_PDU pdu_cc;
                pdu_cc.type = NET_CLOSE_CONNECTION;
                pdu_njr->next_address = ntohl(node->successor->addr.sin_addr.s_addr);
                pdu_njr->next_port = ntohs(node->successor->addr.sin_port);

                send_tcp(node->successor, seralize_NET_CLOSE_CONNECTION_PDU(&pdu_cc), sizeof(pdu_cc));
                if (close(node->successor->socket) == -1)
                {
                    perror("close: ");
                    exit(1);
                }
                node->successor->socket = 0;
                pollfds[3].fd = 0;

                struct NET_JOIN_PDU *pdu = deserialize_NET_JOIN_PDU(recieved_data);
                node->successor->addr.sin_addr.s_addr = htonl(pdu->src_address);
                node->successor->addr.sin_port = htons(pdu->src_port);

                connect_to_successor(node);
                printf("Connected to new succcesor V4(%s:%u)\n", inet_ntoa(node->successor->addr.sin_addr), htons(node->successor->addr.sin_port));

                pollfds[3].fd = node->successor->socket;

                update_hash_range(node, pdu_njr);
                send_tcp(node->successor, seralize_NET_JOIN_RESPONSE_PDU(pdu_njr), 9);
                transfer_values(pdu_njr->range_start, pdu_njr->range_end, hash_table, 's', node);
                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q14
             *  In this state the node received a NET_JOIN_PDU and determined that it was not the
             *  node with the maximum span.
             */
            case Q14:
            {
                print_state("Q14");
                printf("Forwarding to successor\n");
                struct NET_JOIN_PDU *pdu = deserialize_NET_JOIN_PDU(recieved_data);
                pdu->type = NET_JOIN;
                send_tcp(node->successor, seralize_NET_JOIN_PDU(pdu), 14);

                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q15
             *  In this state the node received a NET_NEW_RAGE_PDU and updates its hash range
             *  and also send a NET_NEW_RAGE_RESPONSE_PDU.
             */
            case Q15:
            {
                print_state("Q15");
                struct NET_NEW_RANGE_PDU *pdu_nnr = deserialize_NET_NEW_RANGE_PDU(recieved_data);
                printf("Current range is: (%u, %u)\n", node->range_start, node->range_end);
                struct NET_NEW_RANGE_RESPONSE_PDU pdu_nnrr;
                pdu_nnrr.type = NET_NEW_RANGE_RESPONSE;
                if ((node->range_end != 255) && (pdu_nnr->range_start == node->range_end + 1))
                {
                    node->range_end = pdu_nnr->range_end;
                    printf("    Sending NET_NEW_RANGE_RESPONSE to successor\n");
                    send_tcp(node->successor, seralize_NET_NEW_RANGE_RESPONSE_PDU(&pdu_nnrr), 1);
                }
                else
                {
                    node->range_start = pdu_nnr->range_start;
                    printf("    Sending NET_NEW_RANGE_RESPONSE to predecessor\n");
                    send_tcp(node->predecessor, seralize_NET_NEW_RANGE_RESPONSE_PDU(&pdu_nnrr), 1);
                }
                node->max_span = node->range_end - node->range_start;
                printf("    New range is: (%u, %u)\n", node->range_start, node->range_end);
                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q16
             *  In this state, the node received a NET_LEAVING PDU And disconnects with its current successor
             *  to connect with a new one 
             * 
             */
            case Q16:
            {
                print_state("Q16");
                struct NET_LEAVING_PDU *pdu_nlp = deserialize_NET_LEAVING_PDU(recieved_data);
                if (close(node->successor->socket) == -1)
                {
                    perror("close: ");
                    exit(1);
                }
                if ((node->range_start == 0 && node->range_end == 255))
                {
                    printf("I am the last node.\n");
                    pollfds[3].fd = 0;
                }
                else
                {

                    node->successor->addr.sin_addr.s_addr = htonl(pdu_nlp->new_address);
                    node->successor->addr.sin_port = htons(pdu_nlp->new_port);
                    connect_to_successor(node);
                    printf("    Connected to new succcesor V4(%s:%u)\n",
                           inet_ntoa(node->successor->addr.sin_addr), htons(node->successor->addr.sin_port));
                    pollfds[3].fd = node->successor->socket;
                }

                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q17
             *
             *  In this state, the node received a NET_CLOSE_CONNECTION_PDU and disconnects from its 
             *  current predecessor to reconnect with the new predecessor. 
             */
            case Q17:
            {

                print_state("Q17");
                printf("Disconnection from predecessor\n");
                //close(node->predecessor->socket);
                if (close(node->predecessor->socket) == -1)
                {
                    perror("close: ");
                    exit(1);
                }
                pollfds[2].fd = 0;
                node->predecessor->socket = 0;
                if ((node->predecessor->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                {
                    perror("Socket Q17\n");
                    exit(1);
                }
                if (node->range_start == 0 && node->range_end == 255)
                {
                    printf("    I am the last node\n");
                }
                else
                {
                    printf("    Waiting for new predecessor\n");
                    accept_new_predecessor(pollfds, node);
                }

                new_event = BACK_TO_DEFAULT_EVENT;
            }
            break;
            /** State Q18:
             * 
             * Here the node is preparing to leave the network and makes necessary operations to leave successfully
             */
            case Q18:
            {
                print_state("Q18");

                struct NET_LEAVING_PDU pdu_nl;
                pdu_nl.type = NET_LEAVING;
                pdu_nl.new_address = ntohl(node->successor->addr.sin_addr.s_addr);
                pdu_nl.new_port = ntohs(node->successor->addr.sin_port);
                printf("Transferring all entries to successor\n");
                transfer_values(node->range_start, node->range_end, hash_table, 's', node);
                //To give the other node some time to read it's circular buffer                
                sleep(1);
                struct NET_CLOSE_CONNECTION_PDU pdu_cc;
                pdu_cc.type = NET_CLOSE_CONNECTION;
                char* data_to_send = seralize_NET_CLOSE_CONNECTION_PDU(&pdu_cc);
                send_tcp(node->successor, data_to_send, 1);
                free(data_to_send);
                if (close(node->successor->socket) == -1)
                {
                    perror("close");
                    exit(1);
                }
                node->successor->socket = 0;
                pollfds[3].fd = 0;
                printf("    Sending NET_LEAVING to predecessor\n");
                data_to_send = seralize_NET_LEAVING_PDU(&pdu_nl);
                if (send(node->predecessor->socket, data_to_send, 7, 0) == -1)
                {
                    perror("send");
                    exit(1);
                }
                free(data_to_send);
                if (close(node->predecessor->socket) == -1)
                {
                    perror("close");
                    exit(1);
                }
                pollfds[2].fd = 0;
                node->predecessor->socket = 0;
                new_event = exit_event;
            }
            break;
            /** State exit_state
             *  
             *  Destroy the node;
             */
            case exit_state:
            {
                cleanup(node, hash_table);
                printf("Goodbye :)\n");
                exit(0);
            }
            case last_state:
            {
            }
            break;
            }
        }
        else
        {
            break;
        }
    }
    return 0;
}
void cleanup(node_struct *node, h_table *h){
    printf("1\n");
    dlist* list = h_table_kill(h);
    printf("2\n");
    if(list != NULL){
        printf("3\n");
        cell* c = dlist_head(list);
        printf("4\n");
        struct VAL_INSERT_PDU* pdu;
        while(!dlist_is_last(dlist_head(list), list)){
            printf("5\n");
            c = dlist_inspect(c, list);
            printf("6\n");
            pdu = (struct VAL_INSERT_PDU*)c->value;
            printf("7\n");
            if(c != NULL){
                printf("pdu : %u\n", (uint8_t)pdu->name_length);
                printf("8\n");
                c = dlist_next(c, list);
                printf("9\n");

            }else{
                break;
            }
        }
        dlist_kill(list);
    }
    printf("3\n");
    free(node->connector);
    free(node->predecessor);
    free(node->tracker);
    free(node->udp);
    free(node->successor);
    free(node);

}
/*
 * Function - connect_to_successor
 * 
 * Description - Connects our the successor socket to a new successor socket
 * 
 * Input:
 *        *node - The node_struct containing the successor socket
 * Output:
 */
void connect_to_successor(node_struct *node)
{

    if ((node->successor->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error");
        exit(1);
    }

    if ((connect(node->successor->socket, (struct sockaddr *)&node->successor->addr, sizeof(node->successor->addr)) == -1))
    {
        perror("connect: ");
        exit(1);
    }
}
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
char *read_from_buf(node_struct *node)
{
    int return_value = 0;

    return_value = check_size(&node->circ_buf.circular_buffer[node->circ_buf.read_index]);
    if (return_value == -1)
    {
        node->circ_buf.read_index = node->circ_buf.write_index;
        return NULL;
    }
    else
    {
        char *buf = malloc_error(sizeof(char) * return_value);
        for (int i = 0; i < return_value; i++)
        {
            if (node->circ_buf.read_index > node->circ_buf.last_change)
            {
                node->circ_buf.read_index = 0;
            }
            buf[i] = node->circ_buf.circular_buffer[node->circ_buf.read_index++];
        }

        return buf;
    }
}

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
void update_hash_range(node_struct *node, struct NET_JOIN_RESPONSE_PDU *pdu)
{

    uint8_t min_predecessor = node->range_start;
    uint8_t max_successor = node->range_end;
    uint8_t max_predecessor = ((node->range_end - node->range_start) / 2) + node->range_start;
    uint8_t min_successor = max_predecessor + 1;

    node->range_start = min_predecessor;
    node->range_end = max_predecessor;
    node->max_span = max_predecessor - min_predecessor;
    printf("    Other hash-range is %u-%u\n", min_successor, max_successor);
    printf("    New hash-range is %u-%u\n", min_predecessor, max_predecessor);
    pdu->type = NET_JOIN_RESPONSE;
    pdu->range_start = min_successor;
    pdu->range_end = max_successor;
}
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
e_system_state parse_buffer(char *buffer)
{
    switch ((uint8_t)buffer[0])
    {
    case 3:
        return NET_JOIN_EVENT;
    case 5:
        return NET_CLOSE_CONNECTION_EVENT;
    case 6:
        return NET_NEW_RAGE_EVENT;
    case 7:
        return NET_LEAVING_PDU_EVENT;
    case 100:
        return VAL_INSERT_EVENT;
    case 101:
        return REMOVE_EVENT;
    case 102:
        return LOOKUP_EVENT;
    default:
        buffer = NULL;
        return BACK_TO_DEFAULT_EVENT;
    }
}
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
int check_size(char *buffer)
{
    switch ((uint8_t)buffer[0])
    {
    case 3:
    {
        return 14;
        break;
    }
    case 4:
    {
        return 9;
        break;
    }
    case 5:
    {
        return 1;
        break;
    }
    case 6:
    {
        return 3;
        break;
    }
    case 7:
    {
        return 7;
        break;
    case 8:
    {
        return 1;
        break;
    }
    }
    case 100:
    {
        struct VAL_INSERT_PDU *pdu = deserialize_VAL_INSERT_PDU(buffer);
        return 15 + pdu->name_length + pdu->email_length;
        break;
    }
    case 101:
    {
        return 13;
        break;
    }
    case 102:
    {
        return 19;
        break;
    }
    default:
    {
        return -1;
        break;
    }
    }
}
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
void process_lookup_request(h_table *h, char *buffer, node_struct *node)
{

    struct VAL_LOOKUP_PDU *pdu = deserialize_VAL_LOOKUP_PDU(buffer);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(pdu->sender_address);
    addr.sin_port = htons(pdu->sender_port);
    if (in_hash_range((char *)pdu->ssn, node))
    {
        struct VAL_LOOKUP_RESPONSE_PDU *value = (struct VAL_LOOKUP_RESPONSE_PDU *)h_table_lookup(h, (char *)pdu->ssn);
        if (value != NULL)
        {
            value->type = 103;
            char *data = seralize_VAL_LOOKUP_RESPONSE_PDU(value);
            printf("Value found (ssn: ");
            for (int i = 0; i < SSN_LENGTH; i++)
            {
                printf("%c", pdu->ssn[i]);
            }
            printf(").\n");
            if (sendto(node->udp->socket, data, 15 + value->email_length + value->name_length,
                       0, (struct sockaddr *)&addr, sizeof(addr)) == -1)
            {
                perror("sendto: ");
                exit(1);
            }
        }
        else
        {
            char *data = calloc_error(1, 17);
            data[0] = 103;
            printf("In hash range but value not found, sending empty response\n");
            if (sendto(node->udp->socket, data, 17, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1)
            {
                perror("sendto: ");
                exit(1);
            }
        }
    }
    else
    {
        char *data = seralize_VAL_LOOKUP_PDU(pdu);
        send_tcp(node->successor, data, 19);
        printf("    Forwarding VAL_LOOKUP to successor\n");
    }
}
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
void transfer_values(uint8_t low_boundry, uint8_t upper_boundry, h_table *h, char x, node_struct *node)
{

    dlist *list_of_values = h_table_get_values(low_boundry, upper_boundry, h);
    struct VAL_INSERT_PDU *pdu;
    char *data;

    if (list_of_values != NULL)
    {
        while (!dlist_is_empty(list_of_values))
        {
            pdu = (struct VAL_INSERT_PDU *)dlist_inspect(dlist_head(list_of_values), list_of_values);
            if (pdu != NULL)
            {
                data = seralize_VAL_INSERT_PDU(pdu);
                if (x == 'p')
                {
                    send_tcp(node->predecessor, data, 15 + pdu->name_length + pdu->email_length);
                }
                else if (x == 's')
                {
                    send_tcp(node->successor, data, 15 + pdu->name_length + pdu->email_length);
                }
            }
            dlist_remove(dlist_head(list_of_values), list_of_values);
        }
    }
    dlist_kill(list_of_values);
}
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
void process_removal_request(h_table *h, char *buffer, node_struct *node)
{

    struct VAL_REMOVE_PDU *pdu = deserialize_VAL_REMOVE_PDU(buffer);

    if (in_hash_range((char *)pdu->ssn, node))
    {
        h_table_remove(h, (char *)pdu->ssn);
        printf("Entry removed succesfully\n");
    }
    else
    {
        send(node->successor->socket, buffer, sizeof(struct VAL_REMOVE_PDU), 0);
        printf("    Forwarding VAL_REMOVAL to successor\n");
    }
}
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
void process_insert_request(h_table *h, char *buffer, node_struct *node)
{
    struct VAL_INSERT_PDU *pdu;
    pdu = deserialize_VAL_INSERT_PDU(buffer);
    if (in_hash_range((char *)pdu->ssn, node))
    {
        h_table_insert(h, (char *)pdu->ssn, pdu);
        print_VAL_INSERT_PDU(pdu);
    }
    else
    {
        char *data = seralize_VAL_INSERT_PDU(pdu);
        send_tcp(node->successor, data, 15 + pdu->name_length + pdu->email_length);
        printf("    Forwarding VAL_INSERT to successor\n");
    }
}
/*
 * Function - print_VAL_INSERT_PDU
 * 
 * Description - Prints the VAL_INSERT_PDU
 * 
 * Input:
 *        *pdu - The PDU to print
 * Output:
 */
void print_VAL_INSERT_PDU(struct VAL_INSERT_PDU *pdu)
{

    printf("    Inserting ssn Entry { ssn: \"");
    for (int i = 0; i < SSN_LENGTH; i++)
    {
        printf("%c", pdu->ssn[i]);
    }
    printf("\", name: \"");
    for (int i = 0; i < pdu->name_length; i++)
    {
        printf("%c", pdu->name[i]);
    }
    printf("\", email: \"");
    for (int i = 0; i < pdu->email_length; i++)
    {
        printf("%c", pdu->email[i]);
    }
    printf("\" };\n");
}
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
bool in_hash_range(char ssn[SSN_LENGTH], node_struct *node)
{
    hash_t hashed_key = hash_ssn(ssn);

    if (hashed_key >= node->range_start && hashed_key <= node->range_end)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*
 * Function - get_STUN_RESPONSE
 * 
 * Description - Reads from the UDP socket and determines if it's a STUN_RESPOSNE_PDU
 * 
 * Input:
 *        *node - The node struct containing the UDP socket and tracker address
 * Output:
 */
void get_STUN_RESPONSE(node_struct *node)
{
    char *buf2 = recieve_UDP(node);
    struct STUN_RESPONSE_PDU *pdu_sr = deserialize_STUN_RESPONSE_PDU(buf2);

    while (1)
    {
        if (pdu_sr->type == STUN_RESPONSE)
        {
            if (pdu_sr->address != 0)
            {

                char ip2[16];
                inet_ntop(AF_INET, &pdu_sr->address, ip2, 16);
                printf("Got STUN_RESPONSE my address is %s\n", ip2);
                strncpy(node->self_adress, ip2, 16);
                break;
            }
        }
    }
    free(buf2);
    free(pdu_sr);
}
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
bool handle_NET_GET_NODE_RESPONSE(struct NET_GET_NODE_RESPONSE_PDU *pdu, node_struct *node)
{
    if (pdu->type == NET_GET_NODE_RESPONSE)
    {
        if (pdu->address == 0)
        {
            printf("I am the first node to join the network\n");
            return true;
        }
        char ip2[16];
        inet_ntop(AF_INET, &pdu->address, ip2, 16);

        node->predecessor->addr.sin_family = AF_INET;
        node->predecessor->addr.sin_addr.s_addr = inet_addr(ip2);
        node->predecessor->addr.sin_port = htons(pdu->port);
        printf("\n");
        return false;
    }
    else
    {
        return NULL;
    }
}
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
void accept_new_predecessor(struct pollfd *pollfds, node_struct *node)
{
    int poll_result = 0;
    while (1)
    {
        poll_result = poll(pollfds, 1, 1000);
        if (poll_result > 0)
        {

            if (pollfds[0].revents & POLLIN)
            {
                struct sockaddr_in cliaddr;
                socklen_t addrlen = sizeof(cliaddr);
                int client_socket = accept(node->connector->socket, (struct sockaddr *)&cliaddr, &addrlen);
                printf("    Accepted new predecessor %s:%u\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                if (client_socket == -1)
                {
                    perror("accept error");
                    exit(EXIT_FAILURE);
                }
                for (int i = 2; i < MAX_CLIENTS; i++)
                {
                    if (pollfds[i].fd == 0)
                    {

                        pollfds[i].fd = client_socket;
                        node->predecessor->socket = pollfds[i].fd;
                        pollfds[i].events |= POLLIN;
                        break;
                    }
                }
            }
            break;
        }
    }
}
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
void read_sockets(struct pollfd *pollfds, node_struct *node)
{
    char *buf = calloc_error(BUFSIZE, 1);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        while (pollfds[i].revents & POLLIN)
        {
            int nr_of_bytes = read(pollfds[i].fd, buf, BUFSIZE);
            if (nr_of_bytes == -1)
            {
                perror("read: ");
                exit(1);
            }
            pollfds[i].revents = 0;
            write_to_buf(buf, nr_of_bytes, node);

        }
    }
    free(buf);
}
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
socket_struct *init_socket_struct()
{
    socket_struct *ss = calloc_error(1, sizeof(*ss));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    ss->addr = addr;
    ss->addr.sin_family = AF_INET;
    ss->addrlen = sizeof(addr);
    return ss;
}
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
void print_state(char *buf)
{
    printf("[%s]\n", buf);
    printf("    ");
}
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
char *recieve_UDP(node_struct *node)
{
    char *buffer = calloc(BUFSIZE, sizeof(char));
    if (recvfrom(node->udp->socket, buffer, BUFSIZE, 0,
                 (struct sockaddr *)&node->udp->addr, &node->udp->addrlen) == -1)
    {
        perror("recvfrom: ");
        exit(1);
    }
    return buffer;
}
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
void send_UDP(node_struct *node, char *buffer, int nr_of_bytes)
{
    if (sendto(node->udp->socket, buffer, nr_of_bytes, 0,
               (struct sockaddr *)&node->tracker->addr, sizeof(node->tracker->addr)) == -1)
    {
        perror("sendto: ");
        exit(1);
    }
}
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
void create_UDP_socket(socket_struct *ss)
{

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1)
    {
        perror("UDP socket: ");
        exit(1);
    }
    ss->socket = s;
    ss->addr.sin_family = AF_INET;
    ss->addr.sin_addr.s_addr = INADDR_ANY;
    ss->addr.sin_port = 0;

    if (bind(ss->socket, (struct sockaddr *)&ss->addr, sizeof(ss->addr)) == -1)
    {
        perror("Bind: ");
        exit(1);
    }

    if (getsockname(ss->socket, (struct sockaddr *)&ss->addr, &ss->addrlen) == -1)
    {
        perror("Getsockname: ");
        exit(1);
    }
    else
    {
        printf("Node listening on UDP v4(%s:%d)", inet_ntoa(ss->addr.sin_addr),
               ntohs(ss->addr.sin_port));
    }
    return;
}
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
void connect_TCP_socket(socket_struct *ss, uint32_t ip, uint16_t port)
{

    if ((ss->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket: ");
        exit(1);
    }
    ss->addr.sin_family = AF_INET;
    ss->addr.sin_addr.s_addr = htonl(ip);
    ss->addr.sin_port = htons(port);

    if ((bind(ss->socket, (struct sockaddr *)&ss->addr, sizeof(ss->addr))) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(ss->socket, 5) == -1)
    {
        perror("listen");
    }
    if (getsockname(ss->socket, (struct sockaddr *)&ss->addr, &ss->addrlen) == -1)
    {
        perror("Getsockname: ");
        exit(1);
    }

    printf(", accepts TCP connections on v4(%s:%d)\n", inet_ntoa(ss->addr.sin_addr),
           ntohs(ss->addr.sin_port));

    return;
}
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
void *calloc_error(int size, ssize_t size_of_type)
{
    void *p;
    if ((p = calloc(size, size_of_type)) == NULL)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    return p;
}
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
void send_alive(node_struct *node)
{
    struct NET_ALIVE_PDU pdu_na;
    pdu_na.type = NET_ALIVE;
    char *buf = seralize_NET_ALIVE_PDU(&pdu_na);
    send_UDP(node, buf, 1);
    free(buf);
}
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
node_struct *init_node(char *ip, uint16_t port_no)
{
    node_struct *node = calloc_error(1, sizeof(*node));
    socket_struct *UDP_socket = init_socket_struct();
    create_UDP_socket(UDP_socket);

    socket_struct *tracker = init_socket_struct();

    tracker->addrlen = sizeof(tracker->addr);
    tracker->addr.sin_family = AF_INET;
    tracker->addr.sin_addr.s_addr = inet_addr(ip);
    tracker->addr.sin_port = htons(port_no);

    socket_struct *connector_socket = init_socket_struct();
    connect_TCP_socket(connector_socket, INADDR_ANY, 0);

    socket_struct *successor_sock = init_socket_struct();
    socket_struct *predecessor_sock = init_socket_struct();
    node->connector = connector_socket;
    node->udp = UDP_socket;
    node->successor = successor_sock;
    node->predecessor = predecessor_sock;
    node->tracker = tracker;
    node->max_span = 255;
    node->range_start = 0;
    node->range_end = 255;
    node->circ_buf.read_index = 0;
    node->circ_buf.write_index = 0;
    node->circ_buf.last_change = 20480;
    initiate_signal_handler(node);
    return node;
}
/*
 * Function - handler
 * 
 * Description - The signal handler
 * 
 * Input:
 *        num  - arbitary int
 * Output:
 */
void handler(int num)
{
    write(STDOUT_FILENO, "\n", 1);
    interrupted = true;
}
/*
 * Function - initiate_signal_handler
 * 
 * Description - Initiates the signal handler function 
 * 
 * Input:
 *        *node  - The node_struct that the corresponding set will be inserted  
 * Output:
 */
void initiate_signal_handler(node_struct *node)
{
    sigset_t set;
    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigaction(SIGINT, &sa, NULL);
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    node->set = set;
}
/*
 * Function - block_signals
 * 
 * Description - blocks incoming interrupt signals
 * 
 * Input:
 *        *node  - The node_struct containing the corresponding sigset  
 * Output:
 */
void block_signals(node_struct *node)
{
    if (sigprocmask(SIG_BLOCK, &node->set, NULL) == -1)
    {
        perror("sigprocmask: ");
        exit(1);
    }
}
/*
 * Function - unblock_signals
 * 
 * Description - Unblocks incoming interrupt signals
 * 
 * Input:   
 *        *node  - The node_struct containing the corresponding sigset
 * Output:           
 */
void unblock_signals(node_struct *node)
{
    if (sigprocmask(SIG_UNBLOCK, &node->set, NULL) == -1)
    {
        perror("sigprocmask: ");
        exit(1);
    }
}
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
void write_to_buf(char *buf, int size, node_struct *node)
{
    for (int i = 0; i < size; i++)
    {
        node->circ_buf.circular_buffer[node->circ_buf.write_index++] = buf[i];
    }
    if (node->circ_buf.write_index >= 18000)
    {
        node->circ_buf.last_change = node->circ_buf.write_index;
        node->circ_buf.write_index = 0;
    }
}
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
void send_tcp(socket_struct *ss, char *buf, size_t size)
{
    if (send(ss->socket, buf, size, 0) == -1)
    {
        perror("Send:");
        exit(1);
    }
}