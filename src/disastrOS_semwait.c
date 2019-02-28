#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  // retrieves semaphore number from syscall arguments
  int semnum = running->syscall_args[0];

  SemDescriptor* des=SemDescriptorList_bySemnum(&running->sem_descriptors, semnum);
  // checks if the given file descriptor exists for the running process
  if (! des){
    disastrOS_debug("No open semaphore with the given fd\n");
    running->syscall_retvalue=DSOS_ENOSEM;
    return;
  }

  disastrOS_debug("Waiting on semaphore with fd %d\n", semnum);

  // retrieves the open semaphore with the given descriptor
  Semaphore* sem=des->semaphore;

  // decrements semaphore value
  sem->count--;

  // checks if the process should be put in the waiting queue
  if(sem->count < 0){
    running->status=Waiting;

    // allocates a semeaphore pointer to be used in the waiting queu
    SemDescriptorPtr* descptr = SemDescriptorPtr_alloc(des);
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) descptr);

    // pick the next
    PCB* next_running = (PCB*) List_detach(&ready_list, ready_list.first);
    running=next_running;
  }

  running->syscall_retvalue = 0;
  return;
}
