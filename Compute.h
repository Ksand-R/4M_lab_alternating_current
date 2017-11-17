#pragma once
#include "row_table.h"

typedef double(*func)(double, double);
typedef double(*Method) (func, double, double, double);

double diff_eq(double x, double y);

double RK4(func, double h, double x, double y);

const Method method_array[] = { RK4 };

void Integrate(Method, func, double x0, double maxX,
	double y0, int maxI, double h0, double eps_,
	 Table* T, int p, double _L = 10, 
	double _R = 10, double _E0 = 10, double _OM = 10);