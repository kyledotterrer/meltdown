# Makefile for meltdown stuff. 

KVERS = $(shell uname -r)

# kernel modules
obj-m += kernelmod.o

kernelmod:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules

# interesting problem addressed in the following recipes:
#	when I had the object file (.o) specified a dependency, 
#	make automatically invoked cc to compile *.c -> *.o 
#	BUT this runs into the issue of "alway_inline failed" on 
#	calls like _mm_clflush. 
#	Avoided this by calling gcc directly and not setting an object 
#	file dependency for the recipe. 
#	Likely could also have fixed by setting CC = gcc, but this works. 

cachetime: 
	gcc -Wall -Werror -pedantic -march=native  cachetime.c -o cachetime

flushreload: 
	gcc -Wall -Werror -pedantic -march=native  flushreload.c -o flushreload

testaccess: 
	gcc -Wall -Werror -pedantic -march=native  testaccess.c -o testaccess

exception: 
	gcc -Wall -Werror -pedantic -march=native  exception.c -o exception

ooe: 
	gcc -Wall -Werror -pedantic -march=native  ooe.c -o ooe

vanilla: 
	gcc -Wall -Werror -pedantic -march=native  vanilla.c -o vanilla

improved: 
	gcc -Wall -Werror -pedantic -march=native  improved.c -o improved

asm: 
	gcc -Wall -Werror -pedantic -march=native  asm.c -o asm

practical: 
	gcc -Wall -Werror -pedantic -march=native  practical.c -o practical

full:
	gcc -Wall -Werror -pedantic -march=native  full.c -o full

clean:
	rm -f *.o
	rm -f *.~
	rm -rf *.dSYM
	rm -f cachetime
	rm -f flushreload
	rm -f testaccess 
	rm -f exception 
	rm -f ooe 
	rm -f vanilla
	rm -f improved 
	rm -f asm 
	rm -f practical
	rm -f full 
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean

.PHONY: clean




