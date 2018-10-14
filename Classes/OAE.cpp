#include "OAE.h"

double dis(MyNodeData a, MyNodeData b)
{
	double t = pow((a.PosX - b.PosX), 2) + pow((a.PosY - b.PosY), 2);
	return sqrt(t);
}
