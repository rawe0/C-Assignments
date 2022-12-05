/*
* Systemnära programmering
* Autumn 2020
* Assignment 3
* File:         mdu.c
* Description:  An implementation of the unix tool "du" that uses threads.
*
* Author:       Rasmus welander
* CS username:  c19rwr@cs.umu.se
* Date:         2020-10-09
* Input:        files/directorys to calculate size of
* Output:       the size of given directorys/files
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include "stack.h"

typedef struct target_info {
        char *origin_path;
        int *total_size;
        char *file_path;
} target_info ;

typedef struct master_struct {
        stack *stack;
        int threads;
        int waiting;
        int return_val;
} master_struct;

void create_work(target_info**, stack*, int, char**, int);
void cleanup (int, target_info**, master_struct*, int*);
target_info *create_target_info(char*, char*, int*);
int process_directory(target_info*, master_struct*);
void error_message(target_info*, master_struct*);
master_struct *create_master_struct(int);
void start_working(int*, master_struct*);
void calculate_size(target_info*, int);
void print_result(target_info**, int);
void wait_for_work(master_struct*);
char* create_path(char*, char*);
struct stat lstat_error(char*);
void* malloc_error(int);
void* work(void*);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]){

        int* n_threads;
        bool no_threads = true;
        master_struct *master;
        target_info **files;
        int nr_args;
        int status;
        int opt;

        n_threads = malloc_error(sizeof(int));


        while ((opt = getopt(argc, argv, "j:")) != -1) {
                switch (opt) {
                        case 'j':
                                no_threads = false;
                                *n_threads = strtol(optarg, NULL, 10);
                                if (errno == ERANGE){
                                        perror("strtol");
                                        exit(EXIT_FAILURE);
                                }
                        break;
                }
        }
        nr_args = argc - optind;

        if (no_threads){
                *n_threads = 1;
        }

        master = create_master_struct(*n_threads);

        files = malloc_error((nr_args + 1)*sizeof(target_info*));

        create_work(files, master->stack, nr_args, argv, optind);
        start_working(n_threads, master);
        print_result(files, nr_args);
        status = master->return_val;
        cleanup (nr_args,files,master,n_threads);

        return status;
}

/*
* work - a thread function, that recursively goes through a catalogue and it's sub- files/catalogues
*        and calculate the total size.
*
*       @void *arg - a pointer to a master_struct structure, which should contain the following:
*
*              @ master->stack - a stack filled with target_info strucutres.
*              @ master->waiting - an integer which represents the amount of threads currently sleeping.
*              @ master->threads - the total number of threads runing.
*              @ master->return_value - an integer where we can put the return value of the threads.
*
* returns - NULL (return value is set through master struct)
*/
void* work(void *arg){

        master_struct *master = (master_struct*)arg;
        struct stat file_info;
        target_info *target;
        bool finished = false;
        int size = 0;

        //Work until we break the loop
        pthread_mutex_lock(&lock);
        while(!stack_is_empty(master->stack) && !finished){

                if(!stack_is_empty(master->stack)){
                        size = 0;
                        //Get target from stack
                        target = stack_top(master->stack);
                        master->stack = stack_pop(master->stack);
                        pthread_mutex_unlock(&lock);
                        //Get file information
                        file_info = lstat_error(target->file_path);
                        //If target is a directory, add new targets to stack
                        if (S_ISDIR(file_info.st_mode)){
                                size = process_directory(target, master);
                        }

                        size += (int)file_info.st_blocks;
                        calculate_size(target, size);

                        //Lock mutex before we check
                        pthread_mutex_lock(&lock);
                }
                //Wait if there is no work on stack
                else {
                        master->waiting++;
                        while(stack_is_empty(master->stack) || !finished){
                                //If all threads are waiting for work we must be finished
                                if(master->waiting == master->threads){
                                        finished = true;
                                }
                                pthread_cond_wait(&cond, &lock);
                        }
                        master->waiting--;
                }
        }
        pthread_mutex_unlock(&lock);
        return NULL;
}
/*
* error_message - a function for printing error message incase we run into problems.
*
*       @target - a pointer to the target we were working when the error occured.
*       @master - a pointer to the master structure.
*
*/
void error_message(target_info *target, master_struct *master){

        char error_msg[1024];

        pthread_mutex_lock(&lock);
        master->return_val = 1;
        pthread_mutex_unlock(&lock);

        switch (errno){
                case EACCES:
                        snprintf(error_msg, sizeof(error_msg), "mdu: cannot read directory %s", target->file_path);
                        perror(error_msg);
                break;
                case ENOTDIR:
                        snprintf(error_msg, sizeof(error_msg), "mdu: %s is not a directory", target->file_path);
                        perror(error_msg);
                break;
                default:
                        perror("opendir");
                        exit(EXIT_FAILURE);
        }
}
/*
* process_directory - a function for process a target that is a direcotry, where we add all the files in
*                     the directory to the work stack.
*
*       @target - a pointer to the targets.
*       @ master - a pointer to the master structure the thread is using.
*
*/
int process_directory(target_info *target, master_struct *master){

        struct dirent *dir_pointer;
        struct stat file_info;
        target_info *new_target;
        DIR *curr_dir;
        int size = 0;
        char *path;

        //Open directory
        if ((curr_dir = opendir(target->file_path)) == NULL){
                error_message(target, master);
        }
        else {
                //While there still are files in the directory, put them in stack as new targets.
                while((dir_pointer = readdir(curr_dir)) != NULL){
                        //create new struct for the target
                        if (errno == EBADF){
                                perror("readdir");
                                exit(EXIT_FAILURE);
                        }
                        if (strcmp(dir_pointer->d_name, ".") == 0 || strcmp(dir_pointer->d_name, "..") == 0){
                                continue;
                        }
                        //create path to new file
                        path = create_path(dir_pointer->d_name, target->file_path);

                        file_info = lstat_error(path);
                        if (S_ISDIR(file_info.st_mode)){

                                new_target = create_target_info(target->origin_path, path, target->total_size);
                                pthread_mutex_lock(&lock);
                                stack_push(master->stack, new_target);
                                
                                pthread_mutex_unlock(&lock);
                        }
                        else {
                                size += (int)file_info.st_blocks;
                                free(path);
                        }

                }

                if (closedir(curr_dir) == -1){
                        perror("closedir");
                        exit(EXIT_FAILURE);
                }
        }
        return size;
}


/*
* create_path - creates a path to a file
*
*       @file_name - the name of file we want to create a path to
*       @old_path - the path to the directory the file is in.
*
*       returns - A full path to the file
*
*/
char* create_path(char* file_name, char* old_path){

        int str_size = 0;
        char *path;

        str_size = strlen(file_name) + strlen(old_path) + 2;

        path = malloc_error(sizeof(char)*str_size);

        snprintf(path, str_size, "%s/%s", old_path, file_name);

        return path;

}
/*
* calculate_size - a function for caluculating the file size of a file in a target_info structure.
*
*       @target - a pointer to the target_info structure in question.
*       @file_info - a stat structure to the file in the target_info structure.
*
*/
void calculate_size(target_info *target, int size){

        //Add filesize to global
        pthread_mutex_lock(&lock);
        *target->total_size += size;
        pthread_mutex_unlock(&lock);

        //Free target if it is not original file
        if (strcmp(target->file_path, target->origin_path)){
                free(target->file_path);
                free(target);
        }
}

/*
* create_target_info - A function for creating a target_info structure.
*
*       @origin_path - a pointer to the path the file we're created a structure for originated from.
*       @total_size - a pointer to the total_size of the file tree we've calculated so far.
*       @path - the path to the file we're creating the structure for.
*
*
*       Returns - A pointer to the structure.
*/
target_info *create_target_info(char *origin_path, char *path, int *total_size){

        target_info *info;

        info = malloc_error(sizeof(struct target_info));
        info->origin_path = origin_path;
        info->file_path = path;
        //If total_size is NULL, it's a starting target
        if (total_size == NULL){
                info->total_size = malloc_error(sizeof(int));
                *info->total_size = 0;
        }
        else {
                info->total_size = total_size;
        }
        return info;
}


/*
* create_master_struct - A function for creating a master_struct structure.
*
*       @n_threads - the amount of threads the program will be runing
*
*       Returns - a pointer to the structure.
*/
master_struct *create_master_struct(int n_threads){

        master_struct *master;

        master = malloc_error(sizeof(struct master_struct));
        master->stack = stack_empty();
        master->threads = n_threads;
        master->waiting = 0;
        master->return_val = 0;
        return master;
}


/*
* cleanup - A function that returns memory after the program has been executed.
*
*       @nr_args - the amount of arguments used by the program.
*       @files - a pointer to the arguments used by the program.
*       @master - a pointer to the master_struct used by the program.
*       @n_threads - a pointer the to amount of threads used by the program.
*
*/
void cleanup(int nr_args, target_info** files, master_struct *master, int* n_threads){

        for(int i = 0; i < nr_args; i++){
                free(files[i]->total_size);
                free(files[i]);
        }
        stack_kill(master->stack);
        free(master);
        free(files);
        free(n_threads);
}

/*
* print_result - prints the result after calculations are done.
*
*       @nr_args - the amount of arguments used by the program.
*       @files - a pointer to the arguments used by the program.
*/
void print_result(target_info** files, int nr_args){

        for(int i = 0; i < nr_args; i++){
                printf("%d\t%s\n", *(files[i]->total_size)/2, files[i]->origin_path);
        }
}

/*
* create_work - creates the intial target_info strucutres that the user specifices and pushes them to the stack.
*
*       @nr_args - the amount of arguments used by the program.
*       @stack - a pointer to the stack we're pushing the work to.
*       @argv[] - a pointer to the arguments used by the program.
*       @files - a pointer to the array of target_info structures we will be creating.
*       @optind - a value where the arguments to the program starts.
*
*/
void create_work(target_info **files, stack *stack, int nr_args, char* argv[], int optind){

        for (int i = 0; i < nr_args; ++i) {
                //Create target_info struct
                files[i] = create_target_info(argv[optind + i], argv[optind + i], NULL);
                //Push the structure to the work stack
                stack = stack_push(stack, files[i]);
        }
}
/*
* start_working - A function which creates threads, puts them to work and waits for them to finish start_working.
*
*       @n_threads - a pointer to the number of threads to create.
*       @master - a pointer to a master_struct structure, which should contain the following:
*
*              @ master->stack - a stack filled with target_info strucutres.
*              @ master->waiting - an integer which represents the amount of threads currently sleeping.
*              @ master->threads - the total number of threads runing.
*              @ master->return_value - an integer where we can put the return value of the threads.
*
*/
void start_working(int* n_threads, master_struct *master){

        pthread_t thread[*n_threads];
        //Create threads
        for(int i = 0; i < *n_threads; i++){
                if (pthread_create(&thread[i], NULL, work, master) != 0){
                        perror("pthread_create");
                        exit(EXIT_FAILURE);
                }
        }
        //Wait for threads to finish working
        for(int i = 0; i < *n_threads; i++){
                if (pthread_join(thread[i], NULL) != 0){
                        perror("pthread_join");
                        exit(EXIT_FAILURE);
                }
        }
}

/* malloc_error - simple function exits if there are any errors when calling malloc
*
*       @size - the space that will be allocated.
*
*       Returns - a pointer to the allocated space
*
*/
void* malloc_error(int size){
    void* p;
    if ((p = malloc(size)) == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
    }
    return p;
}
/* lstat_error - Simple function that exits if there are any errors when calling lstat
*
*       @path - Path to the file we will call lstat with.
*
*       Returns - A structure with the file information.
*
*/
struct stat lstat_error(char* path){
    struct stat file_info;
    if (lstat(path, &file_info) == -1){
            perror("lstat");
            exit(EXIT_FAILURE);
    }
    return file_info;
}
