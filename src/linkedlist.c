#include "linkedlist.h"


/***
 * Create a new node
 * @param file_path
 * @param file_name
 * @param new_file_name
 * @param next node
 * @return the newly created node
 */
node* create(char* file_path, char *file_name, char *new_file_name, node* next){
    node* new_node = (node*)malloc(sizeof(node));
    if(new_node == NULL){
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->file_path = file_path;
    new_node->file_name = file_name;
    new_node->new_file_name = new_file_name;
    new_node->next = next;

    return new_node;
}

/***
 * Add a new node at the beginning of the list
 * @param linkedlist head
 * @param file_path
 * @param file_name
 * @param new_file_name
 * @return return linkedlist head
 */
node* prepend(node* head,char* file_path, char *file_name, char *new_file_name){
    node* new_node = create(file_path,file_name,new_file_name,head);
    head = new_node;
    return head;
}

/***
 * Add a new node at the end of the list
 * @param head
 * @param file_path
 * @param file_name
 * @param new_file_name
 * @return return linkedlist head
 */
node* append(node* head, char* file_path, char *file_name, char *new_file_name){

    if(head == NULL){
        return NULL;
    }

    // go to the last node
    node *cursor = head;
    while(cursor->next != NULL){
        cursor = cursor->next;
    }

    // create a new node
    node* new_node =  create(file_path, file_name, new_file_name ,NULL);
    cursor->next = new_node;

    return head;
}

/***
 * Remove node from the front of list
 * @param head
 * @return
 */
node* remove_front(node* head){
    if(head == NULL){
        return NULL;
    }
    node *front = head;
    head = head->next;
    front->next = NULL;

    // is this the last node in the list
    if(front == head){
        head = NULL;
    }
    free(front);
    return head;
}

/***
 * Remove node from the back of the list
 * @param head
 * @return
 */
node* remove_back(node* head){
    if(head == NULL){
        return NULL;
    }

    node *cursor = head;
    node *back = NULL;
    while(cursor->next != NULL){
        back = cursor;
        cursor = cursor->next;
    }

    if(back != NULL){
        back->next = NULL;
    }

    // if this is the last node in the list
    if(cursor == head){
        head = NULL;
    }

    free(cursor);

    return head;
}

/***
 * Remove a node from the list
 * @param head
 * @param nd
 * @return
 */
node* remove_any(node* head,node* nd){
    if(nd == NULL){
        return NULL;
    }

    // if the node is the first node
    if(nd == head){
        return remove_front(head);
    }

    // if the node is the last node
    if(nd->next == NULL){
        return remove_back(head);
    }

    // if the node is in the middle
    node* cursor = head;
    while(cursor != NULL){
        if(cursor->next == nd){
            break;
        }
        cursor = cursor->next;
    }

    if(cursor != NULL){
        node* tmp = cursor->next;
        cursor->next = tmp->next;
        tmp->next = NULL;
        free(tmp);
    }
    return head;
}

/***
 * Search for a specific node with input data
 * @param head
 * @param data
 * @param type => 1 = file_path,  2 = file_name, 3 = new_file_name
 * @return return the first matched node that stores the input data, otherwise return NULL
 */
node* search(node* head, char *data, int type){
    node *cursor = head;
    while(cursor != NULL){

    	// return path
    	if(type == 1){
			if(strcmp(cursor->file_path, data) == 0){
				return cursor;
			}
    	}else if(type == 2){ //return name
    		if(strcmp(cursor->file_name, data) == 0){
				return cursor;
			}
    	}else if(type == 3){//return new name
    		if(strcmp(cursor->new_file_name, data) == 0){
				return cursor;
			}
    	}
    	cursor = cursor->next;
    }
    return NULL;
}

/***
 * Remove all element of the list
 * @param head
 */
void dispose(node *head){
    node *cursor, *tmp;

    if(head != NULL){
        cursor = head->next;
        head->next = NULL;
        while(cursor != NULL){
            tmp = cursor->next;
            free(cursor);
            cursor = tmp;
        }
    }
}

/***
 * Return the number of elements in the list
 * @param head
 * @return
 */
int count(node *head){
    node *cursor = head;
    int c = 0;
    while(cursor != NULL){
        c++;
        cursor = cursor->next;
    }
    return c;
}
