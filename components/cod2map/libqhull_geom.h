#pragma once

// http://www.qhull.org/src/libqhull/geom.c

typedef double coordT;
typedef double realT;
typedef int boolT;

/*
qh_normalize2( normal, dim, toporient, minnorm, ismin )
normalize a vector and report if too small
qh.MINdenom/MINdenom1 are the upper limits for divide overflow

returns:
normalized vector
flips sign if !toporient
if minnorm non-NULL,
sets ismin if normal < minnorm

notes:
if zero norm
sets all elements to sqrt(1.0/dim)
if divide by zero (divzero())
sets largest element to   +/-1
bumps Znearlysingular

design:
computes norm
test for minnorm
if not near zero
normalizes normal
else if zero norm
sets normal to standard value
else
uses qh_divzero to normalize
if nearzero
sets norm to direction of maximum value
*/
void qh_normalize2(coordT *normal, int dim, boolT toporient, realT *minnorm, boolT *ismin);

/*
qh_divzero( numer, denom, mindenom1, zerodiv )
divide by a number that's nearly zero
mindenom1= minimum denominator for dividing into 1.0

returns:
quotient
sets zerodiv and returns 0.0 if it would overflow

design:
if numer is nearly zero and abs(numer) < abs(denom)
return numer/denom
else if numer is nearly zero
return 0 and zerodiv
else if denom/numer non-zero
return numer/denom
else
return 0 and zerodiv
*/
realT qh_divzero(realT numer, realT denom, realT mindenom1, boolT *zerodiv);

/*
qh_maxabsval( normal, dim )
return pointer to maximum absolute value of a dim vector
returns NULL if dim=0
*/
realT *qh_maxabsval(realT *normal, int dim);