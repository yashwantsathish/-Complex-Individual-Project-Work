#ifndef HTABLE_H
#define HTABLE_H

typedef struct Byte Byte;

struct Byte{
    int ch;
    int freq;
    Byte* next;
    Byte* left;
    Byte* right;
};

Byte* insert(Byte* head, Byte* input);

Byte* build_tree(Byte* head, int num_elements);

void tree_to_array(Byte* current, char *array[], char* str, int str_len);

char* newstr(char* str, int str_len);

Byte* add(Byte* head, Byte* input);

void my_free_tree(Byte* head);

void header();

void add_body();

void write_body();
  
void readheader();

void buildTree();
#endif          
