# Flatten `hash_node_t` Allocations

**Priority:** 05
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low

## What

Combine the two heap allocations made per `hash_table_insert` call (one for the `hash_node_t` struct, one for the key string via `strdup`) into a single allocation using a flexible array member.

## Why

Every insert currently allocates memory twice: `create_hash_node_t()` allocates the node struct, and `strdup(key)` allocates a copy of the key string separately. These two objects always have the same lifetime (freed together in `hash_node_free`). A single allocation is simpler, reduces heap fragmentation, and improves cache locality (the key string is adjacent to the struct fields that reference it).

## How

### 1. Update `hash_node_t` — `src/data/hash_table/hash_node.h`

Replace the `char* key` pointer field with a C99 flexible array member:

```c
// BEFORE
typedef struct hash_node_t {
    char* key;
    void* value;
    void (*deallocator)(void*);
} hash_node_t;

// AFTER
typedef struct hash_node_t {
    void* value;
    void (*deallocator)(void*);
    char key[];   // flexible array member — key string stored inline
} hash_node_t;
```

### 2. Update `hash_node_new` — `src/data/hash_table/hash_node.c`

Change `create_hash_node_t` (or `hash_node_new`) to accept the key and allocate the combined struct+key in one call:

```c
// BEFORE
hash_node_t* create_hash_node_t(void) {
    return calloc(1, sizeof *node);
}

// AFTER
hash_node_t* hash_node_new(const char* key, size_t key_len) {
    hash_node_t* node = calloc(1, sizeof *node + key_len + 1);
    if (!node) return NULL;
    memcpy(node->key, key, key_len);
    node->key[key_len] = '\0';
    return node;
}
```

The `key_len` argument avoids a second `strlen` call since the caller already knows it.

### 3. Update `hash_node_free` — `src/data/hash_table/hash_node.c`

Remove the separate `free(node->key)` call — the key is now part of the same allocation as the struct:

```c
// BEFORE
void node_free(hash_node_t* node) {
    assert(node);
    if (node->deallocator) node->deallocator(node->value);
    free(node->key);   // REMOVE THIS
    free(node);
}

// AFTER
void hash_node_free(hash_node_t* node) {
    assert(node);
    if (node->deallocator) node->deallocator(node->value);
    free(node);
}
```

### 4. Update `hash_table_insert` — `src/data/hash_table/hash_table.c`

Remove the `strdup(key)` call and the now-unused `len` local variable. Pass the key directly to `hash_node_new`:

```c
// BEFORE
char* hash_key = strdup(key);
size_t len = strnlen(hash_key, MAX_KEY_LENGTH - 1);
// ... truncation block (already removed by task 01) ...
hash_node_t* hash_node = create_hash_node_t();
hash_node->key = hash_key;

// AFTER
size_t key_len = strnlen(key, MAX_KEY_LENGTH);
hash_node_t* hash_node = hash_node_new(key, key_len);
if (!hash_node) return -1;
```

### 5. Review all other access to `node->key`

Search for `node->key` and `hash_node->key` across the codebase to confirm no other code frees or reassigns the key field:

```
grep -rn "->key" src/data/hash_table/
```

The key is read in `hash_table_delete` and `hash_table_get` for comparison — these remain unchanged since `node->key` still works as a `char*` (flexible array members decay to pointers).

### Verification

Build cleanly. Run the game and exercise operations that use the hash table heavily: entity registration, component lookup, system registration. These all go through the hash table.
