/* Print attributes of a complex number.
 * 	Written by:
 * 		Date: 13/8/2017
 */
#include <stdio.h>
#include <math.h>
#include <complex.h>

int main (void)
{
/*	Local Declarations */
	double complex x = 4 + 4 * I;
	double complex xc;

/*	Statements */
	xc = conj (x);
	printf("%f %f %f %f\n",	creal(x), cimag(x),
				cabs(x),  carg(x));

	printf("%f %f %f %f\n",	creal(xc), cimag(xc),
				cabs(xc),  carg(xc));

	return 0;
}	// main
