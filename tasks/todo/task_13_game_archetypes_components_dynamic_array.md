# Replace Archetypes and Components Linked Lists with Dynamic Arrays

`game->archetypes` is iterated on every entity component change. `game->components` is iterated when an entity is deleted. Both are linked lists that are populated at startup and rarely modified.

**Solution:** Replace both with `ptr_array_t` (task 10). Note that functions accepting these lists as parameters will need their signatures updated.
