/*
 * exception.c
 * A look at handling exceptions, in this case the SIGSEGV signal 
 * raised by attempted access to kernel memory, and how we can save 
 * and restore context and continue execution in the face of such events.
 *
 * Kyle Dotterrer
 * December, 2018 
 */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>

// address of secret data in kernel memory 
// NOTE: this address will change each time kernal module is reinstalled 
#define KERNEL_ADDR 0xf8d73000

static sigjmp_buf jbuf;

static void catch_segv() {
	// roll back to checkpoint set by sigsetjmp
	// copy the context saved in jbuf back to the processor state
	// now, however, the returned value from sigsetjmp is the second 
	// argument of siglongjmp (1 here) so the branch is now taken 
	// (whereas before it was not taken)
	siglongjmp(jbuf, 1);
}

int main(void) {
	unsigned long kernel_data_addr = KERNEL_ADDR; 

	// setup a signal handler for SIGSEGV
	signal(SIGSEGV, catch_segv);

	// setsetjmp saves stack context in jbuf for later use by siglongjmp
	// sigsetjmp returns 0 when this checkpoint is set up
	if (sigsetjmp(jbuf, 1) == 0) {
		// SIGSEV raised by this 
		char kernel_data = *(char *)kernel_data_addr;

		// you think we get here? 
		printf("kernel data at address %lu is: %c\n", kernel_data_addr, kernel_data);
	} else {
		printf("memory access violation!\n");
	}

	printf("program continues execution\n");

	exit(0);
}
