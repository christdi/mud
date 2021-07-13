#ifndef MUD_TASK_TASK_H
#define MUD_TASK_TASK_H

#include <time.h>

/**
 * Typedefs
**/
typedef struct game game_t;
typedef struct linked_list linked_list_t;
typedef int (*task_func_t)(game_t* game);

/**
 * Structs
**/
typedef struct task {
  time_t execute_at;
  task_func_t function;
} task_t;

/**
 * Function prototypes
**/
task_t* create_task_t();
void free_task_t(task_t* task);
void deallocate_task_t(void* value);

int task_schedule(linked_list_t* tasks, int seconds_in_future, task_func_t function);
int task_execute(linked_list_t* tasks, game_t* game);

#endif