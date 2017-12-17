#define _FILE2_LOAD_FROM_MEMORY_
#include "sspc.c"


int a[11] = {1,2,3,INTHUGE,2,4,INTHUGE,3,4,INTHUGE-1}, *buf;

main (int argc, char *argv[]){
  __load_from_memory_org__ = a;
  EXECSUB (SSPC_main, 0, exit, "sspc R void 0.2 void", 0);
  buf = (int *)__write_to_memory_org__;
  while (*buf != INTHUGE-1){
    printf ("%d\n", *buf);
    buf++;
  }
  free2 (__write_to_memory_org__);
}



