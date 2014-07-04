////////////////////////////////////////////////////////////////////////////
//                                                                        //
// monitor.h                                                             //
//                                                                        //
// L2 cache performance counters                                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef _MONITORS_H
#define _MONITORS_H

#include "intercore.h"

#define DEVICE_MONITORS ((7 << 2) | 2)

static void clean_monitors(unsigned int cnt) {
  //clear "cnt" count of the monitor module
  //only 0, 1, 2, 3 being used
  //
  rawWrite((unsigned int *)(DEVICE_MONITORS | ((cnt & 0x00000003) << 5));
}

unsigned int read_monitors(unsigned int cnt) {
  //get "cnt" count of the monitor module
  //only 0, 1, 2, 3 being used
  //
  return *(unsigned int *)(DEVICE_MONITORS | ((cnt & 0x00000003) << 5));
}
  
#endif
