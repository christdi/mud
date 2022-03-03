#ifndef MUD_NARRATOR_H
#define MUD_NARRATOR_H

/**
 * Structs
**/
typedef struct narrator {
  int ref;
} narrator_t;

/**
 * Function prototypes
**/
narrator_t* narrator_new_narrator_t(int ref);
void narrator_free_narrator_t(narrator_t* narrator);

#endif