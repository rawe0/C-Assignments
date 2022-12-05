/*
* Systemnära programmering
* Autumn 2020
* Assignment 3
* File:         stack.c
* Description:  a stack implemented as a directed list.
*
* Author:       Rasmus welander
* CS username:  c19rwr@cs.umu.se
* Date:         2020-10-16
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"



typedef struct cell {
	void* value;
	struct cell* next;
} cell;
struct stack {
	void* value;
	struct cell* top;
};

/*
* stack_empty - creates an empty stack
*
* Returns: A pointer to the stack
*/
stack *stack_empty(void){

	stack *stack;
	cell *cell;

        if ((stack = calloc(1, sizeof(struct stack))) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

        if ((cell = calloc(1, sizeof(struct cell))) == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

        cell->value = NULL;
        cell->next = NULL;
        stack->top = cell;
        return stack;
}

/*
* stack_is_empty - returns true or false depending on if the stack is empty or not
*
* Returns: true or false
*/
bool stack_is_empty(const stack *stack){

        return stack->top->value == NULL;
}

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
stack *stack_push(stack *stack, void *value){

        //Don't need to allocate first element in stack
        if (stack->top->value == NULL){
                stack->top->value = value;
        }
        //if it is not the first element we have to allocate
        else {
                cell *cell;

		if ((cell = calloc(1, sizeof(struct cell))) == NULL) {
			perror("calloc");
			exit(EXIT_FAILURE);
		}

                cell->next = stack->top;
                stack->top = cell;
                cell->value = value;
        }

        return stack;
}

/*
* stack_pop - removes the top most cell in the stack
*
* Parameters:
*
* @stack, a pointer to the stack we're popping
*
* Returns: A pointer to the stack
*/
stack *stack_pop(struct stack *stack){

        cell *cell = stack->top;
        //set next cell to top
	if(stack->top->next != NULL){
        	stack->top = stack->top->next;
		free(cell);
	}
	else {
		stack->top->value = NULL;
	}
        //free the cell
        return stack;

}

/*
* stack_top - returns the value at the top of the stack
*
* Parameters:
*
* @stack, a pointer to the stack
*
* returns: A pointer to the value
*/
void *stack_top(struct stack *stack){
        //check so we have a value to return
        if(stack->top->value != NULL){
                return stack->top->value;
        }
        return NULL;
}


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
void stack_kill(struct stack *stack){

        while (!stack_is_empty(stack)){
                stack_pop(stack);
        }
	free(stack->top);
	free(stack);
}
