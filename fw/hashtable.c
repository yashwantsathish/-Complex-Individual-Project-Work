/*My name is Yashwant Sathish Kumar and this is my hashmap.c, which, as
 * the name says, is my implementation of a hashmap. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
 
/*#define INIT_CAPACITY 20*/

/* creates empty table of size 20*/
HashTable* constructor(void){
   HashTable* table;
   int i;
   table = (HashTable *) malloc(sizeof(HashTable));
   if (table == NULL) {
        return NULL;
   }
   table -> num_elements = 0;
   table -> load_factor = .75;
   table -> capacity = 20;
   table -> elements = (Item **) malloc(sizeof(Item*) * (table -> capacity));
   for(i = 0; i < table -> capacity; i++){
     table -> elements[i] = NULL;
   }
   return table;
}

int size(HashTable* table){
  return table -> num_elements;
}

int isEmpty(HashTable* table){
  if(table->num_elements == 0){
    return 1;
  }
  return 0;
}

long unsigned int hashing(HashTable* table, char *string){
  long unsigned int val;
  for(val = 0; *string != '\0'; string++){
    val = *string + 31 * val; /*make into MACRO */
  }
  return val % (table -> capacity);
}  

int get(HashTable* table, char* input_key){
  long unsigned int index;
  index = hashing(table, input_key);
  while(table -> elements[index] != NULL){
    if(!strcmp(input_key, table -> elements[index] -> key)){
      return table -> elements[index] -> value;
    }
    index++;
    index = index % (table -> capacity);
  }

  return -1;
}

void insert(HashTable* table, char* input_key, long unsigned int input_val){
  Item* input;
  long unsigned int index;
 
  index = hashing(table, input_key);
 
  while(table -> elements[index] != NULL && 
        strcmp(table -> elements[index] -> key, input_key) != 0){
    index = (index + 1) % (table -> capacity);   
  }
  
  if(table -> elements[index] == NULL){
    input = (Item*) malloc(sizeof(Item));

    input -> key = input_key;
    input -> value = input_val;
 
    table -> elements[index] = input;
    table -> num_elements = table -> num_elements + 1;
  }

  else{
    table -> elements[index] -> value = (table -> elements[index] -> value) + 1;
    free(input_key); 
  }

  if( ((double)(table -> num_elements) / (double)(table -> capacity)) 
      >= (table -> load_factor)){
      rehash(table);  
  }
}

void rehash(HashTable* table){
  unsigned long int i;
  unsigned long int num;
  unsigned long int old_size;

  Item** items;
  items = table -> elements;
  old_size = table -> num_elements;   
 
  table -> capacity = table -> capacity * 2;
  table -> elements = (Item**) malloc(sizeof(Item*) * (table -> capacity));
  num = 0;

  for(i = 0; i < table -> capacity; i++){
     table -> elements[i] = NULL;
  }
 
  table -> num_elements = 0;

  for(i = 0; i < (table -> capacity)/2; i++){
    if(items[i] != NULL){
      num ++;
      insert(table, items[i] -> key, items[i] -> value);
      free(items[i]);
    }
    if(num >= old_size){
      break;
    }
  }
  free(items);
}
