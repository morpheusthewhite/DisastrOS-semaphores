CC=gcc
CCOPTS=--std=gnu99 -Wall -I include/
AR=ar

HEADERS=include/disastrOS.h\
	include/disastrOS_constants.h\
	include/disastrOS_descriptor.h\
	include/disastrOS_globals.h\
	include/disastrOS_pcb.h\
	include/disastrOS_resource.h\
	include/disastrOS_syscalls.h\
	include/disastrOS_timer.h\
	include/disastrOS_semaphore.h\
	include/disastrOS_semdescriptor.h\
	include/linked_list.h\
	include/pool_allocator.h\
	include/disastrOS_semtest.h

OBJS=pool_allocator.o\
     linked_list.o\
     disastrOS_timer.o\
     disastrOS_pcb.o\
     disastrOS_resource.o\
     disastrOS_descriptor.o\
     disastrOS_semaphore.o\
     disastrOS_semdescriptor.o\
     disastrOS.o\
     disastrOS_fork.o\
     disastrOS_wait.o\
     disastrOS_spawn.o\
     disastrOS_exit.o\
     disastrOS_shutdown.o\
     disastrOS_schedule.o\
     disastrOS_preempt.o\
     disastrOS_sleep.o\
     disastrOS_open_resource.o\
     disastrOS_close_resource.o\
     disastrOS_destroy_resource.o\
     disastrOS_semopen.o\
     disastrOS_semclose.o\
     disastrOS_semwait.o\
     disastrOS_sempost.o\
     disastrOS_semtest.o


LIBS=libdisastrOS.a

BINS=disastrOS_test

#disastros_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	src/%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libdisastrOS.a: $(OBJS) $(HEADERS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

disastrOS_test: src/disastrOS_test.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
