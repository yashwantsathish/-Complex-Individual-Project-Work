/* My name is Yashwant Sathishkumar and this is hencode.*/

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

#define STDIN 0
#define STDOUT 1
#define MAX_BUFFER_SIZE 4096
#define FREQ_SIZE 4


int array[VALID_BYTES] = {0};
int fd1, fd2;
char *paths[VALID_BYTES];
uint8_t bytes[MAX_BUFFER_SIZE] = {0};
int num_written = 0;

/*main reads in the input/output files + 
 * takes care of error cases, builds the
 * tree with given chars from input file, writes the 
 * number of unique elements, then calls helper methods
 * to write the header segment to the output and the body 
 * segment.*/
int main(int argc, char *argv[]){
    int count = 0;
    uint8_t c = 0;
    Byte* input;
    int i;
    int r_val;
    unsigned long int num_elements = 0;
    uint8_t num_uniq = 0;
    Byte* head = NULL;  
    
    if(argc < 2){
        printf("Usage: hencode infile [ outfile ]");
        return 0;
    }
    
    if((fd1 = open(argv[INPUT_ARG], O_RDONLY, S_IRUSR | S_IWUSR)) < 0){
        //printf("%s: No such file or directory\n", argv[INPUT_ARG]);
        perror("open");
        exit(EXIT_FAILURE);
        //return 0;
    } 

    if(argc == 2){
        fd2 = STDOUT; 
    } 
  
    if(argc == 3){
        if((fd2 = open(argv[OUTPUT_ARG], O_RDWR | O_TRUNC | O_CREAT, 
            S_IRUSR | S_IWUSR)) < 0){
            //printf("%s: No such file or directory\n", argv[OUTPUT_ARG]);
            perror("open");
            exit(EXIT_FAILURE);
            return 0;
        }
    }
    
    while((r_val = read(fd1, &c,  1)) > 0){
        array[c] += 1;
    }  
    
    if(r_val < 0){
       perror("read");
       exit(EXIT_FAILURE);
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
        count++;
        return 0;
    }

    head = build_tree(head, num_elements);  
   
    for(i = 0; i < VALID_BYTES; i++){
        paths[i] = NULL;
    }

    tree_to_array(head, paths, "", 1); 
    
    num_uniq = num_elements - 1;
    if((write(fd2, &num_uniq, 1)) < 0){
        perror("write");
        exit(EXIT_FAILURE);
    }
    header();
    
    if((lseek(fd1, 0, SEEK_SET)) == -1){
        perror("lseek");
        exit(EXIT_FAILURE);
    }
   
    if(num_uniq > 0){ 
        add_body();
        write_body();
    }
    
    for(i = 0; i < VALID_BYTES; i++){
       if(count > num_elements){
           break;
       }
       if(paths[i] != NULL){
           count++;
           //printf("0x%02x: %s\n", i, paths[i]);
           free(paths[i]); 
       }
    }
    
    my_free_tree(head); 
    
    close(fd1);
    close(fd2);
    return 0;
}

/*writes the header segment of the file. While a letter
 * exists within the frequency array, it writes it and
 * its corresponding frequency.*/
void header(){
    int i = 0;
    char letter;
    uint32_t frequency;
    for(i = 0; i < VALID_BYTES; i++){
        if(array[i]){
            letter = (char) i;    
            if(write(fd2, &letter, 1) < 0){
                perror("write");
                exit(EXIT_FAILURE);
            }
            frequency = htonl((uint32_t)array[i]);
            if((write(fd2, &(frequency), FREQ_SIZE)) < 0){
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
    }

    return;
}

/*adds the encoded paths of the body to the buffer 
 * which will then be written to the output. If the
 * buffer fills up, write the buffer to fd2 then 
 * continue reading into the beginning of the buffer. */
void add_body(){
    uint32_t each = 0;
    int index = MAX_BIT_SHIFT;
    char* path;
    int mask = 1;
    int r_val = 0;
    
    while((r_val = read(fd1, &each, 1)) > 0){
        path = paths[each];
        while(*path != '\0'){
            if(index < 0){
                num_written ++;
                index = MAX_BIT_SHIFT;
            }
            if(num_written == MAX_BUFFER_SIZE){
                index = MAX_BIT_SHIFT;
                write_body();
            }
            if(*path == '0'){
                index -= 1;                
            }
            else if(*path == '1'){
                bytes[num_written] |= (mask << index);
                index -= 1;
            }
            path ++;
        }
        
    }
   
    while(index >= 0 && index < MAX_BIT_SHIFT){
        bytes[num_written] &= ~(1UL << index);
        index -= 1;
    } 
    
    if(r_val < 0){
       perror("read");
       exit(EXIT_FAILURE);
    }   
    return;
}

/*writes contents of bytes array when full and preps to
 * rewrite the array with remaining contents of file
 * by setting num_written to 0*/
void write_body(){
    if(num_written < MAX_BUFFER_SIZE){
        num_written += 1;
    }
    if(write(fd2, bytes, num_written) < 0){
        perror("write");
        exit(EXIT_FAILURE);
    }
    num_written = 0;
    memset(bytes, 0, sizeof bytes);
}
