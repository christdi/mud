#ifndef MUD_TASK_TASK_H
#define MUD_TASK_TASK_H

#include <time.h>

#include "mud/util/muduuid.h"

/**
 * Typedefs
 **/
typedef struct game game_t;
typedef struct linked_list linked_list_t;

/**
 * Structs
 **/
typedef struct task {
  mud_uuid_t uuid;
  char* name;
  time_t execute_at;
  int ref;
} task_t;

/**
 * Function prototypes
 **/
task_t* task_new_task_t(const char* name, int execute_in, int ref);
void task_free_task_t(task_t* task);
void task_deallocate_task_t(void* value);

int task_schedule_task(linked_list_t* tasks, task_t* task);
int task_cancel_task(linked_list_t* tasks, game_t* game, task_t* task);
int task_execute_tasks(linked_list_t* tasks, game_t* game);

#endif