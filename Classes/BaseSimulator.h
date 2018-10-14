#pragma once

#include "NodeData.h"

class BaseSimulator
{
public:
	BaseSimulator(MyGlobalData *globalData)
	{
		this->g = globalData;
		currentStep = 0;
	}
	virtual void SimulateOneFrame()  { currentStep++; }
	virtual std::string GetAlgoName() { return "Override me with your name"; }
	virtual int GetCurrentStep() { return currentStep; };
protected:
	MyGlobalData *g;
private:
	int currentStep;
};
