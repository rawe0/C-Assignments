/*
* Computer Networks.
* Autumn 2021
* Assignment 3
* File:         serialize.h
* Description:  Functions to serialize structs defined in pdu.h
*
* Author:       Rasmus Welander
* CS username:  c19rwr@cs.umu.se
* Date:         2021-10-29
* CS username:  bio16mrt@cs.umu.se
* Date:         2021-10-28
*/

#ifndef __SERIALIZE_H
#define __SERIALIZE_H
#include "pdu.h"
#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/* Function - free_VAL_LOOKUP_RESPONSE_PDU
 * 
 * Description  - frees the memeory used by the VAL_LOOKUP_RESPONSE_PDU struct
 * 
 * Input:
 *          pdu - a pointer to the struct that is to be freed
 * Output:
 **/
void free_VAL_LOOKUP_RESPONSE_PDU(struct VAL_LOOKUP_RESPONSE_PDU *pdu);
/* Function - free_VAL_INSERT_PDU
 * 
 * Description  - frees the memeory used by the VAL_INSERT_PDU struct
 * 
 * Input:
 *          pdu - a pointer to the struct that is to be freed
 * Output:
 **/
void free_VAL_INSERT_PDU(struct VAL_INSERT_PDU *pdu);
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
struct VAL_LOOKUP_RESPONSE_PDU *create_lookup_response(char *name, int name_length, char *email, int email_length, char ssn[SSN_LENGTH]);
/* Function - malloc_error
 * 
 * Description - a simple function for error detection during malloc
 * 
 * Input:
 *          the size we want to allocate
 * Output:
 *          a pointer to the allocated memory
 **/
void *malloc_error(ssize_t size);
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
char *seralize_NET_ALIVE_PDU(struct NET_ALIVE_PDU *pdu);
/* Function - deserialize_NET_ALIVE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_ALIVE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_ALIVE_PDU
 **/
struct NET_ALIVE_PDU *deserialize_NET_ALIVE_PDU(char *buffer);
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
char *seralize_NET_GET_NODE_PDU(struct NET_GET_NODE_PDU *pdu);
/* Function - deserialize_NET_GET_NODE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_GET_NODE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_GET_NODE_PDU
 **/
struct NET_GET_NODE_PDU *deserialize_NET_GET_NODE_PDU(char *buffer);
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
char *seralize_NET_GET_NODE_RESPONSE_PDU(struct NET_GET_NODE_RESPONSE_PDU *pdu);
/* Function - deserialize_NET_GET_NODE_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_GET_NODE_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_GET_NODE_RESPONSE_PDU
 **/
struct NET_GET_NODE_RESPONSE_PDU *deserialize_NET_GET_NODE_RESPONSE_PDU(char *buffer);
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
char *seralize_NET_JOIN_PDU(struct NET_JOIN_PDU *pdu);
/* Function - deserialize_NET_JOIN_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_JOIN_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_JOIN_PDU
 **/
struct NET_JOIN_PDU *deserialize_NET_JOIN_PDU(char *buffer);
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
char *seralize_NET_JOIN_RESPONSE_PDU(struct NET_JOIN_RESPONSE_PDU *pdu);
/* Function - deserialize_NET_JOIN_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_JOIN_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_JOIN_RESPONSE_PDU
 **/
struct NET_JOIN_RESPONSE_PDU *deserialize_NET_JOIN_RESPONSE_PDU(char *buffer);
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
char *seralize_NET_CLOSE_CONNECTION_PDU(struct NET_CLOSE_CONNECTION_PDU *pdu);
/* Function - deserialize_NET_CLOSE_CONNECTION_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_CLOSE_CONNECTION_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_CLOSE_CONNECTION_PDU
 **/
struct NET_CLOSE_CONNECTION_PDU *deserialize_NET_CLOSE_CONNECTION_PDU(char *buffer);
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
char *seralize_NET_NEW_RANGE_PDU(struct NET_NEW_RANGE_PDU *pdu);
/* Function - deserialize_NET_NEW_RANGE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_NEW_RANGE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_NEW_RANGE_PDU
 **/
struct NET_NEW_RANGE_PDU *deserialize_NET_NEW_RANGE_PDU(char *buffer);
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
char *seralize_NET_NEW_RANGE_RESPONSE_PDU(struct NET_NEW_RANGE_RESPONSE_PDU *pdu);
/* Function - deserialize_NET_NEW_RANGE_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_NEW_RANGE_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_NEW_RANGE_RESPONSE_PDU
 **/
struct NET_NEW_RANGE_RESPONSE_PDU *deserialize_NET_NEW_RANGE_RESPONSE_PDU(char *buffer);
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
char *seralize_NET_LEAVING_PDU(struct NET_LEAVING_PDU *pdu);
/* Function - deserialize_NET_LEAVING_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a NET_LEAVING_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the NET_LEAVING_PDU
 **/
struct NET_LEAVING_PDU *deserialize_NET_LEAVING_PDU(char *buffer);
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
char *seralize_VAL_INSERT_PDU(struct VAL_INSERT_PDU *pdu);
/* Function - deserialize_VAL_INSERT_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_INSERT_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_INSERT_PDU
 **/
struct VAL_INSERT_PDU *deserialize_VAL_INSERT_PDU(char *buffer);
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
char *seralize_VAL_REMOVE_PDU(struct VAL_REMOVE_PDU *pdu);
/* Function - deserialize_VAL_REMOVE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_REMOVE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_REMOVE_PDU
 **/
struct VAL_REMOVE_PDU *deserialize_VAL_REMOVE_PDU(char *buffer);
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
char *seralize_VAL_LOOKUP_PDU(struct VAL_LOOKUP_PDU *pdu);
/* Function - deserialize_VAL_LOOKUP_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_LOOKUP_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_LOOKUP_PDU
 **/
struct VAL_LOOKUP_PDU *deserialize_VAL_LOOKUP_PDU(char *buffer);
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
char *seralize_VAL_LOOKUP_RESPONSE_PDU(struct VAL_LOOKUP_RESPONSE_PDU *pdu);
/* Function - deserialize_VAL_LOOKUP_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a VAL_LOOKUP_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the VAL_LOOKUP_RESPONSE_PDU
 **/
struct VAL_LOOKUP_RESPONSE_PDU *deserialize_VAL_LOOKUP_RESPONSE_PDU(char *buffer);
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
char *seralize_STUN_LOOKUP_PDU(struct STUN_LOOKUP_PDU *pdu);
/* Function - deserialize_STUN_LOOKUP_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a STUN_LOOKUP_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 

 * Output:
 *          returns a pointer to the STUN_LOOKUP_PDU
 **/
struct STUN_LOOKUP_PDU *deserialize_STUN_LOOKUP_PDU(char *buffer);
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
char *seralize_STUN_RESPONSE_PDU(struct STUN_RESPONSE_PDU *pdu);
/* Function - deserialize_STUN_RESPONSE_PDU
 * 
 * Description - deserializes the incoming data in the buffer to a STUN_RESPONSE_PDU
 * 
 * Input:
 *          buffer - the buffer containing the data 
 * Output:
 *          returns a pointer to the STUN_RESPONSE_PDU
 **/
struct STUN_RESPONSE_PDU *deserialize_STUN_RESPONSE_PDU(char *buffer);

#endif