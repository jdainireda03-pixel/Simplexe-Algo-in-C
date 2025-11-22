//  NOTE: do not remove the main stdio, because the other two files will be
//  working with it.
#include <stdio.h>
#define MAX_SLACK 10
#define MAX_VARS 10
#define big_M 10000.0

typedef enum { false = 0, true = 1 } bool;

struct simplexe {
  int m;      // number of constraints
  int n;      // number of variables (slack ones not included)
  int is_min; // 0 for max 1 for min
  double tableau[MAX_SLACK + 1]
                [MAX_VARS + MAX_SLACK + MAX_SLACK + 1]; // evident
};
// function to find the entering col
