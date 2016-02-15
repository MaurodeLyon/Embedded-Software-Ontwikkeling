#include <string.h>
#include <stdio.h>


void main()
{
	float fahr, celsius;
	int lower, upper, step;

	lower = -20.0;
	upper = 100.0;
	step = 10.0;

	celsius = lower;
	printf("%s    %s\n", "Celsius", "Fahrenheit");
	while (celsius <= upper)
	{
		fahr = (celsius * 9.0) / 5.0 + 32.0;
		printf("%10.0f %10.0f\n", celsius, fahr);
		celsius = celsius + step;
	}



}