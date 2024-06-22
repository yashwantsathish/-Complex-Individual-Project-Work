#include "hashtable.h"
#include <stdio.h>
#include <ctype.h>
#include <unistd.h> 
#include <getopt.h>
#include <stdlib.h>

/*This is Yashwant Sathish Kumar and this is my fw.c
 * Main receives the user input, determines based on 
 * the arguments passed what to do. What comes after n
 * is the amount of words.*/
int main(int argc, char *argv[]){
  unsigned long int i;
  FILE* file; 
  char* c;
  int k; /* number of words needed to display */
  Item** array;
  HashTable* table;
  int opt; 
  k = 0;

  table = constructor();
 
  while ((opt = getopt(argc, argv, "n:")) != -1) {
    switch (opt) {
      case 'n':
        if(optind == 1){
          printf("%d", optind); 
          fprintf(stdout, "Usage: [-t nsecs] [-n] name\n");
        }
        for(i = 0; optarg[i] !=0; i++){
          if(optarg[i] >= 48 && optarg[i] <= 57){
            k = k * 10 + optarg[i] - 48;
          }
          else{
            fprintf(stdout, "usage: fw [-n num] [ file1 [ file 2 ...] ]\n");
            exit(EXIT_FAILURE);
          }
        }
        break;
      default:
        printf("Usage: %s [-t nsecs] [-n] name\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
    }
  }
  
  if(k == 0){
    k = 10; 
  }

  if(optind >= argc){
    while((c = my_get_word(stdin))){
     insert(table, c, 1);
    }   
  }

  else{ 
    while(optind < argc){
      if( (file = fopen(argv[optind], "r")) != NULL ){
         while((c = my_get_word(file))){
           insert(table, c, 1);
         } 
      }
      else{
       /* printf("%s: No such file or directory\n", argv[optind]);  */
      }
      optind++;
   }
  }

  printf("The top %d words (out of %lu) are:\n", k, table -> num_elements);

  if(table -> num_elements > 0){
    array = create_array(table);
    qsort(array, table -> num_elements, sizeof(Item *), compare);
  
  
    for(i = 0; i < k; i++){
      if(i >= (table -> num_elements)){
        break;
      }
      printf("%9d %s\n", (int) array[i] -> value, array[i] -> key); 
    }
  }
  /*for(i = 0; i < table -> capacity; i++){
     if(table -> elements[i] != NULL){
       if(table -> elements[i] -> key != NULL){
        free(table -> elements[i] -> key);
       }
       free(table -> elements[i]);
     }
   }
  free(table -> elements);
  if (table != NULL) {
    free(array);
  }
  free(table);*/
  return 0;
}

Item** create_array(HashTable *table){
  unsigned long int num;
  unsigned long int i;
  Item** array;
  num = 0; 
  array = NULL;

  for(i = 0; i < table -> capacity; i++){
    if(table -> elements[i] != NULL){
      num++;
      if(num == 1){
        array = (Item**) malloc(sizeof(Item*));
      }
      else{
        array = (Item **) realloc(array, sizeof(Item*) * (num));
      }
      
      array[num-1] = table -> elements[i];
      
      if(num >= table -> num_elements){
        break;
      } 
    }
  }
   
  return array;
}

int compare(const void * a, const void * b){ 
  const Item* item_1 = *(const Item **)a;
  const Item* item_2 = *(const Item **)b;
  if((item_1 -> value) < (item_2 -> value)){
    return 1;
  }
  else if((item_1 -> value) > (item_2 -> value)){
    return -1;
  } 
  else{
    if(strcmp(item_1 -> key, item_2 -> key) < 0){
      return 1;
    }
    else{
      return -1;
    }
  }
}

/* This reads the char from the given
 * file input and returns it as a word.*/
char* my_get_word(FILE *file){  
  char* ret;
  int c;
  unsigned long int len;

  len = 0;
  ret = NULL;
 
  while((isalpha(c = fgetc(file)) != 0 || len ==0) && feof(file) == 0){  
    if(isalpha(c) != 0){
      c = tolower(c);
      if(len == 0){
        ret = (char *) malloc(sizeof(char));
      }
      else{ 
        ret = (char *) realloc(ret, sizeof(char) * (len + 1));
      }
      ret[len] = c;
      len++;
    }
  }
  if(ret != NULL){ 
    ret = (char *) realloc(ret, sizeof(char) * (len + 1));
    ret[len] = '\0';
  }
  return ret;
}  
