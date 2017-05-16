#include <stdlib.h>
#include <stdio.h>

#include "SortedList.h"

int main(int argc, char *argv[]) {
	SortedList_t *l = malloc(sizeof(SortedList_t) * 5);
	printf("%d, %d, %d\n", l, &(l[1]), &(l[2]));
	printf("%d, %d, %d\n", l, l + 1, l + 2);
	printf("%d\n", sizeof(SortedList_t));
	printf("%d\n", atoi("564ghc32"));
	return 0;
}