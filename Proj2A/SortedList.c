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