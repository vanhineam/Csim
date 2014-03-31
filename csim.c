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
#include <math.h>
#include <string.h>
//#include "cachelab.h"

#define OPT_LEN 20
#define BUFF_SIZE 80
#define BITE_SIZE 63

typedef unsigned long tag;

typedef struct
{
  int numSets;
  int setIndexBits;
  int linesPerSet; // associativiy
  int blockSize;
  int blockOffsetBits;
  tag** tags;
} Cache;

void printUsage();
bool parseArgs(int argc, char* argv[], int* s, int* E, int* b,
    char** t);
void initCache(Cache* cache, int s, int E, int b);
void deleteCache(Cache* cache);
void simulateCache(Cache* cache, FILE* fp, int* hits, int* misses, int* evics);
unsigned long getBits(int high, int low, unsigned long source);


// Verbose flag
bool verbose = false;

int main(int argc, char * argv[])
{
  int s = -1;
  int E = -1;
  int b = -1;
  char* t = NULL;
  int hits = 0;
  int misses = 0;
  int evics = 0;

  if (!parseArgs(argc, argv, &s, &E, &b, &t))
  {
    printUsage();
    return 1;
  }

  Cache cache;
  initCache(&cache, s, E, b);

  FILE * fp = fopen(t, "r");

  if(fp != NULL)
  {
    simulateCache(&cache, fp, &hits, &misses, &evics);
  }

  fclose(fp);
  deleteCache(&cache);
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

bool parseArgs(int argc, char* argv[], int* s, int* E, int* b,
    char** t)
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
        verbose = true;
        break;

      case 's':
        *s = atoi(optarg);
        break;

      case 'b':
        *b = atoi(optarg);
        break;

      case 'E':
        *E = atoi(optarg);
        break;

      case 't':
        *t = optarg;
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

  if(*s <= 0 || *b <= 0 || *E <= 0 || *t == NULL)
  {
    printf("Error: missing required arguments\n");
    return false;
  }

  return true;
}


void initCache(Cache* cache, int s, int E, int b)
{
  cache->numSets = pow(2, s);
  cache->setIndexBits = s;
  cache->linesPerSet = E;
  cache->blockSize = pow(2, b);
  cache->blockOffsetBits = b;

  cache->tags = malloc(cache->numSets * sizeof(tag*));
  for (int i = 0; i < cache->numSets; i++)
  {
    cache->tags[i] = malloc(cache->linesPerSet * sizeof(tag));
    memset(cache->tags[i], 0, cache->linesPerSet * sizeof(tag));
  }
}

void deleteCache(Cache* cache)
{
  for (int i = 0; i < cache->numSets; i++)
  {
    free(cache->tags[i]);
    cache->tags[i] = NULL;
  }
  free(cache->tags);
  cache->tags = NULL;
}

void simulateCache(Cache* cache, FILE* fp, int* hits, int* misses, int* evics)
{
  char buff[BUFF_SIZE];
  int setBits = 0;
  int tagBits = 0;
  int numOfSetBits = cache->setIndexBits;
  int numOfBlockOffset = cache->blockOffsetBits;

  char operation;
  unsigned long address = 0;
  int size = 0;

  while(fgets(buff, BUFF_SIZE, fp))
  {
    int i;

    sscanf(buff, " %c %lx,%d", &operation, &address, &size);
    tagBits = getBits(63, numOfBlockOffset + numOfSetBits, address);
    setBits = getBits(numOfBlockOffset + numOfSetBits - 1,
      numOfBlockOffset, address);

    for(i = 0; i < cache->linesPerSet; i++)
    {
      tag line = -1;
      tag* set = cache->tags[setBits];
      line = set[i];
      printf("CurrentLine: %lu    ", line);
      if(line == -1)
      {
        misses++;
        if(verbose)
        {
          printf("miss");
        }
      }
      else if(line == tagBits)
      {
        hits++;
        if(verbose)
        {
          printf("hit");
        }
      }
      else
      {
        misses++;
        evics++;
        if(verbose)
        {
          printf("miss eviction");
        }
      }

      if(operation == 'M')
      {
        hits++;
        if(verbose)
        {
          printf(" hit \n");
        }
      }
      else if(verbose)
      {
        printf("\n");
      }

      set[i] = tagBits;
    }

    if(feof(fp))
    {
      break;
    }
  }
}

unsigned long getBits(int high, int low, unsigned long source)
{
  return (source << (63 - high)) >> (63 - high + low);
}