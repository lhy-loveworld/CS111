#include <stdlib.h>
#include <string.h>
#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	SortedListElement_t *tmp = (list->next == NULL) ? list : list->next;
	while ((tmp->next != NULL)&&(strcmp(tmp->key, element->key) > 0)) {
		tmp = tmp->next;
	}
	element->next = tmp->next;
	if (tmp->next != NULL) tmp->next->prev = element;
	tmp->next = element;
	element->prev = tmp; 	
}

int SortedList_delete(SortedListElement_t *element) {
	if ((element->prev == NULL) || (element->prev->next != element)) {
		return 1;
	}
	if (element->next == NULL) {
		//free(element->key);
	}
	return 0;
}