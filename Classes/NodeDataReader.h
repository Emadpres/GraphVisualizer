#pragma once

#include<string>
#include <fstream>
#include <ctime>
#include "NodeData.h"
#include <unistd.h>
class NodeDataReader
{
public:
	NodeDataReader(MyGlobalData& g)
	{
		srand(std::time(0));
		std::ifstream inFile;
        inFile.open(cocos2d::FileUtils::getInstance()->fullPathForFilename(inputFile));
		if (!inFile.is_open())
			return;
		//////////////
		int totalNode;
		int nodeA, nodeB;
		double size, rel;
		inFile >> totalNode;
		g.totalNumberOfNode = totalNode;
		////////////// Positions
		for (int i = 0; i < totalNode; ++i)
		{
            g.nodes.push_back(MyNodeData());
			g.nodes[i].PosX = (std::rand() / (float)RAND_MAX) * 200+300;
			g.nodes[i].PosY = (std::rand() / (float)RAND_MAX) * 200+300;
			g.nodes[i].size = 1;
			//inFile >> g.nodes[i].size >> g.nodes[i].PosX >> g.nodes[i].PosY;
            /////////////
            g.relationships.push_back(std::vector<std::tuple<int/*which node*/, double/*measure*/>>());
		}
		///////////////
        
		while (!inFile.eof())
		{
			inFile >> nodeA >> nodeB >> rel;
			g.totalNumberOfEdge++;
			if (nodeA == nodeB && nodeA == rel && rel == 0) break;
            if(nodeA<=nodeB)
                g.relationships[nodeA].push_back(std::tuple<int/*which node*/, double/*measure*/>(nodeB,rel));
            else
                g.relationships[nodeB].push_back(std::tuple<int/*which node*/, double/*measure*/>(nodeA,rel));
		}
        g.relationships[0].push_back(std::tuple<int, double>(1,20));
        g.relationships[0].push_back(std::tuple<int, double>(2,20));
        g.relationships[0].push_back(std::tuple<int, double>(3,20));
        g.relationships[0].push_back(std::tuple<int, double>(4,20));
        g.totalNumberOfEdge = 4;
        //g.nodes[0].size=10;

        for (int i = 0; i < std::min(500,totalNode*totalNode/4); ++i)
        {
            if(std::rand()%2 == 0)  continue;
            
            int a = std::rand()%totalNode;
            int b = std::rand()%totalNode;
           // if(g.relationships[a][b]!=0) continue;
            if(a==b) continue;
            
            g.totalNumberOfEdge++;
            int d = std::rand()%900+50;
           // g.relationships[a][b] = d;
           // g.relationships[b][a] = d;

            
        }
        
        
        
        
		//////////////
		inFile.close();
	}
};
