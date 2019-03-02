# disastrOS-semaphores
An implementation of binary semaphores for disastrOS, for the course of Operative Systems held by Giorgio Grisetti in 2018-19.

## Instructions

To compile just run
```
make
```
then
```
./disastrOS_test
```
to start the tests.

## Usage

Semaphores are system structure that wrap a counter that can be increased or decreased. Whenever it is decreased and its current value is <= 0 the process who decremented it is put in a waiting queue whose head is detached every time the counter is incremented.

To open a semaphore in disastrOS, call `disastrOS_semOpen(int semnum)`. It creates a system semaphore with global id `semnum` if it does not exist and then opens it for the calling process, putting it in the list of the opened semaphores of the process. If everything went fine returns an integer representing the descriptor of the semaphore for the calling process, otherwise an error code (see section below).

Both `disastrOS_wait(int semfd)` and `disastrOS_semPost(int semfd)` are used to change the value of the counter of the semaphore with descriptor `semfd` (the process should have opened the semaphore before calling these functions), respectively by decreasing and increasing it. On success they return 0, an error code otherwise.

Semaphores are closed by calling `disastrOS_semClose(int semfd)` and by the descriptor. When all processes using a certain semaphore closed it, it is automatically unlinked.

### Error codes

In `include/disastrOS_constants.h` are defined the following error codes:

- `DSOS_ESEMOPEN` returned by `disastrOS_semOpen` when either is opened a semaphore with negative id or no more semaphore can be created
- `DSOS_ESEMNOFD` returned by `disastrOS_semOpen` when it failed to create the SemDescriptor to the given semaphore
- `DSOS_ESEMCLOSE` returned by `disastrOS_semClose` when a process is trying to close a semaphore that does not exist or that it is not opened by the process
- `DSOS_ENOSEM` returned by `disastrOS_semWait` and by `disastrOS_semPost` when a process is trying to wait or post to a semaphore that is not opened by it.

## Valgrind test

Tested with valgrind-3.12.0.SVN, this implementation presents no memory leak. Valgrind output on an execution of the tests:

```
==17200== 
==17200== HEAP SUMMARY:
==17200==     in use at exit: 0 bytes in 0 blocks
==17200==   total heap usage: 2 allocs, 2 frees, 2,048 bytes allocated
==17200== 
==17200== All heap blocks were freed -- no leaks are possible
==17200== 
==17200== For counts of detected and suppressed errors, rerun with: -v
==17200== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

