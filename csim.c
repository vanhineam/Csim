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
#include <stdbool.h>
//#include "cachelab.h"

#define OPT_LEN 20

void printUsage();
bool parseArgs(int argc, char* argv[]);

bool h = false;
bool v = false;
int s = -1;
int E = -1;
int b = -1;
char* t = NULL;

int main(int argc, char * argv[])
{
  if (!parseArgs(argc, argv))
  {
    printUsage();
    return 1;
  }

 
  printf("v = %d\n", v);
  printf("s = %d, b = %d, E = %d\n", s, b, E);
  printf("t = %s\n", t);


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

bool parseArgs(int argc, char* argv[])
{
  if(argc < 2)
  {
    return false;
  }

  int c;
  while((c = getopt(argc, argv, "hvs:b:E:t:")) != -1)
  {
    switch(c)
    {
      case 'v':
        v = true;
        break;

      case 's':
        s = atoi(optarg);
        break;

      case 'b':
        b = atoi(optarg);
        break;

      case 'E':
        E = atoi(optarg);
        break;

      case 't':
        t = optarg;
        break;

      case '?':
        if(optopt == 's')
          fprintf(stderr, "option -%c requires an argument \n", optopt);
        else if(isprint(optopt))
          fprintf(stderr, "Unkown option '-%c'.\n", optopt);
        else
          fprintf(stderr,
              "Unknown option character '\\x%x'.\n", optopt);
        return false;

      default:
        return false;
    }
  }

  if(s <= 0 || b <= 0 || E <= 0 || t == NULL)
  {
    printf("Error: missing required arguments\n");
    return false;
  }

  return true;
}
