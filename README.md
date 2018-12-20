## Meltdown 

An exploration of the Meltdown processor vulnerability. 

### Overview

In order of integration: 

- `cachetime.c`: a quick look at how we can use timing to determine the presence of particular data in the cache. 
- `flushreload.c`: using the cache flush+reload technique to determine a secret value based on cache timing. 

### Acknowledgements

Adapted from Professor Wenliang Du's original [laboratory exercise](http://www.cis.syr.edu/~wedu/seed/Labs_16.04/System/Meltdown_Attack/). 
