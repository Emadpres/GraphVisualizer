#include "SpringMethodSimulator.h"
#include "OAE.h"
#include <cmath>
#include "NodeData.h"
#include <fstream>

void SpringMethodSimulator::SimulateOneFrame()
{
	BaseSimulator::SimulateOneFrame();
	MyGlobalData newData;
    for(auto n:g->nodes)
        newData.nodes.push_back(n);
    
    for (int n = 0; n < g->totalNumberOfNode; ++n)
	{
		//if (n == 2) continue;
		double Fn_x = 0, Fn_y = 0;

		for (int v = 0; v < g->totalNumberOfNode; ++v) // Repulsive
		{
			/////////////////////////////////////////////////////////////
			double nodeDis = dis(g->nodes[n], g->nodes[v]);
            double deltaDistance = dis(g->nodes[n], g->nodes[v]) - g->getMesure(n, v);
			double deltaDistance_x = g->nodes[n].PosX - g->nodes[v].PosX;
			double deltaDistance_y = g->nodes[n].PosY - g->nodes[v].PosY;
			/////////////////////////////////////////////////////////////
			if (n == v) continue;
			double t_x = (k2 / (nodeDis*nodeDis))*(deltaDistance_x / nodeDis);
			double t_y = (k2 / (nodeDis*nodeDis))*(deltaDistance_y / nodeDis);
			//double sizeFactor = g->nodes[v].size*g->nodes[n].size;
			//sizeFactor *= sizeFactor;
			//t_x = t_x * sizeFactor / (nodesDis*nodesDis);
			//t_y = t_y * sizeFactor / (nodesDis*nodesDis);
			Fn_x = Fn_x + t_x;// +((std::rand() / (float)RAND_MAX) - 0.5) * 2;
			Fn_y = Fn_y + t_y;// + ((std::rand() / (float)RAND_MAX)-0.5) * 2;
		}
		for (int v = 0; v < g->totalNumberOfNode; ++v) // Edges
		{
			/////////////////////////////////////////////////////////////
			double nodeDis = dis(g->nodes[n], g->nodes[v]);
            double deltaDistance = dis(g->nodes[n], g->nodes[v]) - g->getMesure(n, v);
			double deltaDistance_x = g->nodes[n].PosX - g->nodes[v].PosX;
			double deltaDistance_y = g->nodes[n].PosY - g->nodes[v].PosY;
			/////////////////////////////////////////////////////////////
			if (n == v) continue;
			if (g->getMesure(n, v) == 0) continue;
			if (deltaDistance_x == 0) deltaDistance_x = 1;
			if (deltaDistance_y == 0) deltaDistance_y = 1;
			Fn_x = Fn_x - (k1 * deltaDistance) * (deltaDistance_x / nodeDis);
			Fn_y = Fn_y - (k1 * deltaDistance) * (deltaDistance_y / nodeDis);
		}
		const int KKK = 60;
		if (Fn_x > KKK) Fn_x = KKK;
		if (Fn_y > KKK) Fn_y = KKK;
		if (Fn_x < -KKK) Fn_x = -KKK;
		if (Fn_y < -KKK) Fn_y = -KKK;
		///////////////////////////////////
		const int LLL = 2;
		if (Fn_x < LLL && Fn_x>-LLL) Fn_x = 0;
		if (Fn_y < LLL && Fn_y>-LLL) Fn_y = 0;
		newData.nodes[n].PosX = g->nodes[n].PosX + Fn_x;
		newData.nodes[n].PosY = g->nodes[n].PosY +Fn_y;
	}
	/////////////////////////////
	for (int n = 0; n < g->totalNumberOfNode; ++n)
	{
		//if (n == 2) continue;
		g->nodes[n].PosX = newData.nodes[n].PosX;
		g->nodes[n].PosY = newData.nodes[n].PosY;
	}
}

void SpringMethodSimulator::ReadConfig()
{
	std::ifstream configFileStream;
	configFileStream.open(configFile);
	if (!configFileStream.is_open())
		return;
	//////////////
	configFileStream >> k1 >> k1 >> k2;
	configFileStream.close();
}
