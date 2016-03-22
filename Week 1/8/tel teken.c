#include <stdio.h>

/* count lines,space and tabs in input*/
void main()
{
	int c, newlines,spaties,tabs;
	newlines = 0;
	spaties = 0;
	tabs = 0;
	while ((c = getchar()) != EOF) {
		if (c == '\n') ++newlines;
		if (c == ' ') ++spaties;
		if (c == '\t') ++tabs;
	}
	printf("newlines:%d\tspaties:%d\ttabs:%d\n", newlines, spaties, tabs);

}