
#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct node{
	char *file_path;
	char *file_name;
	char *new_file_name;
    struct node* next;
} node;

node* create(char* file_path, char *file_name, char *new_file_name, node* next);
node* prepend(node* head,char* file_path, char *file_name, char *new_file_name);
node* append(node* head, char* file_path, char *file_name, char *new_file_name);
node* remove_front(node* head);
node* remove_back(node* head);
node* remove_any(node* head,node* nd);
node* search(node* head, char *data, int type);
void dispose(node *head);
int count(node *head);


#endif /* LINKEDLIST_H_ */
