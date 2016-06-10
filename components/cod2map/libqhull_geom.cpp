#include <limits>
#include <math.h>
#include "libqhull_geom.h"

#define True 1
#define False 0

#define wmin_(x, y) if ((y) < (x)) (x) = (y);
#define REALmax (DBL_MAX)

double *Wmindenom = (double *)0x315A2128;
double *qh_MINdenom = (double *)0x315A32E8;
double *qh_MINdenom_1 = (double *)0x315A32E0;

void qh_normalize2(coordT *normal, int dim, boolT toporient, realT *minnorm, boolT *ismin)
{
	int k;
	realT *colp, *maxp, norm = 0, temp, *norm1, *norm2, *norm3;
	boolT zerodiv;

	norm1 = normal + 1;
	norm2 = normal + 2;
	norm3 = normal + 3;
	if (dim == 2)
		norm = sqrt((*normal)*(*normal) + (*norm1)*(*norm1));
	else if (dim == 3)
		norm = sqrt((*normal)*(*normal) + (*norm1)*(*norm1) + (*norm2)*(*norm2));
	else if (dim == 4) {
		norm = sqrt((*normal)*(*normal) + (*norm1)*(*norm1) + (*norm2)*(*norm2)
			+ (*norm3)*(*norm3));
	}
	else if (dim > 4) {
		norm = (*normal)*(*normal) + (*norm1)*(*norm1) + (*norm2)*(*norm2)
			+ (*norm3)*(*norm3);
		for (k = dim - 4, colp = normal + 4; k--; colp++)
			norm += (*colp) * (*colp);
		norm = sqrt(norm);
	}
	if (minnorm) {
		if (norm < *minnorm)
			*ismin = True;
		else
			*ismin = False;
	}

	wmin_(*Wmindenom, norm);
	if (norm > *qh_MINdenom) {
		if (!toporient)
			norm = -norm;
		*normal /= norm;
		*norm1 /= norm;
		if (dim == 2)
			; /* all done */
		else if (dim == 3)
			*norm2 /= norm;
		else if (dim == 4) {
			*norm2 /= norm;
			*norm3 /= norm;
		}
		else if (dim > 4) {
			*norm2 /= norm;
			*norm3 /= norm;
			for (k = dim - 4, colp = normal + 4; k--;)
				*colp++ /= norm;
		}
	}
	else if (norm == 0.0) {
		temp = sqrt(1.0 / dim);
		for (k = dim, colp = normal; k--;)
			*colp++ = temp;
	}
	else {
		if (!toporient)
			norm = -norm;
		for (k = dim, colp = normal; k--; colp++) { /* k used below */
			temp = qh_divzero(*colp, norm, *qh_MINdenom_1, &zerodiv);
			if (!zerodiv)
				*colp = temp;
			else {
				maxp = qh_maxabsval(normal, dim);
				temp = ((*maxp * norm >= 0.0) ? 1.0 : -1.0);
				for (k = dim, colp = normal; k--; colp++)
					*colp = 0.0;
				*maxp = temp;
				zzinc_(Znearlysingular);
				trace0((qh ferr, 1, "qh_normalize: norm=%2.2g too small during p%d\n",
					norm, qh furthest_id));
				return;
			}
		}
	}
}

realT qh_divzero(realT numer, realT denom, realT mindenom1, boolT *zerodiv)
{
	realT temp, numerx, denomx;

	if (numer < mindenom1 && numer > -mindenom1) {
		numerx = fabs(numer);
		denomx = fabs(denom);
		if (numerx < denomx) {
			*zerodiv = False;
			return numer / denom;
		}
		else {
			*zerodiv = True;
			return 0.0;
		}
	}
	temp = denom / numer;
	if (temp > mindenom1 || temp < -mindenom1) {
		*zerodiv = False;
		return numer / denom;
	}
	else {
		*zerodiv = True;
		return 0.0;
	}
}

realT *qh_maxabsval(realT *normal, int dim)
{
	realT maxval = -REALmax;
	realT *maxp = nullptr, *colp, absval;
	int k;

	for (k = dim, colp = normal; k--; colp++) {
		absval = fabs(*colp);
		if (absval > maxval) {
			maxval = absval;
			maxp = colp;
		}
	}
	return maxp;
}