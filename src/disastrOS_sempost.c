#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  // retrieves semaphore fd from syscall arguments
  int semfd = running->syscall_args[0];

  SemDescriptor* des=SemDescriptorList_byFd(&running->sem_descriptors, semfd);
  // checks if the given file descriptor exists for the running process
  if (! des){
    disastrOS_debug("No open semaphore with the given fd\n");
    running->syscall_retvalue=DSOS_ENOSEM;
    return;
  }

  disastrOS_debug("Posting on semaphore with fd %d\n", semnum);

  // retrieves the open semaphore with the given descriptor
  Semaphore* sem=des->semaphore;

  // increments semaphore value
  sem->count++;
  disastrOS_debug("Incremented semaphore value\n");

  // checks if any process should be dequeued
  if(sem->count <= 0){
    // put one process in the ready queue
    SemDescriptorPtr* next_running_sem_desc_ptr = (SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, sem->waiting_descriptors.first);
    // retrieves the PCB of the process
    PCB* next_running_PCB = next_running_sem_desc_ptr->descriptor->pcb;

    // removing the process from the system waiting list
    assert(List_detach(&waiting_list, (ListItem*)next_running_PCB));
    next_running_PCB->status=Ready;

    // freeing allocated semaphore pointer
    SemDescriptorPtr_free(next_running_sem_desc_ptr);

    List_insert(&ready_list, ready_list.last, (ListItem*) next_running_PCB);
  }

  running->syscall_retvalue = 0;
  return;
}
