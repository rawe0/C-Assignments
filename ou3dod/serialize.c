/*
* Computer Networks.
* Autumn 2021
* Assignment 3
* File:         serialize.c
* Description:  Functions to serialize and de-serialize data.
*
* Author:       Rasmus Welander
* CS username:  c19rwr@cs.umu.se
* Author:       Marcus Rost
*
* CS username:  bio16mrt@cs.umu.se
* Date:         2021-10-28
*/

#include "serialize.h"
#include "pdu.h"
#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Function - create_lookup_response
 * 
 * Description - Creates a VAl_LOOKUP_RESPONSE_PDU
 * 
 * Input:
 *          name - name to be used
 *          name_length - length of the name string
 *          email - email to be used
 *          email_length - length of the email string
 *          ssn - A social security number, has to be exactly 12 characters long
 * Output:
 *          returns a pointer to the VAL_LOOKUP_RESPONSE_PDU
 **/
struct VAL_LOOKUP_RESPONSE_PDU *create_lookup_response(char *name, int name_length, char *email,
                                                       int email_length, char ssn[SSN_LENGTH])
{

    struct VAL_LOOKUP_RESPONSE_PDU *pdu = malloc(sizeof(struct VAL_LOOKUP_RESPONSE_PDU));
    pdu->type = VAL_LOOKUP_RESPONSE;
    pdu->email = malloc(sizeof(email_length));
    pdu->email_length = email_length;
    pdu->name = malloc(sizeof(name_length));
    pdu->name_length = name_length;
    for (int i = 0; i < SSN_LENGTH; i++)
    {
        pdu->ssn[i] = ssn[i];
    }
    return pdu;
}
/* Function - free_VAL_LOOKUP_RESPONSE_PDU
 * 
 * Description  - frees the memeory used by the VAL_LOOKUP_RESPONSE_PDU struct
 * 
 * Input:
 *          pdu - a pointer to the struct that is to be freed
 * Output:
 **/
void free_VAL_LOOKUP_RESPONSE_PDU(struct VAL_LOOKUP_RESPONSE_PDU *pdu)
{
    free(pdu->email);
    free(pdu->name);
    free(pdu);
}
/* Function - free_VAL_LOOKUP_RESPONSE_PDU
 * 
 * Description  - frees the memeory used by the VAL_LOOKUP_RESPONSE_PDU struct
 * 
 * Input:
 *          pdu - a pointer to the struct that is to be freed
 * Output:
 **/
void free_VAL_INSERT_PDU(struct VAL_INSERT_PDU *pdu)
{
    free(pdu->email);
    free(pdu->name);
    free(pdu);
}

/* Function - seralize_NET_ALIVE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_ALIVE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_ALIVE_PDU(struct NET_ALIVE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_JOIN_PDU));

    memcpy(&buffer[0], &pdu->type, 1);

    return buffer;
}
/* Function - deserialize_NET_ALIVE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_ALIVE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_ALIVE_PDU
 **/
struct NET_ALIVE_PDU *deserialize_NET_ALIVE_PDU(char *buffer)
{

    struct NET_ALIVE_PDU *pdu = malloc_error(sizeof(struct NET_ALIVE_PDU));
    uint8_t type;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    return pdu;
}
/* Function - seralize_NET_GET_NODE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_GET_NODE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_GET_NODE_PDU(struct NET_GET_NODE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_GET_NODE_PDU));

    memcpy(&buffer[0], &pdu->type, 1);

    return buffer;
}
/* Function - deserialize_NET_GET_NODE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_GET_NODE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_GET_NODE_PDU
 **/
struct NET_GET_NODE_PDU *deserialize_NET_GET_NODE_PDU(char *buffer)
{

    struct NET_GET_NODE_PDU *pdu = malloc_error(sizeof(struct NET_GET_NODE_PDU));
    uint8_t type;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    return pdu;
}
/* Function - seralize_NET_GET_NODE_RESPONSE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_GET_NODE_RESPONSE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_GET_NODE_RESPONSE_PDU(struct NET_GET_NODE_RESPONSE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_GET_NODE_RESPONSE_PDU));
    uint32_t address = htonl(pdu->address);
    uint16_t port = htons(pdu->port);

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], &address, 4);
    memcpy(&buffer[5], &port, 2);

    return buffer;
}
/* Function - deserialize_NET_GET_NODE_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_GET_NODE_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_GET_NODE_RESPONSE_PDU
 **/
struct NET_GET_NODE_RESPONSE_PDU *deserialize_NET_GET_NODE_RESPONSE_PDU(char *buffer)
{

    struct NET_GET_NODE_RESPONSE_PDU *pdu = malloc_error(sizeof(struct NET_GET_NODE_RESPONSE_PDU));
    uint8_t type;
    uint32_t address;
    uint16_t port;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(&address, &buffer[1], 4);
    pdu->address = address;

    memcpy(&port, &buffer[5], 2);
    pdu->port = ntohs(port);

    return pdu;
}
/* Function - seralize_NET_JOIN_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_JOIN_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_JOIN_PDU(struct NET_JOIN_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_JOIN_PDU));
    uint32_t src_address = htonl(pdu->src_address);
    uint16_t src_port = htons(pdu->src_port);
    uint32_t max_address = htonl(pdu->max_address);
    uint16_t max_port = htons(pdu->max_port);
    uint8_t type = NET_JOIN;
    memcpy(&buffer[0], &type, 1);
    memcpy(&buffer[1], &src_address, 4);
    memcpy(&buffer[5], &src_port, 2);
    memcpy(&buffer[7], &pdu->max_span, 1);
    memcpy(&buffer[8], &max_address, 4);
    memcpy(&buffer[12], &max_port, 2);

    return buffer;
}
/* Function - deserialize_NET_JOIN_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_JOIN_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_JOIN_PDU
 **/
struct NET_JOIN_PDU *deserialize_NET_JOIN_PDU(char *buffer)
{

    struct NET_JOIN_PDU *pdu = malloc_error(sizeof(struct NET_JOIN_PDU));
    uint8_t type;
    uint32_t src_address;
    uint16_t src_port;
    uint8_t max_span;
    uint32_t max_address;
    uint16_t max_port;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(&src_address, &buffer[1], 4);
    pdu->src_address = htonl(src_address);

    memcpy(&src_port, &buffer[5], 2);
    pdu->src_port = ntohs(src_port);

    memcpy(&max_span, &buffer[7], 1);
    pdu->max_span = max_span;

    memcpy(&max_address, &buffer[8], 4);
    pdu->max_address = ntohl(max_address);

    memcpy(&max_port, &buffer[12], 2);
    pdu->max_port = ntohs(max_port);

    return pdu;
}

/* Function - seralize_NET_JOIN_RESPONSE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_JOIN_RESPONSE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_JOIN_RESPONSE_PDU(struct NET_JOIN_RESPONSE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_JOIN_RESPONSE_PDU));
    uint32_t next_address = htonl(pdu->next_address);
    uint16_t next_port = htons(pdu->next_port);

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], &next_address, 4);
    memcpy(&buffer[5], &next_port, 2);
    memcpy(&buffer[7], &pdu->range_start, 1);
    memcpy(&buffer[8], &pdu->range_end, 1);

    return buffer;
}
/* Function - deserialize_NET_JOIN_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_JOIN_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_JOIN_RESPONSE_PDU
 **/
struct NET_JOIN_RESPONSE_PDU *deserialize_NET_JOIN_RESPONSE_PDU(char *buffer)
{

    struct NET_JOIN_RESPONSE_PDU *pdu = malloc_error(sizeof(struct NET_JOIN_RESPONSE_PDU));
    uint8_t type;
    uint32_t next_address;
    uint16_t next_port;
    uint8_t range_start;
    uint8_t range_end;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(&next_address, &buffer[1], 4);
    pdu->next_address = ntohl(next_address);

    memcpy(&next_port, &buffer[5], 2);
    pdu->next_port = ntohs(next_port);

    memcpy(&range_start, &buffer[7], 1);
    pdu->range_start = range_start;

    memcpy(&range_end, &buffer[8], 1);
    pdu->range_end = range_end;

    return pdu;
}
/* Function - seralize_NET_CLOSE_CONNECTION_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the seralize_NET_CLOSE_CONNECTION_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_CLOSE_CONNECTION_PDU(struct NET_CLOSE_CONNECTION_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_CLOSE_CONNECTION_PDU));

    memcpy(&buffer[0], &pdu->type, 1);

    return buffer;
}

/* Function - deserialize_NET_CLOSE_CONNECTION_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_CLOSE_CONNECTION_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_CLOSE_CONNECTION_PDU
 **/
struct NET_CLOSE_CONNECTION_PDU *deserialize_NET_CLOSE_CONNECTION_PDU(char *buffer)
{

    struct NET_CLOSE_CONNECTION_PDU *pdu = malloc_error(sizeof(struct NET_CLOSE_CONNECTION_PDU));

    uint8_t type;
    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    return pdu;
}

/* Function - seralize_NET_NEW_RANGE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_NEW_RANGE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_NEW_RANGE_PDU(struct NET_NEW_RANGE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_NEW_RANGE_PDU));

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], &pdu->range_start, 1);
    memcpy(&buffer[2], &pdu->range_end, 1);

    return buffer;
}
/* Function - deserialize_NET_NEW_RANGE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_NEW_RANGE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_NEW_RANGE_PDU
 **/
struct NET_NEW_RANGE_PDU *deserialize_NET_NEW_RANGE_PDU(char *buffer)
{

    struct NET_NEW_RANGE_PDU *pdu = malloc_error(sizeof(struct NET_NEW_RANGE_PDU));

    uint8_t type;
    uint8_t range_start;
    uint8_t range_end;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(&range_start, &buffer[1], 1);
    pdu->range_start = range_start;

    memcpy(&range_end, &buffer[2], 1);
    pdu->range_end = range_end;

    return pdu;
}

/* Function - seralize_NET_NEW_RANGE_RESPONSE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the seralize_NET_NEW_RANGE_RESPONSE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_NEW_RANGE_RESPONSE_PDU(struct NET_NEW_RANGE_RESPONSE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_NEW_RANGE_RESPONSE_PDU));

    memcpy(&buffer[0], &pdu->type, 1);

    return buffer;
}

/* Function - deserialize_NET_NEW_RANGE_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_NEW_RANGE_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_NEW_RANGE_RESPONSE_PDU
 **/
struct NET_NEW_RANGE_RESPONSE_PDU *deserialize_NET_NEW_RANGE_RESPONSE_PDU(char *buffer)
{

    struct NET_NEW_RANGE_RESPONSE_PDU *pdu = malloc_error(sizeof(struct NET_NEW_RANGE_RESPONSE_PDU));
    uint8_t type;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    return pdu;
}
/* Function - seralize_NET_LEAVING_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the NET_LEAVING_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_NET_LEAVING_PDU(struct NET_LEAVING_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct NET_LEAVING_PDU));
    uint32_t new_address = htonl(pdu->new_address);
    uint16_t new_port = htons(pdu->new_port);

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], &new_address, 4);
    memcpy(&buffer[5], &new_port, 2);

    return buffer;
}
/* Function - deserialize_NET_LEAVING_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_LEAVING_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_LEAVING_PDU
 **/
struct NET_LEAVING_PDU *deserialize_NET_LEAVING_PDU(char *buffer)
{

    struct NET_LEAVING_PDU *pdu = malloc_error(sizeof(struct NET_LEAVING_PDU));

    uint8_t type;
    uint32_t new_address;
    uint16_t new_port;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(&new_address, &buffer[1], 4);
    pdu->new_address = ntohl(new_address);

    memcpy(&new_port, &buffer[5], 2);
    pdu->new_port = ntohs(new_port);

    return pdu;
}
/* Function - seralize_VAL_INSERT_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the VAL_INSERT_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_VAL_INSERT_PDU(struct VAL_INSERT_PDU *pdu)
{

    char *buffer = malloc_error(pdu->name_length + pdu->email_length + SSN_LENGTH + 3);
    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], pdu->ssn, SSN_LENGTH);
    memcpy(&buffer[SSN_LENGTH + 1], &pdu->name_length, 1);
    memcpy(&buffer[SSN_LENGTH + 2], pdu->name, pdu->name_length);
    memcpy(&buffer[SSN_LENGTH + pdu->name_length + 2], &pdu->email_length, 1);
    memcpy(&buffer[SSN_LENGTH + pdu->name_length + 3], pdu->email, pdu->email_length);
    return buffer;
}

/* Function - deserialize_VAL_INSERT_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_INSERT_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_INSERT_PDU
 **/
struct VAL_INSERT_PDU *deserialize_VAL_INSERT_PDU(char *buffer)
{

    struct VAL_INSERT_PDU *pdu = malloc_error(sizeof(struct VAL_INSERT_PDU));

    uint8_t type;
    uint8_t name_length;
    uint8_t *name;
    uint8_t email_length;
    uint8_t *email;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(pdu->ssn, &buffer[1], 12);

    memcpy(&name_length, &buffer[SSN_LENGTH + 1], 1);
    pdu->name_length = name_length;
    name = malloc_error(name_length);

    memcpy(name, &buffer[SSN_LENGTH + 2], name_length);
    pdu->name = name;

    memcpy(&email_length, &buffer[SSN_LENGTH + name_length + 2], 1);
    pdu->email_length = email_length;
    email = malloc_error(email_length);

    memcpy(email, &buffer[SSN_LENGTH + name_length + 3], email_length);
    pdu->email = email;

    return pdu;
}
/* Function - seralize_VAL_REMOVE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the VAL_REMOVE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_VAL_REMOVE_PDU(struct VAL_REMOVE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct VAL_REMOVE_PDU));
    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], pdu->ssn, SSN_LENGTH);
    return buffer;
}
/* Function - deserialize_VAL_REMOVE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_REMOVE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_REMOVE_PDU
 **/
struct VAL_REMOVE_PDU *deserialize_VAL_REMOVE_PDU(char *buffer)
{

    struct VAL_REMOVE_PDU *pdu = malloc_error(sizeof(struct VAL_REMOVE_PDU));

    uint8_t type;
    memcpy(&type, &buffer[0], 1);
    pdu->type = type;
    memcpy(pdu->ssn, &buffer[1], SSN_LENGTH);

    return pdu;
}
/* Function - seralize_VAL_LOOKUP_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the VAL_LOOKUP_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_VAL_LOOKUP_PDU(struct VAL_LOOKUP_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct VAL_LOOKUP_PDU));
    uint32_t sender_address = htonl(pdu->sender_address);
    uint16_t sender_port = htons(pdu->sender_port);

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], pdu->ssn, SSN_LENGTH);
    memcpy(&buffer[SSN_LENGTH + 1], &sender_address, 4);
    memcpy(&buffer[SSN_LENGTH + 5], &sender_port, 2);

    return buffer;
}
/* Function - deserialize_VAL_LOOKUP_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_LOOKUP_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_LOOKUP_PDU
 **/
struct VAL_LOOKUP_PDU *deserialize_VAL_LOOKUP_PDU(char *buffer)
{

    struct VAL_LOOKUP_PDU *pdu = malloc_error(sizeof(struct VAL_LOOKUP_PDU));

    uint8_t type;
    uint32_t sender_address;
    uint16_t sender_port;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(pdu->ssn, &buffer[1], SSN_LENGTH);

    memcpy(&sender_address, &buffer[SSN_LENGTH + 1], 4);
    pdu->sender_address = ntohl(sender_address);

    memcpy(&sender_port, &buffer[SSN_LENGTH + 5], 2);
    pdu->sender_port = ntohs(sender_port);

    return pdu;
}
/* Function - seralize_VAL_LOOKUP_RESPONSE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the VAL_LOOKUP_RESPONSE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_VAL_LOOKUP_RESPONSE_PDU(struct VAL_LOOKUP_RESPONSE_PDU *pdu)
{
    char *buffer = malloc_error(pdu->name_length + pdu->email_length + SSN_LENGTH + 4);

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], pdu->ssn, SSN_LENGTH);
    memcpy(&buffer[SSN_LENGTH + 1], &pdu->name_length, 1);
    memcpy(&buffer[SSN_LENGTH + 2], pdu->name, pdu->name_length);
    memcpy(&buffer[SSN_LENGTH + pdu->name_length + 2], &pdu->email_length, 1);
    memcpy(&buffer[SSN_LENGTH + pdu->name_length + 3], pdu->email, pdu->email_length);

    return buffer;
}
/* Function - deserialize_VAL_LOOKUP_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_LOOKUP_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_LOOKUP_RESPONSE_PDU
 **/
struct VAL_LOOKUP_RESPONSE_PDU *deserialize_VAL_LOOKUP_RESPONSE_PDU(char *buffer)
{

    struct VAL_LOOKUP_RESPONSE_PDU *pdu =
        malloc_error(sizeof(struct VAL_LOOKUP_RESPONSE_PDU));

    uint8_t type;
    uint8_t name_length;
    uint8_t *name;
    uint8_t email_length;
    uint8_t *email;

    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(pdu->ssn, &buffer[1], 12);

    memcpy(&name_length, &buffer[SSN_LENGTH + 1], 1);
    pdu->name_length = name_length;

    name = malloc_error(sizeof(name_length));

    memcpy(name, &buffer[SSN_LENGTH + 2], name_length);
    pdu->name = name;

    memcpy(&email_length, &buffer[SSN_LENGTH + name_length + 2], 1);
    pdu->email_length = email_length;

    email = malloc_error(sizeof(email_length));

    memcpy(email, &buffer[SSN_LENGTH + name_length + 3], email_length);
    pdu->email = email;

    return pdu;
}
/* Function - seralize_STUN_LOOKUP_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the seralize_STUN_LOOKUP_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_STUN_LOOKUP_PDU(struct STUN_LOOKUP_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct STUN_LOOKUP_PDU));

    memcpy(&buffer[0], &pdu->type, 1);

    return buffer;
}
/* Function - deserialize_STUN_LOOKUP_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a STUN_LOOKUP_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the STUN_LOOKUP_PDU
 **/
struct STUN_LOOKUP_PDU *deserialize_STUN_LOOKUP_PDU(char *buffer)
{

    struct STUN_LOOKUP_PDU *pdu = malloc_error(sizeof(struct STUN_LOOKUP_PDU));

    uint8_t type;

    memcpy(&type, &buffer[0], 1);

    pdu->type = type;

    return pdu;
}
/* Function - seralize_STUN_RESPONSE_PDU
 * 
 * Description - Serializses the given struct and 
 *               returns a buffer containing the data
 * 
 * Input:
 *          pdu - the STUN_RESPONSE_PDU struct
 * Ouput:
 *          a buffer containing the serialized data
 **/
char *seralize_STUN_RESPONSE_PDU(struct STUN_RESPONSE_PDU *pdu)
{

    char *buffer = malloc_error(sizeof(struct STUN_RESPONSE_PDU));
    uint32_t address = htonl(pdu->address);

    memcpy(&buffer[0], &pdu->type, 1);
    memcpy(&buffer[1], &address, 4);

    return buffer;
}

/* Function - deserialize_STUN_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a STUN_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 
 * Output:
 *          returns a pointer to the STUN_RESPONSE_PDU
 **/
struct STUN_RESPONSE_PDU *deserialize_STUN_RESPONSE_PDU(char *buffer)
{

    struct STUN_RESPONSE_PDU *pdu = malloc_error(sizeof(struct STUN_RESPONSE_PDU));

    uint8_t type;
    uint32_t address;
    memcpy(&type, &buffer[0], 1);
    pdu->type = type;

    memcpy(&address, &buffer[1], 4);
    pdu->address = address;
    return pdu;
}
/* Function - malloc_error
 * 
 * Description - a simple function for error detection during malloc
 * 
 * Input:
 *          the size we want to allocate
 * Output:
 *          a pointer to the allocated memory
 **/
void *malloc_error(ssize_t size)
{
    void *mem;
    if ((mem = malloc(size)) == NULL)
    {
        perror("malloc_error error");
        exit(EXIT_FAILURE);
    }
    return mem;
}
