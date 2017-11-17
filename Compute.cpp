#include <cmath>
#include "row_table.h"
#include "compute.h"


static double eps;
static double L, R, E0, OM, I0;

void set_task_param(double _L, double _R, double _E0,
	double _OM, double _I0)
{
	L	= _L;
	R = _R;
	E0	= _E0;
	OM	= _OM;
	I0	= _I0;
}

double diff_eq(double x, double y)
{
	return (E0 * sinl(OM * x) - R * y) / (L); 
}

double RK4(func f, double h, double x, double y)
{
	double k1 = f(x, y);
	double k2 = f(x + h / 2., y + k1 * h / 2.);
	double k3 = f(x + h / 2., y + k2 * h / 2.);
	double k4 = f(x + h, y + (k3 * h));

	return y + ((1. / 6.) * h * (k1 + 2. * k2 + 2. * k3 + k4));
}
	Row double_count(Method method, func f, int p, double &h_, Row* R)
{
		Row Res; Res.stepDec = Res.stepInc  = 0;
		double y1 = method(f, h_, R->xi, R->vi_pr);
		double y2 = method(f, h_*0.5, R->xi + h_*0.5, method(f, h_*0.5, R->xi, R->vi_pr));
		double e_ = (y2 - y1) / (pow(2, p) - 1);
		if (abs(e_)>eps)
		{
			while (abs(e_) > eps) 
			{
				h_ *= 0.5;
				Res.stepDec++;
				y1 = method(f, h_, R->xi, R->vi_pr);
				y2 = method(f, h_*0.5, R->xi + h_*0.5, 
					method(f, h_*0.5, R->xi, R->vi_pr));
				e_ = (y2 - y1) / (pow(2, p) - 1);
			}
		}
	
		Res.hi_1 = h_;
		Res.xi = R->xi + h_;

		if (abs(e_)<eps / (pow(2, p + 1)))
		{
			Res.stepInc++;
			h_ *= 2;
		}

		Res.vi_pr = y1;
		Res.vi_cor = y2;
		Res.viPr_vi_сor = y1 - y2;
		Res.s = e_;
		Res.vi_utoch = y1 + e_*pow(2, p);
		Res.vi_itog = y1;
		Res.total = 0;
		return Res;

}

void Integrate(Method method, func f, double x0, double maxX, double y0, 
				int maxI, double h0, double eps_,  Table* T, 
				int p, double _L, double _R, double _E0,
				double _OM)
{
	eps = eps_;
	set_task_param(_L, _R, _E0, _OM, y0);
	int i = 0;
	Row tmp;
	tmp.i = 0;
	tmp.hi_1 = 0;
	tmp.xi = x0;
	tmp.vi_pr = y0;
	tmp.vi_cor = y0;
	tmp.viPr_vi_сor = 0;
	tmp.s = 0;
	tmp.vi_utoch = y0;
	tmp.vi_itog = y0;
	tmp.stepInc = 0;
	tmp.stepDec = 0;
	tmp.total = 0;
	double hi = h0;

	while (tmp.xi + hi<maxX && i<maxI)
	{
		T->AddRow(tmp);
		i++;
		tmp = double_count(method, f, p, hi, &tmp);
		tmp.i = i;
	}
	T->AddRow(tmp);

	if (i < maxI) {
		hi = maxX - tmp.xi;
		do {
			i++;
			tmp = double_count(method, f, p, hi, &tmp);
			tmp.i = i;
			T->AddRow(tmp);
		} while (maxX - tmp.xi > eps);
	}

}