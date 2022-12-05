/*
* Systemnära programmering
* Autumn 2020
* Assignment 1
* File:         mexec.c
* Description: Executes a pipeline of commands given correct input, the first
*              commands output will be the next commands input, and the last
*              command will write to stdout.
*
* Author:       Rasmus welander
* CS username:  c19rwr@cs.umu.se
* Date:         2020-09-20
* Input:        File or stdin
* Output:       no output, except whatever the pipeline outputs.
*/



#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFSIZE	1024
#define READ_END 0
#define WRITE_END  1


char** readInput(FILE*, int*);
char** stringToWords(char*);
int** createPipes(int);
void executePipeline(char***, int , int**);
void cleanup(char**, char*** , int** , int*);
void dup2Pipe(int, int);
void closePipes(int**, int);
void executeCommand(char**);
void waitForChildren(int);

int main(int argc, char *argv[]){

	FILE* inputfile;
	char** commands;
	char*** arguments;
	int** pipes;
	int* nCommands;

	if (argc == 1) {
		inputfile = stdin;
	}
	else if (argc == 2) {
		if ((inputfile = fopen(argv[1], "r")) == NULL) {
			perror(argv[1]);
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "Incorrect amount of arguements\n");
		fprintf(stderr, "correct usage:\n%s <file>\nor \n%s", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	if((nCommands = malloc(sizeof(int))) == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	//read file and create array of command lines
	commands = readInput(inputfile, nCommands);
	//if we opened a file close it.


	if ((arguments = malloc(sizeof(char*)*(*nCommands))) == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	//create an array of commands, subdivided into arguments
	for(int i = 0; i < *nCommands; i++){
		arguments[i] = stringToWords(commands[i]);
	}
	//Create pipes for executing the pipeline
	pipes = createPipes(*nCommands-1);
	//close file before we fork so we don't duplicate it
	fclose(inputfile);
	//Execute the pipeline
	executePipeline(arguments, *nCommands, pipes);
	//Free memory after executing pipeline
	cleanup(commands, arguments, pipes, nCommands);
	return 0;
}



/*
* readinput - Reads the user input and arranges the commands in an array
*
* IMPORTANT: UP TO CALLER TO FREE AFTER USE
* NOTE: the value that the pointer "n" is pointing to is changed.
*
* @in - the input file
* @n - the pointer to the number of command lines.
*
* Returns: An array of the input commands, and the number of commands in the
*	   form of a pointer to a value "n" that is changed.
*/
char** readInput(FILE* in, int* n){

	char* buf;
	if((buf = malloc(sizeof(char*)*BUFSIZE)) == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	char** cmd = NULL;
	int counter = 0;
	//read file line by line and insert into an array of commands
	while (fgets(buf, BUFSIZE, in) != NULL){
		counter++;
		if((cmd = realloc(cmd,sizeof(char*)*(counter))) == NULL){
			perror("realloc");
			exit(EXIT_FAILURE);
		}
		if((cmd[counter - 1] = malloc(sizeof(char)*(strlen(buf) + 1))) == NULL){
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		strcpy(cmd[counter - 1], buf);
		//Every line is ended by '\n' when reading,
		//and this complicates using "execvp()"
		//so replace the '\n' with \0 to end the string.
		cmd[counter-1][strlen(cmd[counter-1])-1] = '\0';
	}
	*n = (counter);
	free(buf);
	return cmd;
}

/*
* stringToWords - Takes a string and seperates every word in the string
*                 words are defined as characters with space or tabs inbetween.
*
*
* IMPORTANT: UP TO CALLER TO FREE AFTER USE
*
* @string - the string we want to seperate
*
* Returns: An array words in the string and NULL in the position after the
*          last word.
*/
char** stringToWords(char* string){

	char* word;
	char** args = NULL;
	int n = 1;
	//Seperate the "words" in the command line (string),
	//and put them in an array of strings
	while((word = strsep(&string," ""	")) != NULL){
		if((args = realloc(args,sizeof(char*)*(n+1))) == NULL){
			perror("realloc");
			exit(EXIT_FAILURE);
		}
		if((args[n-1] = malloc(sizeof(char)*strlen(word)+1)) == NULL){
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		strcpy(args[n-1], word);
		n++;
	}
	//terminate array of arguments with NULL
	args[n-1] = NULL;
	return args;
}

/*
* allocatePipes - allocates space for n pipes.
*
* IMPORTANT: UP TO CALLER TO FREE AFTER USE
*
* @n - the number of pipes
*
* Returns: An array of the pipes
*/
int** createPipes(int n){

	int** p;
	if((p = malloc(sizeof(int*)*(n))) == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n; i++){
		p[i] = malloc(sizeof(int)*2);
		if (pipe(p[i]) != 0 ) {
			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}
	return p;
}

/*
* executePipeline - Executes the pipeline of commands given by
*
* @commands - the array of commands
* @n - the number of commands in the pipeline
* @pipes - the array of pipes that will be used to execute the pipeline
*
* returns: n/a
*/
void executePipeline(char*** commands, int n, int **pipes){

	pid_t pid;
	for (int i = 0; i < n; i++){
		pid = fork();
		if (pid < 0){
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0){
			//Special case when we only have one command.
			if (n == 1){
				executeCommand(commands[i]);
			}
			//Duping pipe for the first child.
			if (i == 0){
				dup2Pipe(pipes[i][WRITE_END], STDOUT_FILENO);
			}
			//Duping pipe for the last child.
			else if (i == (n-1)){
				dup2Pipe(pipes[i-1][READ_END],STDIN_FILENO);
			}
			//Duping pipes for the middle children.
			else {
				dup2Pipe(pipes[i][WRITE_END], STDOUT_FILENO);
				dup2Pipe(pipes[i-1][READ_END],STDIN_FILENO);
			}
			//Closing the remaining pipes.
			closePipes(pipes, n-1);
			//Execute command number i.
			executeCommand(commands[i]);
		}
	}
	//Close parent pipes
	closePipes(pipes, n-1);
	//Parent waits for children
	waitForChildren(n);
}


/**
* dup2Pipe - a function for duping pipes
*
* @oldfd - old file descriptor
* @newfd - new file descriptor
*
* returns: n/a
*/
void dup2Pipe(int oldfd, int newfd){

	if(dup2(oldfd,newfd)<0){
		perror("dup2");
		exit(EXIT_FAILURE);
	}
}
/**
* closePipes - a function for closing pipes
*
* @pipes - the array of pipes that need to be closed
* @nrOfPipes - the number of pipes in the array
*
* returns: n/a
*/
void closePipes(int** pipes, int n){

	for(int i = 0; i < n; i++){
		close(pipes[i][READ_END]);
		close(pipes[i][WRITE_END]);
	}
}

/**
* executeCommand - A function for executing a command with execvp
*
* @cmd - An array of strings, which is the command, must be NULL terminated.
*
* returns: n/a
*/
void executeCommand(char** cmd){

	if (execvp(cmd[0], cmd)){
		perror(cmd[0]);
		exit(EXIT_FAILURE);
	}
}

/**
* waitForChildren - a function that waits n times, and checks the return
*                   values from the children, and the return value
*                   of the wait function
*
* @n - the amount of times we want to wait
*
* returns: n/a
*/
void waitForChildren(int n){

	int status = 0;
	for(int i = 0; i < n; i++){
		if(wait(&status) == -1){
			perror("wait");
			exit(EXIT_FAILURE);
		}
		if ((WEXITSTATUS(status)) != 0){
			exit(EXIT_FAILURE);
		}
	}
}




/*
* cleanup - a function for free'ing memeory after the program has been executed
*
*
* @commands - a pointer to the commands
* @arguments - a pointer to the argument array
* @pipes - a pointer the the pipes
* @n - a pointer the the number of commands
*
* returns: n/a
*/
void cleanup(char** commands, char*** arguments, int** pipes, int *n){

	//Free all arguments
	for(int i = 0; i < *n; i++){
		char* temp;
		int j = 0;
		temp = arguments[i][j];
		//Free every argument in the command
		while(temp != NULL){
			temp = arguments[i][j];
			free(temp);
			j++;
		}
		//Free the terminating "NULL".
		free(temp);
		free(arguments[i]);
	}
	free(arguments);
	//Free commands
	for (int i = 0; i < *n; i++){
		free(commands[i]);
	}
	free(commands);
	//Free all the pipes
	for(int i = 0; i < *n-1; i++){
		free(pipes[i]);
	}
	free(pipes);
	free(n);
}
