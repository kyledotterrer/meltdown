/*
 * testaccess.c
 * See if we can directly access kernel memory from user process. 
 * (the answer should not surpise you)
 *
 * Kyle Dotterrer
 * December, 2018 
 */

#include <stdio.h>
#include <stdlib.h>

/* ----------------------------------------------------------------------------
	Constants
*/

// NOTE: this address will change each time kernal module is reinstalled 
#define KERNEL_ADDR 0xf8d73000

/* ----------------------------------------------------------------------------
	Main 
*/

int main(void) {
	char *kernel_data_addr = (char *) KERNEL_ADDR; 
	char kernel_data = *kernel_data_addr; 
	printf("success!\n");

	exit(0);
}
