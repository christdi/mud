#ifndef MUD_TASK_TASK_H
#define MUD_TASK_TASK_H

#include <sys/time.h>

#include "mud/util/muduuid.h"

/**
 * Typedefs
 **/
typedef struct game game_t;
typedef struct linked_list linked_list_t;
typedef struct lua_ref lua_ref_t;

/**
 * Structs
 **/
typedef struct task {
  mud_uuid_t uuid;
  char* name;
  struct timeval execute_at;
  long interval_ms;
  lua_ref_t* ref;
  int (*callback)(game_t*);
} task_t;

/**
 * Function prototypes
 **/
task_t* task_new_task_t(const char* name, long execute_in_ms, lua_ref_t* ref);
task_t* task_new_engine_task_t(const char* name, long interval_ms, int (*callback)(game_t*));
void task_free_task_t(task_t* task);
void task_deallocate_task_t(void* value);

int task_schedule_task(linked_list_t* tasks, task_t* task);
int task_cancel_task(linked_list_t* tasks, game_t* game, task_t* task);
int task_execute_tasks(linked_list_t* tasks, game_t* game);

#endif
