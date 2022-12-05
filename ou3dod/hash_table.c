/*
* Computer Networks.
* Autumn 2021
* Assignment 3
* File:         hash_table.c
* Description:  A hash table data structure 
*
* Author:       Rasmus Welander
* CS username:  c19rwr@cs.umu.se
* Author:       Marcus Rost
*
* CS username:  bio16mrt@cs.umu.se
* Date:         2021-10-28
*/
#include "hash_table.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pdu.h"
#include "serialize.h"
#include <string.h>

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
bool h_table_key_in_range(h_table *t, char *key)
{

    hash_t hashed_key = hash_ssn((char *)key);
    if (hashed_key >= t->lower_hash_range && hashed_key <= t->upper_hash_range)
    {
        return true;
    }
    else
    {
        return false;
    }
}
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
element *create_element(hash_t hash_value, char *key, void *value)
{
    element *new_element = malloc(sizeof(element));
    new_element->hash = hash_value;
    new_element->key = key;
    new_element->value = value;

    return new_element;
}
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
h_table *h_table_empty(uint8_t lower_hash_range, uint8_t upper_hash_range)
{

    h_table *h = malloc(sizeof(h_table));

    h->lower_hash_range = lower_hash_range;
    h->upper_hash_range = upper_hash_range;

    //Init array of lists to null
    for (int i = 0; i < MAX_HASH_RANGE; i++)
    {
        h->array[i] = NULL;
    }

    return h;
}
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
bool h_table_insert(h_table *t, char *key, void *value)
{

    bool return_val;
    //check if key is in range
    if (h_table_key_in_range(t, key) == false)
    {

        return_val = false;
    }
    else
    {
        hash_t hashed_key = hash_ssn(key);
        element *new_entry = create_element(hashed_key, (char *)key, value);
        //Check that we've already created a list, if not create one
        if (t->array[new_entry->hash] == NULL)
        {
            t->array[new_entry->hash] = dlist_empty();
        }
        //Insert element
        dlist_insert(new_entry, t->array[new_entry->hash], dlist_head(t->array[new_entry->hash]));
        return_val = 0;
    }
    return return_val;
}
/**
 * Function - h_table_remove
 * 
 * Description - removes an entry from the table
 *               if it's within the tables hash range,
 *               if no value is found it returns NULL
 * 
 * Input:
 *          t - the table
 *          key - the key of the value to be removed
 *          
 * Ouput:
 *          a void pointer to value that was removed 
 **/
void *h_table_remove(h_table *t, char *key)
{

    void *value = NULL;
    if (h_table_key_in_range(t, key))
    {

        hash_t hashed_key = hash_ssn(key);
        dlist *l = t->array[hashed_key];
        //Check that the list is not empty
        if (!dlist_is_empty(l))
        {
            cell *c = dlist_head(l);
            while (!dlist_is_last(c, l))
            {
                element *e = (element *)dlist_inspect(c, l);
                //we found the correct element in the hash-table
                if (strncmp(e->key, key, 12) == 0)
                {
                    //delete from list
                    dlist_remove(c, l);
                    value = e->value;
                    //free element
                    free(e);
                }
                // move on to next element
                if (!dlist_is_last(c, l))
                {

                    c = dlist_next(c, l);
                }
            }
        }
    }
    return value;
}
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
int h_table_size(h_table *t)
{
    dlist *l;
    cell *c;
    int size = 0;
    for (int i = 0; i < MAX_HASH_RANGE; i++)
    {
        //for each list check how many entries
        if (t->array[i] != NULL && !dlist_is_empty(t->array[i]))
        {
            l = t->array[i];
            c = dlist_head(l);
            while (!dlist_is_last(c, l))
            {
                size++;
                c = dlist_next(c, l);
            }
        }
    }

    return size;
}

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
bool h_table_is_empty(h_table *t)
{
    bool lists_are_empty = true;
    for (int i = 0; i < MAX_HASH_RANGE; i++)
    {
        //Check if the table is empty
        if (t->array[i] != NULL && !dlist_is_empty(t->array[i]))
        {
            lists_are_empty = false;
            break;
        }
    }
    return lists_are_empty;
}

/**
 * Function - h_table_lookup
 * 
 * Description - A function to lookup an entry in the table
 * 
 * Input:
 *          t - the table 
 *          key - the key of the entry we want to lookup
 * Ouput:
 *          a void pointer to the value that was looked up, or NULL if no value was found
 **/
void *h_table_lookup(h_table *t, char *key)
{

    if (h_table_key_in_range(t, key))
    {

        hash_t hashed_key = hash_ssn(key);
        dlist *l = t->array[hashed_key];
        //Check that list isn't null
        if (l != NULL)
        {
            if (!dlist_is_empty(l))
            {
                cell *c = dlist_head(l);
                //Search through the list for the value
                while (!dlist_is_last(c, l))
                {
                    element *e = (element *)dlist_inspect(c, l);
                    //we found the correct element in the hash-table
                    if (strncmp(e->key, key, 12) == 0)
                    {
                        return e->value;
                    }
                    // move on to next element
                    c = dlist_next(c, l);
                }
            }
        }
    }
    //Return NULL if we don't find an element
    return NULL;
}

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
void *h_table_kill(h_table *h)
{
    dlist *val_list = dlist_empty();
    dlist *hash_list;
    cell *val;
    for (int i = 0; i < MAX_HASH_RANGE; i++)
    {
        hash_list = h->array[i];
        //Continue while list is not empty
        if (hash_list != NULL)
        {
            while (!dlist_is_empty(hash_list))
            {
                //Get head of list
                val = dlist_inspect(dlist_head(hash_list), hash_list);
                //insert value in list we're about to send
                dlist_insert(val, val_list, dlist_head(val_list));
                //Remove from list in hash_table
                dlist_remove(dlist_head(hash_list), hash_list);
            }
        }
    }
    for (int i = 0; i < MAX_HASH_RANGE; i++)
    {
        hash_list = h->array[i];

        if (hash_list != NULL)
        {
            dlist_kill(hash_list);
        }
    }
    free(h);
    return val_list;
}
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
dlist *h_table_get_values(uint8_t low, uint8_t upper, h_table *h)
{
    dlist *send_list = dlist_empty();
    dlist *hash_list;
    cell *val;
    //itterate through all the lists
    for (int i = low; i < upper + 1; i++)
    {
        hash_list = h->array[i];
        //check if list exists for the given hash-range
        if (hash_list != NULL)
        {
            //Continue while list is not empty
            while (!dlist_is_empty(hash_list))
            {
                //Get head of list
                val = dlist_inspect(dlist_head(hash_list), hash_list);
                //insert value in list we're about to send
                dlist_insert(val->value, send_list, dlist_head(send_list));
                //Remove from list in hash_table
                dlist_remove(dlist_head(hash_list), hash_list);
            }
        }
    }

    return send_list;
}