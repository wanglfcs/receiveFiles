////////////////////////////////////////////////////////////////////////////
//                                                                        //
// mcMain.c                                                               //
//                                                                        //
// "main" for simple multi-core programming.                              //
//                                                                        //
// This module implements "main", called from base.o in core #1.          //
//                                                                        //
// The application should provide functions "mc_init" and "mc_main".      //
//                                                                        //
// "main" calls "mc_init", and on return from there it allocates stacks   //
// for  all the other cores, then resumes their execution at a call of    //
// "mc_main", concurrently in each core.                                  //
//                                                                        //
// Return from "mc_main" in any core just does an infinite loop there.    //
//                                                                        //
// Seel also mcLibc, for multi-core implementations of putchar, malloc    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "intercore.h"
#include "threads.h"


// Entry points defined in the application
//
void mc_init();
void mc_main();

// Private entry point for running multi-core putchar, malloc, etc.
//
void mc_initRPC();

static void mc_exit() {
  // This is reached on return from mc_main (by "jump", not "call")
  //
  for (;;) {}
}

static void forkee(void *arg) {
  // Forked onto a full-size stack
  //
  unsigned int status;
  IntercoreMessage buf = {0};
  mc_initRPC();
  int nCores = enetCorenum();
  printf("addr of mc_main: %x\n", (unsigned int)mc_main);
  for (int core = 2; core < nCores; core++) {
    SaveArea *save = getSaveArea(core);
    save->sp = malloc(100000) + 100000;
    save->link = (unsigned int)mc_exit;
    save->pc = (unsigned int)mc_main;
    //printf("SaveArea: %x, sp: %x, link: %x, pc: %x\n", save, save->sp, save->link, save->pc);
    //printf("SaveArea->pc %x\n", &(save->pc));
    //printf("first code of mc_main(): %x\n", *((unsigned int*) 0xc000));
    cache_flushMem(save, sizeof(SaveArea));
  }
  mc_init();
  printf("after mc_init(), going to send messages to slave cores\n");
  for (int core = 2; core < nCores; core++) {
    //printf("%d\n", core);
    message_send(core, 0, NULL, 0);    
/*
    status = 0;
    while(status == 0) {
      status = message_recv(buf);
    }
    printf("get content %x %x from core %d type %d\n", buf[0], buf[1], message_srce(status), message_type(status));
    */
  }
}

int main (int argc, const char * argv[]) {
  printf("\n%d cores, clock speed is %d MHz\n",
	 enetCorenum()-1, clockFrequency());
  thread_fork(forkee, NULL);
  thread_exit(0);
}
