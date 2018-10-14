#include "BarycenterSimulator.h"
#include "OAE.h"
#include <cmath>
#include "NodeData.h"
#include <fstream>

void BarycenterSimulator::SimulateOneFrame()
{
	BaseSimulator::SimulateOneFrame();
	MyGlobalData newData(*g);
	for (int n = numberOfFixNodes; n < g->totalNumberOfNode; ++n)
	{
		double t_x = 0, t_y = 0;
		///////////////////////////////////////
		int deg = 0;
        for (int p = 0; p < g->totalNumberOfNode; ++p)
			//if (n != p && g->relationships[n][p] != 0)
            if(n!=p && g->isAdjacent(n,p))
				deg++;
		///////////////////////////////////////
		for (int v = 0; v < g->totalNumberOfNode; ++v)
		{
			if (n == v) continue;
			if (g->isAdjacent(n, v)==false) continue;
			////////////
			t_x += g->nodes[v].PosX;
			t_y += g->nodes[v].PosY;
			/////////////////////////////////////////////////////////////
		}
		newData.nodes[n].PosX = t_x / deg;
		newData.nodes[n].PosY = t_y / deg;
	}
	/////////////////////////////
	for (int n = numberOfFixNodes; n < g->totalNumberOfNode; ++n)
	{
		//if (n == 2) continue;
		g->nodes[n].PosX = newData.nodes[n].PosX;
		g->nodes[n].PosY = newData.nodes[n].PosY;
	}
}

void BarycenterSimulator::ReadConfig()
{
	std::ifstream configFileStream;
	configFileStream.open(configFile);
	if (!configFileStream.is_open())
		return;
	//////////////
	configFileStream >> k1 >> k1 >> k2 >> numberOfFixNodes;
	configFileStream.close();
}

