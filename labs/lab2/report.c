/* This program will print four lnes of inventory data
 * on an inventory report to give the suer an idea of
 * what a new report will look like. Since this is not
 * a real report, no input is required. The data are
 * all specified as constants
 * 	Written by:
 * 		Date: 13/8/2017
 */
#include <stdio.h>

int main (void)
{
/*	Statements */

	/* Print captions */
	printf("\tPart Number\tQty On Had");
	printf("\tQty On Order\tPrice\n");

	/* Print data */
	printf("\t %06d\t\t%7d\t\t%7d\t\t $%7.2f\n",
		31235, 22, 86,45.62);
	printf("\t %06d\t\t%7d\t\t%7d\t\t $%7.2f\n",
		321, 55, 21, 122.);
	printf("\t %06d\t\t%7d\t\t%7d\t\t $%7.2f\n",
		28764, 0, 24, .75);
	printf("\t %06d\t\t%7d\t\t%7d\t\t $%7.2f\n",
		3232, 12, 0, 10.91);

	/* Print end message */	
	printf("\n\tEnd of Report\n");
	return 0;
}	// main
