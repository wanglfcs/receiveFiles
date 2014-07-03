// BEEHIVE GCC 2010.07.09.18.09  (no pre-modify) (mult)
// ------------------------------------------------------------
// Standard as definitions

zero = $0    // fixed zero
void = $0    // void destination
r1 = $1      // function return value
r2 = $2      // not callee save
r3 = $3      // not callee save, function argument 1
r4 = $4      // not callee save, function argument 2
r5 = $5      // not callee save, function argument 3
r6 = $6      // not callee save, function argument 4
r7 = $7      // not callee save, function argument 5
r8 = $8      // not callee save, function argument 6
r9 = $9      // callee save
r10 = $10    // callee save
r11 = $11    // callee save
r12 = $12    // callee save
r13 = $13    // callee save
r14 = $14    // callee save
r15 = $15    // callee save
r16 = $16    // callee save
r17 = $17    // callee save
r18 = $18    // callee save
r19 = $19    // callee save
r20 = $20    // callee save
r21 = $21    // callee save
r22 = $22    // callee save
fp = $23     // callee save, frame pointer
t1 = $24     // not callee save, temporary 1, not avail for reg alloc
t2 = $25     // not callee save, temporary 2, not avail for reg alloc
t3 = $26     // not callee save, temporary 3, not avail for reg alloc
p1 = $27     // not callee save, platform 1, not avail for reg alloc
sp = $28     // callee save, stack pointer
vb = $29     // not callee save, rw & rb only, not avail for reg alloc

    .assume   zero,0

// ------------------------------------------------------------
	.file	"isr.c"
	.code
	.alignw	1
	.globl	_isr
	.type	_isr, @function
_isr:
// 4 "isr.c" 1
	//close the interrupt
	ld		wq,1
	aqw_ld void,26

	//save 0-30 register
	ld       wq, $1	
	ld       wq, $2  
	ld       wq, $3  
	ld       wq, $4  
	ld       wq, $5  
	ld       wq, $6  
	ld       wq, $7  
	ld       wq, $8  
	ld       wq, $9  
	ld       wq, $10 
	ld       wq, $11 
	ld       wq, $12 
	ld       wq, $13 
	ld       wq, $14 
	ld       wq, $15 
	ld       wq, $16 
	ld       wq, $17 
	ld       wq, $18 
	ld       wq, $19 
	ld       wq, $20 
	ld       wq, $21 
	ld       wq, $22 
	ld       wq, $23 
	ld       wq, $24 
	ld       wq, $25 
	ld       wq, $26 
	ld       wq, $27 
	ld       wq, $28 
	add      wq, zero, $29  
	ld       wq, link
	//write address
	ld		  $1,30
	j7       1        
	sub  link, link, 4 
	aqw_add  link, link, 4 
	sub      $1, $1, 1 
	jnz      .-2 
	//get condition register
	aqr_ld	  vb,250
	ld		  $2,rq
	ld		  wq,$2
	aqw_add  link,link,4
	//get saved pc
	aqr_ld	  vb,122
	ld		  $2,rq
	ld		  wq,$2
	aqw_add  link,link,4
	//ld		sp,link

	//do schedule
	long_call   _schedule

	j7  1
	sub      link, link, 4  
	ld       $1, 32      
	aqr_add  link, link, 4  
	sub      $1, $1, 1
	jnz      .-2            
	ld       $1, rq         
	ld       $2, rq         
	ld       $3, rq         
	ld       $4, rq         
	ld       $5, rq         
	ld       $6, rq         
	ld       $7, rq         
	ld       $8, rq         
	ld       $9, rq         
	ld       $10, rq        
	ld       $11, rq        
	ld       $12, rq        
	ld       $13, rq        
	ld       $14, rq        
	ld       $15, rq        
	ld       $16, rq        
	ld       $17, rq        
	ld       $18, rq        
	ld       $19, rq        
	ld       $20, rq        
	ld       $21, rq        
	ld       $22, rq        
	ld       $23, rq        
	ld       $24, rq        
	ld       $25, rq        
	ld       $26, rq        
	ld       $27, rq        
	ld       $28, rq        
	ld       $29, rq        
	ld       link, rq       
	
	//open interrupt
	ld		wq,0

	//set conditon register
	ld	    wq,rq
	aqw_ld void,26
	aqw_ld void,250

	j		  rq             
//	j .
	
	.size	_isr, .-_isr
	.ident	"GCC: (GNU) 4.3.3"
