/* My name is Yashwant Sathish Kumar and
 * this is my htable.*/

#include <stdio.h>
#include <stdlib.h>
#include "htable.h"
#include <string.h>

#define VALID_BYTES 256

/* main reads in the file as a param & does all of the error checks 
 * with it. Then, I create an array of size 256 (1 for each valid char), 
 * and for every character read in, I index its value in the array and
 * add 1, which serves as a way to track the frequency. Then for each
 * char, I create a Byte struct and add it to a linked list ordering 
 * by frequency and ASCII values. Then, as long as more than 1 char has 
 * been read in, I build a binary tree with those chars. Now, I need the
 * encoding, or "path", for each char, so I call tree_to_array, which finds
 * all of the leaf nodes from the tree, which are the inputted chars, 
 * and inputs the encoded path. It's stored into its corresponding index
 * in an array of ASCII values. Finally, I print out the chars and their 
 * path from this array. */
/*int main(int argc, char *argv[]){
   
    int count = 0;
    int i;
    FILE* file;
    int c;
    int num_elements = 0;
    Byte* head = NULL;
    Byte* input;
    int array[VALID_BYTES] = {0};
    char *paths[VALID_BYTES] = {0};
    
    if(argc == 0){
        return 0;
    }

    if( (file = fopen(argv[1], "r")) == NULL){
        printf("%s: No such file or directory\n", argv[1]);
        return 0;
    }  
  
    while((c = getc(file)) != EOF){
        array[c] += 1;
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
       return 0;
   }
  
   if(num_elements == 1){
       printf("0x%02x: \n", (head->ch));
       return 0; 
   }
   head = build_tree(head, num_elements);      
   
   for(i = 0; i < VALID_BYTES; i++){
       paths[i] = NULL;
   }

   tree_to_array(head, paths, "", 1);
   
   for(i = 0; i < VALID_BYTES; i++){
       if(count > num_elements){
           break;
       }
       if(paths[i] != NULL){
           count++;
           printf("0x%02x: %s\n", i, paths[i]);
           printf(%s, &paths
           free(paths[i]); 
       }
   }
   my_free_tree(head);
//   free(input);
   fclose(file);
   return 0;
}
*/
/*add takes in a linked list (through it's head val) as well
 * as the Byte* input we want to add to it. First, we compare with
 * the head, adding the element to the front of the list if the
 * head doesn't exist, head has a higher frequency than the input
 * element, or if when both frequencies are the same, and if head's char
 * is further up in the alphabet than the input. We return the head of 
 * the list, which is input, in this case. Else, we set current to
 * head and we traverse through the list, comparing the input frequency
 * with the current's next. We traverse through the list as long as
 * we aren't at the end of the list and if current's next's frequency
 * is less than the input or if both are the same, if current's next's
 * char is less than the input's char. When either of these conditions 
 * fails, we add after current's next. If we get to the end, we add 
 * input to the end of the list. If we get to a current -> next in the
 * list that either has a greater frequency than input or has the
 * same frequency but a higher char ASCII, we add input after current
 * (right before current -> next). This also takes care of collision
 * resolution when I'm re-inserting when creating my binary tree, as
 * that only inserts based on frequency (all chars are -1 and the same).*/
Byte* add(Byte* head, Byte* input){
    Byte* current;
    if(head == NULL || (head -> freq > input -> freq) ||
           (head -> freq == input -> freq && head -> ch > input -> ch)){
        input -> next = head;
        head = input;
        return head;
    }
    else{
        current = head;
        while(current -> next != NULL && 
             (current -> next -> freq < input -> freq ||
              ((current -> next -> freq == input -> freq) &&
                (current -> next -> ch < input -> ch)) ) ){
           current = current -> next;
        }
        input -> next = current -> next;
        current -> next = input;
    } 
    return head;  
}

/*Build tree removes the first 2 elements of the linked list and creates
 * a new Byte with those 2 elements as its children and sets
 * its frequency to the sum of the 2 frequencies, then reinserts
 * this new Byte into the list. This process cycles through until
 * there is 1 element left in the linked list. The head of the list
 * turned tree is returned. */
Byte* build_tree(Byte* head, int num_elements){
   Byte* new_node;
   Byte* temp_1;
   Byte* temp_2;
   Byte* node_temp;

   while((num_elements) > 1){
       new_node = (Byte *) malloc(sizeof(Byte));
      
       if(new_node == NULL){
           perror("malloc");
           exit(EXIT_FAILURE);
        }
       
       temp_1 = head;
       temp_2 = head -> next;
        
       node_temp = head;
       head = head -> next -> next; 
       node_temp -> next -> next = NULL;
       
       num_elements = num_elements - 2;
       
       new_node -> ch = -1;
       new_node -> freq = temp_1 -> freq + temp_2 -> freq;
       new_node -> left = temp_1;
       new_node -> right = temp_2;
       new_node -> next = NULL;

       head = add(head, new_node); 
       num_elements = num_elements + 1;
   }
   //free(temp_1);
   //free(temp_2);
   //free(node_temp);
   //free(new_node);
   return head;
}

 
/* Traverses tree for leaf nodes (inputted chars) nodes of tree and puts
 * into array along with the encoded path. A '0' is added to a string
 * when traversing left and '1' while traversing right. When 
 * backtracking, we remove the last char from the string. When a 
 * leaf node is found, it adds a null character. */

void tree_to_array(Byte* current, char *array[], char* str, int str_len){
// malloc space for str and for array element
    if(current -> left == NULL && current -> right == NULL){
        char *copy = newstr(str, str_len);
        array[(current -> ch)] = copy;
        return;
    }
   
    str_len++;
    
    char temp[str_len];
    strcpy(temp, str);
    char toLeft[str_len];
    temp[str_len-2] = '0';
    temp[str_len-1] = '\0';
    strcpy(toLeft, temp);
    tree_to_array(current -> left, array, toLeft, str_len);

    char toRight[str_len];
    temp[str_len-2] = '1';
    strcpy(toRight, temp);	
    tree_to_array(current -> right, array, toRight, str_len); 
   
    return;
}
 
/*returns a new string copying a given string. used
 * in tree_to_array when adding path for each char. */
char* newstr(char* str, int str_len){
    char* string;
    string = (char*) malloc(sizeof(char) * str_len);
    if(string == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
   }    
    strcpy(string, str);
    return string;   
}

/*Traverses through the tree and frees each element.*/
void my_free_tree(Byte* current){
    if(current == NULL){
        return;
    }
    
    my_free_tree(current -> left);
    my_free_tree(current -> right);
    free(current);
} 
