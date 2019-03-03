#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

// creates a semaphore in the system, having id semnum
// the semaphore is accessible throughuot the entire system
// by its id.
// on success, the function call returns the fd (>=0);
// in failure the function returns an error code (<0)
void internal_semOpen(){
  int semnum = running->syscall_args[0];

  if(running->sem_descriptors.size > MAX_NUM_SEMDESCRIPTORS_PER_PROCESS){
    disastrOS_debug("Semaphore allocation failed: no more descriptors available for the process\n");
    running->syscall_retvalue = DSOS_ESEMOPEN;
    return;
  }

  if(semnum < 0){
    // invalid semnum
    disastrOS_debug("Requested semaphore with invalid id");
    running->syscall_retvalue = DSOS_ESEMNEG;
    return;
  }

  // calling SemaphoreList_byId the semaphore with the given id is retrieved if a
  // it already exists, otherwise the function will return NULL
  disastrOS_debug("Finding semaphore with id: %d (if exists)\n", semnum);
  Semaphore* sem = SemaphoreList_byId(&semaphores_list, semnum);

  if(sem == NULL){
    // the semaphore does not exist, it must be allocated
    disastrOS_debug("Allocating semaphore with id: %d\n", semnum);

    // all semaphores will be binary
    sem = Semaphore_alloc(semnum, 1);

    // checks if the semaphore was correctly allocated
    if(sem == NULL) {
      disastrOS_debug("Semaphore allocation failed\n");
      running->syscall_retvalue = DSOS_ESEMOPEN;
      return;
    }

    disastrOS_debug("Semaphore allocation succeded\n");

    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);
  }

  disastrOS_debug("Semaphore correctly allocated/retrieved\n");
  // allocates the descriptor of the semaphore for the current process
  SemDescriptor* des = SemDescriptor_alloc(running->last_sem_fd, sem, running);
  if(! des){
    running->syscall_retvalue = DSOS_ESEMNOFD;
    return;
  }

  disastrOS_debug("Descriptor correctly created\n");

  running->last_sem_fd++; // increments the count of opened semaphores for the process

  // adds the opened semaphore among the ones of the current process
  SemDescriptorPtr* descptr = SemDescriptorPtr_alloc(des);
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) des);

  // adds the descriptor to the semaphore struct
  des->ptr=descptr;
  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) descptr);

  running->syscall_retvalue = des->fd;
  return;
}
