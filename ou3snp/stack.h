/*
* Systemnära programmering
* Autumn 2020
* Assignment 3
* File:         stack.h
* Description:  a stack implemented as a directed list.
*
* Author:       Rasmus welander
* CS username:  c19rwr@cs.umu.se
* Date:         2020-10-16
*/
#ifndef __STACK_H
#define __STACK_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/* Defining the stack structure */
typedef struct stack stack;

/*
* stack_empty - creates an empty stack
*
* Returns: A pointer to the stack
*/
stack *stack_empty(void);
/*
* stack_is_empty - returns true or false depending on if the stack is empty or not
*
* Returns: true or false
*/
bool stack_is_empty(const stack*);
/*
* stack_push - pushes a value to the stack
*
* Parameters:
*
* @stack, a pointer to the stack pusing a value to
* @value, a pointer to the value we're pushing
*
* Returns: A pointer to the stack
*/
stack *stack_push(stack*, void*);
/*
* stack_pop - removes the top most cell in the stack
*
* Parameters:
*
* @stack, a pointer to the stack we're popping
*
* Returns: A pointer to the stack
*/
stack * stack_pop(stack*);
/*
* stack_top - returns the value at the top of the stack
*
* Parameters:
*
* @stack, a pointer to the stack
*
* returns: A pointer to the value
*/
void *stack_top(stack*);
/*
* stack_kill - kills the stack and deallocates all memory that was used by the stack_kill
*
* 	NOTE: Does not deallocate the memory used by the elements in the stack
*
* Parameter:
*
* @stack - a pointer to the stack
*
* Returns: nothing
*/
void stack_kill(stack*);
#endif
