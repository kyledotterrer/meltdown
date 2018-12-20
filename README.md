## Meltdown 

An exploration of the Meltdown processor vulnerability. 

### Overview

In order of integration: 

- `kernelmod.c`: a small kernel module that we compile and install prior to running the attack in order to insert a known, secret value into kernel memory. 

- `cachetime.c`: a quick look at how we can use timing to determine the presence of particular data in the cache. 
- `flushreload.c`: using the cache flush+reload technique to determine a secret value based on cache timing. 
- `testaccess.c`: try accessing kernel memory without any of the additional powers that exploiting the Meltdown vulnerability affords us.
- `exception.c`: a look at handling the SIGSEGV signal raised by illegal memory accesses, and how doing so allows us to continue execution even after our process is deemed guilty of such an operation. 
- `ooe.c`: now incorporating and relying upon exploitation of out of order execution in the processor to observe the microarchitectural state changes caused by instructions that, semantically (i.e. at the architectural level), are never actually executed. 
- `vanilla.c`: first attempt at full Meltdown attack on a single secret byte. 
- `improved.c`: a slightly improved version of the vanilla attack that utilizes the kernel module functionality to ensure that the secret data is cached prior to initiation of the attack. 
- `asm.c` a further refinement of the improved attack via inline assembly code that increases the probability of attack success. 
- `practical.c` coming full circle on the single-byte attack by running the attack over many iteration to reduce noise. 
- `full.c` extension of the practical attack to read arbitrary-size regions of kernel memory. 

### Installing the Kernel Module 

One of the very first steps in setting up this (admittedly simplifed) version of the Meltdown attack is to compile and install the kernel module implemented in `kernelmod.c`. To do so, first compile the module

```$ make kernelmod```

and install it with

```$ insmod kernelmod.ko```

Unless you are developing as `root`, the above command will need to be prefixed by `sudo`. 

Once the module is installed, we can use the `dmesg` command to print the kernel buffer and find out the address of the secret data that we have stored in kernel memory

```$ dmesg | grep 'helpful string you have inserted into module as grep hook'```

When complete, uninstall the kernel module with

```$ rmmod kernelmod```

Again, this operation requires root privileges. 

### Acknowledgements

Adapted from Professor Wenliang Du's original [laboratory exercise](http://www.cis.syr.edu/~wedu/seed/Labs_16.04/System/Meltdown_Attack/). 
