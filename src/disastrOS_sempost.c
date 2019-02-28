#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  // retrieves semaphore number from syscall arguments
  int semnum = running->syscall_args[0];

  SemDescriptor* des=SemDescriptorList_bySemnum(&running->sem_descriptors, semnum);
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

    // retrieves the PCB of the process, setting its status as ready and putting it in the ready queue
    PCB* next_running_PCB = next_running_sem_desc_ptr->descriptor->pcb;
    next_running_PCB->status=Ready;

    // freeing allocated semaphore pointer
    SemDescriptorPtr_free(next_running_sem_desc_ptr);

    List_insert(&ready_list, ready_list.first, (ListItem*) next_running_PCB);
  }

  return;
}
