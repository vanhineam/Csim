/**
 * File: csim.c
 * 
 * Name: Adam Van Hine
 * Username: vanhineam
 *
 * Description: Cache simulator that takes a "valgrind" memory trace as input,
 * simulates the hit/miss behavior of a cache memory on this trace, and outputs
 * the total number of hits, misses and evictions.
 * 
 */
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
//#include "cachelab.h"

#define OPT_LEN 20

void printUsage();

int main(int argc, char * argv[])
{
  // Create the set, block, and associativity variables.
  int s = 0;
  int b = 0;
  int E = 0;
  int c;
  int index;
  
  if(argc < 2)
  {
    printUsage();
    return 1;
  }
  // Handle command-line arguements.
  while((c = getopt(argc, argv, "s:b:E:")) != -1)
  {
    switch(c)
    {
      case 's':
        s = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case '?':
        if(optopt == 's')
          fprintf(stderr, "option -%c requires an argument \n", optopt);
        else if(isprint(optopt))
          fprintf(stderr, "Unkown option '-%c'.\n", optopt);
        else
          fprintf(stderr,
              "Unknown option character '\\x%x'.\n", optopt);
        return 1;
      default:
        printUsage();
        abort();
    }
  }

  if( s == 0 || b == 0 || E == 0)
  {
    printf("Error: missing required arguments\n");
    printUsage();
    exit(1);
  }
  
  printf("s = %d, b = %d, E = %d\n", s, b, E);

  for(index = optind; index < argc; index++)
  {
    printf("Non-option argument %s\n", argv[index]);
  }

  return 0;

}

void printUsage()
{
  printf("%s\n\n", "Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>");
  printf("%s\n", "-h: Optional help flag that prints this text.");
  printf("%s\n", "-v: Optional verbose flag that displays trace info.");
  printf("%s\n", "-s <s>: Number of set index bits (S = 2^s is the number of "
    "sets.)");
  printf("%s\n", "-E <E>: Associativity (number of lines per set).");
  printf("%s\n", "-b <b>: Number of block bits (B = 2^b is the block size.)");
  printf("%s\n\n", "-t <tracefile>: Name of the valgrind trace to replay.");
}

