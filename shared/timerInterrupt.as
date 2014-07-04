////////////////////////////////////////////////////////////////////////////
//                                                                        //
// timerInterrupt                                                         //
//                                                                        //
// Raw inter-core hardware: messages, cache management, semaphores.       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

// Imports:
	.include "stdas.as"

// Exports:
	.file "timerInterrupt.as"
	
      	
	.globl	_set_mask
	.globl	_set_timerInterval
	.globl	_set_handlerR

// Constants:
	timerInerruptControl_mask = 26
	timerInerruptControl_timeInterval = 58
	timerInerruptControl_handlerR = 90


	
////////////////////////////////////////////////////////////////////////////

//                                                                        //
// void message_send(int dest, int type, IntercoreMessage *buf, int len)  //
//                                                                        //
// Send a message to core number "dest", using "len" words at "buf".      //
//                                                                        //
// Arguments are in r3-r6                                                 //
//                                                                        //
// The implementation has no uses of LINK, including long_* ops, so we    //
// don't need to preserve LINK on the stack.                              //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
	.type	_set_mask, @function
_set_mask:
    ld  wq, r3
    aqw_ld vb, timerInerruptControl_mask
	j	link
	.size	_set_mask,.-_set_mask

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// unsigned int message_recv(IntercoreMessage *buf)                       //
//                                                                        //
// Receive a message into "buf" and return its status, or 0.              //
//                                                                        //
// Argument is in r3, result goes in r1                                   //
//                                                                        //
// The implementation has no uses of LINK, including long_* ops, so we    //
// don't need to preserve LINK on the stack.                              //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
	.type	_set_timer, @function
_set_timerInterval:
    ld  wq, r3
    aqw_ld vb, timerInerruptControl_timeInterval 
	j	link
	.size	_set_timerInterval,.-_set_timerInterval


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// void cache_flush(int line, int countMinus1)                            //
//                                                                        //
// Flush a cache line                                                     //
//                                                                        //
// Arguments are in r3-r4                                                 //
//                                                                        //
// The implementation has no uses of LINK, including long_* ops, so we    //
// don't need to preserve LINK on the stack.                              //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
	.type	_set_handlerR, @function
_set_handlerR:
    ld  wq, r3
    aqw_ld vb, timerInerruptControl_handlerR
	j	link
	.size	_set_handlerR,.-_set_handlerR
