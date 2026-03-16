# Fix Thread-Safety Warnings

clang-tidy flags ~31 `concurrency-mt-unsafe` warnings. The majority are `strerror` calls inside the linked list mutex error paths, which will disappear when task 4 is completed.

The remaining warnings are `localtime` and `strerror` calls in the network and logging code, plus `getopt` and `exit` in startup code.

**Solution:** Replace `localtime` and `strerror` with their reentrant `_r` variants. Suppress `getopt` and `exit` where no thread-safe alternative exists — the game is single-threaded.
