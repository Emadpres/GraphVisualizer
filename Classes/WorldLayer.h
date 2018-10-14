#ifndef __WORLD_LAYER_H__
#define __WORLD_LAYER_H__

#include "cocos2d.h"
#include <ui/CocosGUI.h>
#include "NodeData.h"
#include "BaseSimulator.h"


class WorldLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    CREATE_FUNC(WorldLayer);
    void menuCloseCallback(cocos2d::Ref* pSender);
    const static int N_NODES = 500;
private:
	enum class SystemStatus{ NONE, READING_NODE_DATA, LOCATING_NODES, RENDERING} currentSystemStatus;
	cocos2d::Label *statuslbl;
	void HandleTouch();
	void CreateTouchHandlerForDraggingNode(cocos2d::Sprite *targetNode);
	/////////////////////////// Locating Node
	int currentNodeonProcess; // used in SystemStatus::LOCATING_NODES
	void LocateNextUnlocatedNodeAfterIndex(int whereToStartSearch);
	///////////////////////////
	MyGlobalData globalData;
	cocos2d::Sprite *nodesSprite[N_NODES];
	std::vector<cocos2d::Sprite*> edges;
	void LoadWorld();
	void UpdateNodesSprite(bool noAnimation);
	/////////////////////////////
    cocos2d::Node *renderBox;
    double simulatorIntervalSec;
	BaseSimulator *sim;
	void RunDrawingAlgorithm();
    void ReadConfig();
	/////////////////
};

#endif
