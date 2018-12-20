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

#define KERNEL_ADDR 0xf8813000

int main(void) {
	char *kernel_data_addr = (char *) KERNEL_ADDR; 
	char kernel_data = *kernel_data_addr; 
	printf("success!\n");

	exit(0);
}
