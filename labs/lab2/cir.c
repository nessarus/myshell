/* This program calculate the area and circumference
 * of a circle using PI as a defined constant.
 * 	Written by: Joshua Ng
 * 		Date: 13/8/2017
 */

#include <stdio.h>
#define PI 3.1416

int main(void)
{
/*	Local Declarations */
	float circ;
	float area;
	float radius;

/*	Statements */
	printf("\nPlease enter the value of the radius: ");
	scanf("%f", &radius);

	circ	= 2 * PI	* radius;
	area	= PI * radius	* radius;

	printf("\Radius is :		%10.2f", radius);
	printf("\nCircumference is :	%10.2f", circ);
	printf("\nArea is :		%10.2f\n", area);

	return 0;
}	//main
