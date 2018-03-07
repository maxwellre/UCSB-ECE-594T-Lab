//**************************************
//  File:      random.h
//  Author:    Yon Visell
//  Copyright: 2002-3, Yon Visell
//**************************************
// (File renamed to avoid conflict with <random>)

// Random number generating utilities..

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
#define FLOOR(a)  	(((a) == 0.0) ? (0.0000001) : (a))
#define PI			3.1415926535897932384626433832797

double ran1(long *idum);
double gauss(double mean, double var, long *idum);