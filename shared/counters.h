////////////////////////////////////////////////////////////////////////////
//                                                                        //
// counters.h                                                             //
//                                                                        //
// Hardware performance counters                                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef _COUNTERS_H
#define _COUNTERS_H

#include "intercore.h"

#define DEVICE_COUNTERS ((7 << 2) | 2)

static void counters_start() {
  // Set "run" in the counters I/O device
  //
  rawWrite((unsigned int *)(DEVICE_COUNTERS | (1 << 5)));
}
  
static void counters_stop() {
  // Clear "run" in the counters I/O device
  //
  rawWrite((unsigned int *)(DEVICE_COUNTERS | (0 << 5)));
}
  
static unsigned int counters_readAndZero(int n) {
  // Read and zero one of the counters in the counters I/O device.
  // Assumes n is in [0..63]
  //
  return *(unsigned int *)(DEVICE_COUNTERS | (n << 5));
}
  
#endif
