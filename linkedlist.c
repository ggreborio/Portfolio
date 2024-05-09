#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

/* 
 * adds a new node with a given (key, value) in the beginning of the linked list. If malloc fails, print the error message and terminate the program with return code -1. 
 * @param: head is the pointer to the first node in the linked list
 * @param: key is a string in the (key, value) pair
 * @param: value is a string in the (key, value) pair
 * @return: a pointer to the head of the updated linked list
 */
node* addNode(node* head, char* key, char* value) {
		//malloc and error handle
    node* newN = (node*)malloc(sizeof(node));
    if (newN == NULL){
        printf("Malloc for addNode() didn't work.\n");
        return NULL;
    }
    
    //strdup duplicates string over using malloc
    newN->key = key;
    if (newN->key == NULL) {
        printf("Memory allocation failed.\n");
        free(newN); //need free() because of malloc
        return NULL;
    }

    newN->value = value;
    if (newN->value == NULL) {
        printf("Memory allocation failed.\n");
        free(newN->key);
        free(newN);
        return NULL;
    }

	
    newN->next = head; //link newN to the head

    return newN; //newN == head

}

/* 
 * checks if there is a node in the linked list with the given key
 * @param: head is the pointer to the first node in the linked list
 * @param: key is a string in the (key, value) pair
 * @return: value if the key is found, NULL otherwise
 */
char* getNode(node* head, char* key){
    node* current = head;

    while (current != NULL) { //iterate through list
    	if (strcmp(current->key, key) == 0) {
    		return current->value; //if key matches, return its value
    	}
    	current = current->next; //increments through list
    }
    return 0;
}

/* 
 * looks for a node with the given key, and if found, remove the node and return the corresponding value.
 * @param: head is the pointer to the first node in the linked list
 * @param: key is a string in the (key, value) pair
 * @param: num is a pointer to an int variable to store the result. num should store 1 if a node was deleted, 0 otherwise. 
 * @return: a pointer to the head of the updated linked list
 */
node* deleteNode(node* head, char* key, int* num) {
	//error handle empty list
    if (head == NULL) {
    	printf("Head is null.");
    	*num = 0;
    	return NULL;;
    }

    node* current = head; //rename
    node* previous = 0; //for unlinking, current->previous

	//iterating through list until one key matches OR current->next is null
	while (current->next != NULL && strcmp(current->key, key) != 0) {
		previous = current;
		current = current->next;
	}
	
	if (current == NULL) { //reached end of list
		printf("Node not found.\n");
		*num = 0; //pointer because var is initialized in map.c
		return head;
	}

	if (previous == NULL) { //while loop exited at head
		head = current->next;
		*num = 1;
	} else {
		previous->next = current->next; //current node met conditions
		*num = 1;
	}
	
	
	free(current->value); // free values first
	free(current->key);
	free(current); 
	
	return head;
}

void printList(node* head) {
    
	// error handle node
    if (head == 0) {
    	printf("Head is null.");
    	return;
    }

	node* current = head;

	printf("["); //opening bracket
	
    while (current != NULL) {
    	printf("[%s,%s]", current->key, current->value);
    	current = current->next;
    	if (current != NULL) {
    		printf(",");
    	}
    }
	
    printf("]\n"); // closing bracket

} 
