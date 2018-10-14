#pragma once

#include "BaseSimulator.h"
#include "NodeData.h"

class SpringMethodSimulator: public BaseSimulator
{
public:
	SpringMethodSimulator(MyGlobalData *globalData) :BaseSimulator(globalData) {
		ReadConfig();
	}
	void SimulateOneFrame();
	std::string GetAlgoName() override { return "SpringMethod Simulator"; }
private:
	double k1, k2;
	void ReadConfig();
};