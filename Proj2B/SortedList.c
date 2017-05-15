#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include <errno.h>
#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	SortedListElement_t *tmp = list;
	while ((tmp->next != NULL)&&(strcmp(tmp->next->key, element->key) > 0)) {
		if (tmp->next->prev != tmp) {
			fprintf(stderr, "corrupted list\n");
			exit(2);
		}
		tmp = tmp->next;
	}
	if (opt_yield & INSERT_YIELD) {
		sched_yield();
	}
	element->next = tmp->next;
	if (tmp->next != NULL) tmp->next->prev = element;
	tmp->next = element;
	element->prev = tmp;
}

int SortedList_delete(SortedListElement_t *element) {
	//if (element == NULL) {printf("debug4");return 1;}
	if ((element->prev == NULL) || (element->prev->next != element)) {
		//printf("debug5");
		return 1;
	}
	if (element->next == NULL) {
		if (opt_yield & DELETE_YIELD) {
			sched_yield();
		}
		element->prev->next = NULL;
		//free(element);
	} else {
		if (element->next->prev != element) {
			//printf("debug6");
			return 1;
		} else {
			if (opt_yield & DELETE_YIELD) {
				sched_yield();
			}
			element->prev->next = element->next;
			element->next->prev = element->prev;
			//free(element);
		}
	}
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
	SortedListElement_t *tmp = list->next;
	while (tmp != NULL) {
		if (tmp != tmp->prev->next) {//printf("debug1");
			fprintf(stderr, "corrupted list\n");
			exit(2);
		}
		if (strcmp(tmp->key, key) > 0) {
			if (opt_yield & LOOKUP_YIELD) {
				sched_yield();
			}
			tmp = tmp->next;
		} else {
			if (!strcmp(tmp->key, key)) {
				return tmp;
			} else {
				//printf("debug2");
				return NULL;
			}
		}
	}
	//printf("debug3");
	return NULL;
}

int SortedList_length(SortedList_t *list) {
	int len = 0;
	SortedListElement_t *tmp = list->next;
	while (tmp != NULL) {
		len++;
		//printf("%s\n", tmp->key);
		tmp = tmp->next;
	}
	return len;
}