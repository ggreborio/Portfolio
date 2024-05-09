#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sha256.h"
#include "map.h"

#define DIG_BIN_LEN 32
#define DIG_STR_LEN ((DIG_BIN_LEN * 2) + 1)

void sha256(char *dest, char *src) {
    // zero out the sha256 context
    struct sha256_ctx ctx;
    memset(&ctx, 0, sizeof(ctx));

    // zero out the binary version of the hash digest
    unsigned char dig_bin[DIG_BIN_LEN];
    memset(dig_bin, 0, DIG_BIN_LEN);

    // zero out the string version of the hash digest
    memset(dest, 0, DIG_STR_LEN);

    // compute the binary hash digest
    __sha256_init_ctx(&ctx);
    __sha256_process_bytes(src, strlen(src), &ctx);
    __sha256_finish_ctx(&ctx, dig_bin);

    // convert it into a string of hexadecimal digits
    for (int i = 0; i < DIG_BIN_LEN; i++) {
        snprintf(dest, 3, "%02x", dig_bin[i]);
        dest += 2;
    }
}
/*
 * takes a key, hashes it, and compute the index to the internal array in map
 * @param: key is a string in the (key, value) pair
 * @param: length is the size of the internal array in map
 * @return: index of an array for this key
 */
int mapIndex(char* key, int length) {
	//make a string with the standard hash length
    char hash[DIG_STR_LEN];
    
    //key -> hash
    sha256(hash, key);

    //find log16 of length
    int bytes = ceil(log(length)/log(16));
    //add terminating character
    char hex[bytes + 1];
    hex[bytes] = '\0';
    //?????????????????
    memcpy(hex, hash + strlen(hash) - bytes, bytes);
    //convert hexadecimal to decimal
    long hash_value = strtol(hex, NULL, 16);

    //modulo of hash (remainder = index)
    int index = hash_value % length; //hash value: placement in the map
    
    return index;
}

/* 
 * adds a new node with a given (key, value) in the beginning of the linked list. increase the count in the map.
 * @param: m is a pointer to the map
 * @param: key is a string in the (key, value) pair
 * @param: value is a string in the (key, value) pair
 */
void add(map* m, char* key, char* value) {
		//find index
    int index = mapIndex(key, m->size);

	//call addNode
    m->list[index] = addNode(m->list[index], key, value);

	//error handle and add count
	if (m->list[index] != NULL) {
		m->count = m->count + 1;
	} else {
		printf("Node was not successfully added.\n");
	}

//_______________________________//

	//check load
    float load = (float)m->count / m->size;
    if (load > m->LF_high_thres) {
        // Resize the map
        int newSize = m->size * 2;
        
	    node** newList = (node**)realloc(m->list, newSize * sizeof(node*));
	    if (newList == NULL) {
	    	printf("Memory reallocation failed.\n");
	        return;
	    }

	    //set new half of list to NULL
	    memset(newList + m->size, 0, (m->size * sizeof(node*)) / 2);
		m->list = newList;
	
		//iterate through map    
        for (int i = 0; i < m->size; i++) {
            node* current = m->list[i];
            node* prev = NULL;
            while (current != NULL) {
                //rehash
                int newIndex = mapIndex(current->key, newSize);

                //move node to newIndex (if necessary)
                if (i != newIndex) {
                    if (prev != NULL) {
                        prev->next = current->next; //link prev and next node
                    } else if (current->next != NULL) {
                        m->list[i] = current->next; //make next node head
                    } else {
                    	m->list[i] = NULL; //make empty list NULL
                    }
                    
                    current->next = m->list[newIndex];
                    m->list[newIndex] = current;
                    current = prev ? prev->next : m->list[i];
                } else {
                    prev = current;
                    current = current->next;
                }
            }
        }
        m->size = newSize;
	} 
    
}

/* 
 * find a corresponding value for the given key
 * @param: m is a pointer to the map
 * @param: key is a string in the (key, value) pair
 * @return: value if the key is found, NULL otherwise
 */
char* get(map* m, char* key){
    //find index
    int index = mapIndex(key, m->size);

    //use index to get node
    char* result = getNode(m->list[index], key);

    //
    return result;                  
}


/* 
 * looks for a node with the given key, and if found, remove the (key, value) pair in the map
 * @param: m is a pointer to the map
 * @param: key is a string in the (key, value) pair
 */
void delete(map* m, char* key) {
    // find index
    int index = mapIndex(key, m->size);

    int indicator = 2; //initialized to something other than 1 and 0

    //use index to delete
    m->list[index] = deleteNode(m->list[index], key, &indicator);

	//error handle and subtract count
	if (indicator == 2) {
		printf("Indicator was not updated.\n");
	} else if (indicator == 1) {
        m->count = m->count - 1;
    }

    //_______________________________//
	//check load
	float load = (float)m->count/m->size;
	if (load <= m->LF_low_thres){
		int newSize = m->size / 2;
		// link the beginning of the lists from the second half to the end of the lists from the first half
        for (int i = m->size / 2; i < m->size; i++) {
    		node* list2 = m->list[i];
    		if (list2 != NULL) {
		        //rehash first node to move list
		        int newIndex = mapIndex(list2->key, newSize);

		        //if new index is empty, put list there
		        if (m->list[newIndex] == NULL) {
		            m->list[newIndex] = list2;
		        } else { //if list not empty, go to end of list1 and attach list2
		            node* list1 = m->list[newIndex];
		            while (list1->next != NULL) {
		                list1 = list1->next;
		            }
		            list1->next = list2; //attach list2
		        }

		        //set old index to NULL
		        m->list[i] = NULL;
		    }
		}
        // delete elements from the second half of the map
        for (int i = newSize; i < m->size; i++) {
            node* current = m->list[i];
            while (current != NULL) {
                node* next = current->next;
                free(current);
                current = next;
            }
            m->list[i] = NULL;
        }

        // update map count
        m->count -= 1;

        // reallocate memory to reduce size
        node** newList = (node**)realloc(m->list, newSize * sizeof(node*));
        if (newList == NULL) {
            printf("Memory reallocation failed.\n");
            return;
        }
        m->list = newList;
        m->size = newSize;
	}
	
}

/*
 * prints the content of the internal array of linked lists
 * @param: m is a pointer to the map
 */
void printMap(map m)
{
    for (int i = 0; i < m.size; i++)
    {
        if (m.list[i] != NULL)
        {
            printf("%d, ", i);
            printList(m.list[i]);
        }
    }
}
