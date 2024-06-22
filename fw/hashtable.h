#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>


typedef struct Item {
  char* key; /* Word */
  unsigned long int value; /* Frequency */
} Item;

typedef struct HashTable{
  struct Item** elements;
  long unsigned int capacity;
  long unsigned int num_elements;
  double load_factor;
} HashTable;

HashTable* constructor(void);
void rehash(HashTable* table);

char* my_get_word(FILE *file);

int compare(const void * a, const void * b);

Item** create_array(HashTable *table);

void insert(HashTable* table, char* input_key, long unsigned int input_val);

#endif 
