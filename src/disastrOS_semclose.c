#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

// function automatically called by the semClose whenever a semaphore has been closed and
// no other process uses it
void semUnlink(Semaphore* sem){
  sem=(Semaphore*) List_detach(&semaphores_list, (ListItem*) sem);

  // the semaphore should be in the semaphore list, i. e. sem is not null
  assert(sem);
  // freeing memory
  Semaphore_free(sem);

  return;
}


//releases from an application the given
// returns 0 on success
// returns an error code if the semaphore is not owned by the application
void internal_semClose(){
  int semfd=running->syscall_args[0];

  SemDescriptor* des=SemDescriptorList_byFd(&running->sem_descriptors, semfd);
  // checks if the given file descriptor exists for the running process
  if (! des){
    disastrOS_debug("No semaphore with the given fd\n");
    running->syscall_retvalue=DSOS_ENOSEM;
    return;
  }
  disastrOS_debug("Removing semaphore with the given fd\n");

  // removes the descriptor from the process list
  des = (SemDescriptor*) List_detach(&running->sem_descriptors, (ListItem*) des);
  assert(des);

  // retrieving the semaphore open with the given descriptor
  Semaphore* sem=des->semaphore;

  // removes the descriptor pointer from the semaphore list
  SemDescriptorPtr* desptr=(SemDescriptorPtr*) List_detach(&sem->descriptors, (ListItem*)(des->ptr));

  // desptr should not be null (the descriptor is in the semaphore list)
  assert(desptr);

  // freeing allocated memory
  SemDescriptor_free(des);
  SemDescriptorPtr_free(desptr);

  // the semaphore is removed if all the process that open it closed it
  if(sem->descriptors.size == 0){
    semUnlink(sem);
  }

  // returning success code
  running->syscall_retvalue=0;

}
