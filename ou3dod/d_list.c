/*
* Computer Networks.
* Autumn 2021
* Assignment 3
* File:         d_list.c
* Description:  A linked list data structure 
*
* Author:       Rasmus Welander
* CS username:  c19rwr@cs.umu.se
* Date:         2021-10-29
*/
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "d_list.h"
/**
 * Function - dlist_empty
 * 
 * Description - creates an empty list
 * 
 * Input:
 * Ouput:
 *          a pointer to the list
 **/
dlist* dlist_empty(){

    dlist *list = malloc(sizeof(dlist));
    list->head = malloc(sizeof(struct cell));
    list->head->next= NULL;

    return list;
}
/**
 * Function - dlist_insert
 * 
 * Description - 
 * 
 * Input:
 *          value - a void pointer to the value to be inserted
 *          list - the list to insert the value on
 *          c - the position where the entry should be inserted 
 * Ouput:
 *          a pointer to the cell where the list was inserted
 **/
cell* dlist_insert(void* value,  dlist* list, cell* c){


    cell* new_entry = malloc(sizeof(cell));
    void* temp;

    new_entry->value = value;
    temp = c->next;
    new_entry->next = temp;
    c->next = new_entry;

    return c->next;

}
/**
 * Function - dlist_is_empty
 * 
 * Description - checks if the list is empty
 * 
 * Input:
 *          list - the list to check 
 * Ouput:
 *          returns true if the list is empty,
 *          false if the list is not empty.
 **/
bool dlist_is_empty(dlist* list){
    
    return list->head->next == NULL;

}
/**
 * Function - dlist_inspect
 * 
 * Description - returns the value at the cell c
 * 
 * Input: 
 *          c - the cell
 *          list - the list that contains the cell
 * Ouput:
 *          - returns a pointer to the value
 **/
void* dlist_inspect(cell* c, dlist * list){

    if(dlist_is_last(c,list)){
        return NULL;
    }else{
        return c->next->value;
    }
}
/**
 * Function - dlist_head
 * 
 * Description - returns the head of the list
 * 
 * Input:
 *          list - the list
 * Ouput:
 *          returns a pointer to the head cell
 **/
cell* dlist_head(dlist* list){
    return list->head;
}
/**
 * Function - dlist_is_last
 * 
 * Description - a function to check if the cell is at the end
 *               of the list.
 * 
 * Input:
 *          c - cell to check
 *          list - the list where the cell exists
 *                    
 * Ouput:
 *          true if cell is last, otherwise false 
 **/
bool dlist_is_last(cell* c, dlist * list){
    return c->next == NULL;
}
/**
 * Function - dlist_next
 * 
 * Description - returns the next cell in the list
 * 
 * Input:
 *          c - the cell previous the one we want
 *          list - the list the cell exists in
 * Ouput:
 *          a pointer to the next cell.
 **/
cell* dlist_next(cell* c, dlist * list){ 

    return c->next;

}
/**
 * Function - dlist_remove
 * 
 * Description - removes a value from the list
 * 
 * Input:
 *          c - the cell where the value we want to remove exists
 *          list - the list where the cell exists
 * Ouput:
 *          returns a void pointer to the value that was removed
 **/
void dlist_remove(cell* c, dlist * list){
    cell *temp;
    printf("7\n");
    temp = c->next;
    printf("8\n");
    c->next = temp->next;
    printf("9\n");
    free(temp);
}
/**
 * Function - dlist_kill
 * 
 * Description - kills the list, freeing all the memory that was used by it,
 *               not including the values that it contained. 
 * 
 * Input:
 *          the list to kill 
 * Ouput:
 *     
 **/
void dlist_kill(dlist * list){
    cell *c;
    printf("4\n");
    while(!dlist_is_empty(list)){
        printf("5\n");
        c = dlist_next(dlist_head(list), list);
        printf("6\n");
        if (c != NULL){
            dlist_remove(c, list);
        }
    }
    free(list->head);
    free(list);
}