#pragma once
#include <string>
#include "cocos2d.h"
const int GRID_SIZE = 512;

const std::string inputFile = cocos2d::FileUtils::getInstance()->fullPathForFilename("inp.txt");
const std::string configFile = cocos2d::FileUtils::getInstance()->fullPathForFilename("simConf.txt");
const double defaultDis = 500;

struct MyNodeData
{
	double PosX, PosY;
	double size;
};

struct MyGlobalData
{
	int totalNumberOfNode, totalNumberOfEdge;
    std::vector<MyNodeData> nodes;
    std::vector<std::vector<std::tuple<int/*which node*/, double/*measure*/>>> relationships; //adj matrix
    
//    MyGlobalData(const MyGlobalData& g)
//    {
//        this->totalNumberOfNode = g.totalNumberOfNode;
//    }
    
    double getMesure(int first, int second)
    {
        auto neighbors = this->relationships[first];
        for(auto t: neighbors)
        {
            if(std::get<0>(t)==second)
                return std::get<1>(t);
        }
        return 0;
    }
    
    bool isAdjacent(int first, int second)
    {
        auto neighbors = relationships[first];
        for(auto t: neighbors)
        {
            if(std::get<0>(t)==second)
              return true;
        }
        //////////////////
        neighbors = relationships[second];
        for(auto t: neighbors)
        {
            if(std::get<0>(t)==first)
                return true;
        }
        return false;
    }
};
