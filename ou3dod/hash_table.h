/*
* Computer Networks.
* Autumn 2021
* Assignment 3
* File:         hash_table.h
* Description:  A hash table list data structure 
*
* Author:       Rasmus Welander
* CS username:  c19rwr@cs.umu.se
* Author:       Marcus Rost
*
* CS username:  bio16mrt@cs.umu.se
* Date:         2021-10-28
*/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "d_list.h"
#include "hash.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HASH_RANGE 256

typedef struct h_table
{

    uint8_t lower_hash_range;
    uint8_t upper_hash_range;
    dlist *array[MAX_HASH_RANGE];

} h_table;

typedef struct element
{

    void *value;
    char *key;
    hash_t hash;

} element;

/**
 * Function - h_table_key_in_range
 * 
 * Description - Function to decide if a given key in in the tables hash-range
 * 
 * Input:
 *          t - the table
 *          key - the key
 * Ouput:
 *          boolean - true or false
 **/
bool h_table_key_in_range(h_table *t, char *key);
/**
 * Function - create_element
 * 
 * Description - creates an entry for the hash-table, contains value, key and hash value.
 * 
 * Input:
 *          hash value - the hashed value of the key
 *          key - the key 
 *          value - the entry
 * Ouput:
 *          return an element
 **/
element *create_element(hash_t hash_value, char *key, void *value);
/**
 * Function - h_table_empty
 * 
 * Description - creates an empty table
 * 
 * Input:
 *          lower_hash_range - the lower range of the hash table
 *          upper_hash_range - the upper hash range of the table
 * Ouput:
 *          a pointer to the table
 **/
h_table *h_table_empty(uint8_t lower_hash_range, uint8_t upper_hash_range);
/**
 * Function - h_table_insert
 * 
 * Description - A function for inserting entries in the table
 *               inserting only if the hashed value of the key
 *               is within the tables hash-range.
 * 
 * Input:
 *          t - the table to insert the values in
 *          key - the key 
 *          value - the entry to be inserted
 * Ouput:
 *          
 **/
bool h_table_insert(h_table *t, char *key, void *value);
/**
 * Function - h_table_remove
 * 
 * Description - removes an entry from the table
 *               if it's within the tables hash range
 * 
 * Input:
 *          t - the table
 *          key - the key of the value to be removed
 *          
 * Ouput:
 *          a void pointer to value that was removed 
 **/
void *h_table_remove(h_table *t, char *key);
/**
 * Function - h_table_is_empty
 * 
 * Description - returns true if the table is empty, otherwise false
 * 
 * Input:
 *          t - the table
 * Ouput:
 *          true if the table is empty, otherwise false
 **/
bool h_table_is_empty(h_table *t);
/**
 * Function - h_table_kill
 * 
 * Description - a function to kill the table and return a list of all the values in the table
 * 
 * Input:
 *          t - the table to kill
 * Ouput:
 *          returns a list of all the values in the table.
 **/
void *h_table_kill(h_table *h);
/**
 * Function - h_table_lookup
 * 
 * Description - A function to lookup an entry in the table
 * 
 * Input:
 *          t - the table 
 *          key - the key of the entry we want to lookup
 * Ouput:
 *          a void pointer to the value that was looked up 
 **/
void *h_table_lookup(h_table *t, char *key);
/**
 * Function - h_table_size
 * 
 * Description - returns the size 
 * 
 * Input:
 *          t - the table 
 * Ouput:
 *          the size of the table
 **/
int h_table_size(h_table *t);
/**
 * Function - h_table_get_values
 * 
 * Description - Returns the values in the hash table from a in a given hash range
 * 
 * Input:
 *          low - the lower bound of the hash-range
 *          upper - the upper bound of the hash range
 *         
 * Ouput:
 *          returns a list of the values in the given hash range
 **/
dlist *h_table_get_values(uint8_t low, uint8_t upper, h_table *h);

#endif