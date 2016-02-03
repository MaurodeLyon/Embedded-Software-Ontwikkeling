#include <stdio.h>
void main()
{
	float fahr, celsius;
	int lower, upper, step;

	lower = 0;
	upper = 300;
	step = 20;

	celsius = upper;

	printf("%s\t%s\n", "celsius", "fahr");
	while (celsius >= lower)
	{
		fahr = celsius * 9 / 5 + 32;
		printf("%3.0f\t%6.1f\n", celsius, fahr);
		celsius = celsius - step;
	}
}