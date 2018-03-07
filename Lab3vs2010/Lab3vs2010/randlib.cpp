//**************************************
//  File:      random.c
//  Author:    Yon Visell, except
//             from nr
//**************************************
// (File renamed to avoid conflict with <random>)

#include "randlib.h"
#include <math.h>

//------------------------------------ ran1

// From NR:
// "Minimal" random number generator of Park and Miller, with Bays-Durham
// shuffle and added safeguards.  Returns a uniform deviate between 0.0 and
// 1.0 (exclusive of the endpoint values).  Call with idum a negative int
// to initialize;  thereafter, do not alter idum between successive deviates
// in a sequence.  RNMX should approximate the largest doubleing point
// value that is less than one.  

double ran1(long *idum)
{
	int j;
	long k;
	static long iy=0;
	static long iv[NTAB];
	double temp;

	if (*idum <= 0 || !iy) {
		if (-(*idum) < 1) *idum=1;
		else *idum = -(*idum);
		for (j=NTAB+7;j>=0;j--) {
			k=(*idum)/IQ;
			*idum=IA*(*idum-k*IQ)-IR*k;
			if (*idum < 0) *idum += IM;
			if (j < NTAB) iv[j] = *idum;
		}
		iy=iv[0];
	}
	k=(*idum)/IQ;
	*idum=IA*(*idum-k*IQ)-IR*k;
	if (*idum < 0) *idum += IM;
	j=iy/NDIV;
	iy=iv[j];
	iv[j] = *idum;
	temp = AM*iy;
	if (temp > RNMX) return (double)RNMX;
	else return (double)temp;
}

#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX


// Todo : consider grouping in graph model util 

//------------------------- gauss

double gauss(double mean, double var, long *idum)  // jacobian trick to produce gaussian-
{									   // distributed random values (from NR)
	double u1, u2, v1, g;
	u1 = ran1(idum);
	u1 = sqrt(-2.0 * log(FLOOR(u1)));
	u2 = ran1(idum);
	u2 = cos(2*PI*u2);	
	v1 = u1*u2;
	g = v1*sqrt(var) + mean;
	return(g);
}
