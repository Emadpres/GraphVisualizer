#pragma once

#include "BaseSimulator.h"
#include "NodeData.h"

class BarycenterSimulator : public BaseSimulator
{
public:
	BarycenterSimulator(MyGlobalData *globalData) :BaseSimulator(globalData) {
		ReadConfig();
	}
	void SimulateOneFrame();
	std::string GetAlgoName() override { return "Barycenter Simulator"; }
private:
	double k1, k2;
	int numberOfFixNodes;
	void ReadConfig();
};