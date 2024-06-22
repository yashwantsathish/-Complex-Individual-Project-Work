/* My name is Yashwant Sathish Kumar and this is hdecode.*/

#include <stdio.h>
#include <stdlib.h>
#include "htable.h"
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#define VALID_BYTES 256
#define INPUT_ARG 1
#define OUTPUT_ARG 2
#define MAX_BIT_SHIFT 7
#define FREQ_SIZE 4

#define STDIN 0
#define STDOUT 1

int i = 0;
int num_uniq = 0;
int fd1;
int fd2;
int array[VALID_BYTES] = {0};
unsigned long int num_elements = 0;
Byte* head = NULL;
Byte* input;
char *paths[VALID_BYTES] = {0};

/*main reads in the input and output file as parameters, 
 * reads in the number of unique elements, calls the function
 * to build the tree, then traverses the tree to output the
 * leaf nodes.*/
int main(int argc, char *argv[]){
    int r_val = 0; 
    if(argc == 1){
        fd1 = STDIN; //stdin
        fd2 = STDOUT; //stdout   
    }

    if(argc == 2){
        if((fd1 = open(argv[INPUT_ARG], O_RDONLY, S_IRUSR | S_IWUSR)) < 0){
             //printf("%s: No such file or directory\n", argv[INPUT_ARG]);
            perror("open");
            exit(EXIT_FAILURE);
             //return 0;
        }
        fd2 = STDOUT;
    }

    if(argc == 3){
        if(*(argv[INPUT_ARG]) == '-'){
            fd1 = STDIN;
        }
     
        else if((fd1 = open(argv[INPUT_ARG], O_RDONLY)) < 0){
            //printf("%s: No such file or directory\n", argv[INPUT_ARG]);
            perror("open");
            exit(EXIT_FAILURE);
            //return 0;
        }   
      
        else if((fd2 = open(argv[OUTPUT_ARG], O_RDWR | O_TRUNC | O_CREAT,
                S_IRUSR | S_IWUSR)) < 0){
            //printf("%s: No such file or directory\n", argv[OUTPUT_ARG]);
            perror("open");
            exit(EXIT_FAILURE);
            //return 0;
        }
        
    }

    else{
        printf("Usage: hdecode [ ( infile | - ) [ outfile ] ]");
        return 0;
    }

    if((r_val = read(fd1, &num_uniq, 1)) < 0){
        perror("read");
        exit(EXIT_FAILURE);
    }      
   
    num_uniq += 1;
    buildTree();    
   
    if(num_elements == 0){
        return 0;
    }

    int total_freq = head -> freq;
    int count = 0;
    uint8_t read_val = 0;
    Byte* current = head;
    int mask = 1;
    int index = MAX_BIT_SHIFT;

    while(count < total_freq){
        index = MAX_BIT_SHIFT;
        
        if((r_val = read(fd1, &read_val, 1)) < 0){
            perror("read");
            exit(EXIT_FAILURE);
        }
        while(index >= 0 && count < total_freq){
            if(current -> left == NULL && current -> right == NULL){
                int val = current -> ch;
                if((write(fd2, &val, 1)) < 0){
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                count++;
                current = head;
                continue;
                //}
            }
            else if((read_val & (mask << index)) == 0){
                current = current -> left;     
            } 
            else if((read_val & (mask << index)) > 0){
                current = current -> right;
            }
            index -= 1;
        }
    }
   
    for(i = 0; i < VALID_BYTES; i++){
       if(count > num_elements){
           break;
       }
       if(paths[i] != NULL){
           count++;
           free(paths[i]);
       }
    }
    my_free_tree(head);
    
    close(fd1);
    close(fd2);
    return 0; 
 
}

/*buildTree builds the tree, reading in the
 * char and corresponding frequency */
void buildTree(){
    int count = 1;
    uint8_t ch = 0;
    uint32_t freq = 0;
    int i = 0;

    while(count <= num_uniq){
        if((read(fd1, &ch, 1)) < 0){
            perror("read");
            exit(EXIT_FAILURE);
        }
    
        if((read(fd1, &freq, FREQ_SIZE)) < 0){
            perror("read");
            exit(EXIT_FAILURE);
        }

        freq = ntohl(freq);
        array[ch] = freq;
        count++;
    }
 
    for (i = 0; i < VALID_BYTES; i++){
        if(array[i] != 0){
            input = (Byte*) malloc(sizeof(Byte));
            if(input == NULL){
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            input -> ch = i;
            input -> freq = array[i];
            input -> left = NULL;
            input -> right = NULL; 
            head = add(head, input);
            num_elements++;
        }
    }
   
    if(num_elements == 0){
         return;
    } 
  
    head = build_tree(head, num_elements);      
   
    for(i = 0; i < VALID_BYTES; i++){
        paths[i] = NULL;
    } 

    tree_to_array(head, paths, "", 1);

}   


