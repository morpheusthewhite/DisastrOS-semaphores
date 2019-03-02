#include "disastrOS.h"

#define SEM_TEST 10

// evil tests for the semaphores
void semTestFunction(){
  int i;
  int sems[SEM_TEST];

  printf("Opening a lot of stuff\n");
  for(i=0; i<SEM_TEST; i++){
    sems[i] = disastrOS_semOpen(i);
    if(sems[i] < 0){
      printf("Failed to create semaphore with id %d\n", i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Trying to open semaphores with negative id\n");
  for(i=1; i<SEM_TEST; i++){
    if(disastrOS_semOpen(-i) >= 0){
      printf("Successfully create semaphore with negative id (%d)\n", -i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Closing all open sems\n");
  for(i=SEM_TEST-1; i>=0; i--){
    if(disastrOS_semClose(sems[i]) < 0){
      printf("Failed to close semaphore with id %d\n", i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Closing again all open sems\n");
  for(i=0; i<SEM_TEST; i++){
    if(disastrOS_semClose(i) >= 0){
      printf("Closed again semaphore with id %d!\n", i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Posting and waiting on previously closed semaphores\n");
  for(i=0; i<SEM_TEST; i++){
    if(disastrOS_semPost(i) >= 0){
      printf("Successfully posted on a closed semaphore!\n");
      return;
    }
    if(disastrOS_semWait(i) >= 0){
      printf("Successfully waited on a closed semaphore!\n");
      return;
    }
  }

  disastrOS_printStatus();

  printf("Opening again lot of stuff\n");
  for(i=SEM_TEST; i<2*SEM_TEST; i++){
    sems[i] = disastrOS_semOpen(i);
    if(sems[i] < 0){
      printf("Failed to create semaphore with id %d\n", i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Posting and waiting on opened semaphores\n");
  for(i=SEM_TEST; i<2*SEM_TEST; i++){
    if(i%2 && disastrOS_semPost(i) < 0){
      printf("Failed to post on semaphore %d!\n", i);
      return;
    }
    else if(disastrOS_semWait(i) < 0){
      printf("Failed to wait on semaphore %d!\n", i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Waiting and posting on opened semaphores\n");
  for(i=SEM_TEST; i<2*SEM_TEST; i++){
    if(i%2 && disastrOS_semWait(i) < 0){
      printf("Failed to post on semaphore %d!\n", i);
      return;
    }
    else if(disastrOS_semPost(i) < 0){
      printf("Failed to wait on semaphore %d!\n", i);
      return;
    }
  }

  disastrOS_printStatus();
  int sems2[SEM_TEST];

  printf("Opening already opened semaphores\n");
  for(i=SEM_TEST; i<2*SEM_TEST; i++){
    sems2[i] = disastrOS_semOpen(i);
    if(sems2[i] < 0){
      printf("Failed to create semaphore with id %d\n", i);
      return;
    }
  }

  disastrOS_printStatus();

  printf("Closing twice opened semaphores\n");
  for(i=SEM_TEST; i<2*SEM_TEST; i++){
    if(disastrOS_semClose(sems[i]) < 0){
      printf("Failed to close semaphore with id %d\n", i);
      return;
    }
    if(disastrOS_semClose(sems2[i]) < 0){
      printf("Failed to close semaphore with id %d\n", i);
      return;
    }
  }

  disastrOS_printStatus();
  printf("Semaphore tests completed successfully!\n");

  disastrOS_exit(disastrOS_getpid()+1);
}

