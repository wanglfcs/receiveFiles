////////////////////////////////////////////////////////////////////////////
//                                                                        //
// timerInterrupt.h                                                       //
//                                                                        //
// Hardware timer interrupt module                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef _TIMERINTERRUPT_H
#define _TIMERINTERRUPT_H

#include "intercore.h"

#define DEVICE_TIMERINTRERUPT ((6 << 2) | 2)

static volatile unsigned int* timerInerruptControl_mask = (unsigned int *)0x0000001a;
static volatile unsigned int* timerInerruptControl_timeInterval = (unsigned int *)0x0000003a;
static volatile unsigned int* timerInerruptControl_handlerR = (unsigned int *)0x0000005a;
static volatile unsigned int* timerInerruptControl_epc = (unsigned int *)0x0000007a;

void set_mask(unsigned int mask);
/* {   
    //set timer interrupt mask --- enable or disable: 1 for disable, 0 for enable
    
    *timerInerruptControl_mask = mask;
}
*/
void set_timerInterval(unsigned int timeInterval);
/* {   
    //set timer intervals----send a timer interrupt request every $(timeInterval) clocks, if mask enabled
    
    *timerInerruptControl_timeInterval = timeInterval;
}
*/
void set_handlerR(unsigned int handler);
/* {   
    //set timer interrupt handler routing
    
    *timerInerruptControl_handlerR = handler;
}
*/
static unsigned int read_mask() {
  // Read current mask bit ---- only one bit valid
  //
  return *(unsigned int *)(DEVICE_TIMERINTRERUPT | (0 << 5));
}

static unsigned int read_timeIntervals() {
  // Read timer intervals 
  //
  return *(unsigned int *)(DEVICE_TIMERINTRERUPT | (1 << 5));
}

static unsigned int read_handlerR() {
  // Read current handlerR
  //
  return *(unsigned int *)(DEVICE_TIMERINTRERUPT | (2 << 5));
}

static unsigned int read_epc() {
  // Read EPC ---- return from interrupt handler
  //
  return *(unsigned int *)(DEVICE_TIMERINTRERUPT | (3 << 5));
}

void isr();
  
#endif
