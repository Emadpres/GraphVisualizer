#include "WorldLayer.h"
#include "NodeDataReader.h"
#include "SpringMethodSimulator.h"
#include "BarycenterSimulator.h"
#include "OAE.h"
#include <cmath>
const double PI = 3.14159265;

USING_NS_CC;

bool WorldLayer::init()
{
    if ( !Layer::init() )	return false;
	currentSystemStatus = SystemStatus::NONE;
	currentNodeonProcess = -1;
    simulatorIntervalSec=1;
	/////////////////////////////
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	static cocos2d::Size designResolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    /////////////////////////////
    renderBox = Node::create();
    renderBox->setPosition(origin+visibleSize/2);
    renderBox->setContentSize(visibleSize);
    renderBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->addChild(renderBox);
    //////////////////
	auto bg = Sprite::create("grid-512.png");
	bg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    bg->setPosition(Vec2(0,0));
    //this->addChild(bg, 0);
	//this->setPosition(Vec2(visibleSize.width/2 - GRID_SIZE/2, visibleSize.height / 2 - GRID_SIZE / 2)); //move layer so the center of Grid be in center of screen
	/////////////////////////////
	statuslbl = Label::create("Status", "Arial", 25);
	statuslbl->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	statuslbl->setPosition(origin+Vec2(visibleSize.width/2, visibleSize.height - statuslbl->getContentSize().height/2));
	statuslbl->setColor(Color3B::WHITE);
	this->addChild(statuslbl,100);
	/////////////////////////////
    auto zoom_btn = cocos2d::ui::Button::create("zoom_minus.png");
    zoom_btn->setTouchEnabled(true);
    zoom_btn->addClickEventListener([=](Object* obj){renderBox->setScale(renderBox->getScale()-0.1);});
    zoom_btn->setPosition(origin+Vec2(zoom_btn->getContentSize().width/2,zoom_btn->getContentSize().height/2));
    this->addChild(zoom_btn,10);
    ////
    zoom_btn = cocos2d::ui::Button::create("zoom_plus.png");
    zoom_btn->setTouchEnabled(true);
    zoom_btn->addClickEventListener([=](Object* obj){renderBox->setScale(renderBox->getScale()+0.1);});
    zoom_btn->setPosition(origin+Vec2(zoom_btn->getContentSize().width/2,3*zoom_btn->getContentSize().height/2));
    this->addChild(zoom_btn,10);
    /////////////
    ReadConfig();
	HandleTouch();
	//
	LoadWorld();
    return true;
}

void WorldLayer::ReadConfig()
{
    std::ifstream configFileStream;
    configFileStream.open(configFile);
    if (!configFileStream.is_open())
    return;
    //////////////
    configFileStream >> simulatorIntervalSec;
    configFileStream.close();
}

void WorldLayer::LoadWorld()
{
	currentSystemStatus = SystemStatus::READING_NODE_DATA;
	NodeDataReader f(globalData);
	///////////////////////////////////////////////////////////////////////////// Create Objects
	for (int i = 0; i < globalData.totalNumberOfNode; ++i)
	{
		auto s = Sprite::create("circle-128.png");
		s->setColor(Color3B(rand_0_1() * 255, rand_0_1() * 255, rand_0_1() * 255));
		s->setScale(globalData.nodes[i].size*0.5);
		renderBox->addChild(s, 1);
        s->setTag(i);
		nodesSprite[i] = s;
		CreateTouchHandlerForDraggingNode(s);
		//////////////////
		auto l = Label::create(std::to_string(i),"Arial",20);
		l->setColor(Color3B::BLACK);
		l->setPosition(Vec2(s->getContentSize()/2));
		s->addChild(l);
	}
    for (int i = globalData.totalNumberOfNode; i < WorldLayer::N_NODES; ++i)
		nodesSprite[i] = nullptr;
	/////////////////////////////////////////////
	for (int i = 0; i < globalData.totalNumberOfEdge; ++i)
	{
		auto s = Sprite::create("pixel.png");
		s->setColor(Color3B(rand_0_1() * 255, rand_0_1() * 255, rand_0_1() * 255));
		//s->setScale(globalData.nodes[i].size);
		renderBox->addChild(s, 1);
		edges.push_back(s);
	}
	///////////////////////////////////////////////////////////////////////////// Locate nodes without position
	LocateNextUnlocatedNodeAfterIndex(-1);
}

void WorldLayer::LocateNextUnlocatedNodeAfterIndex(int whereToStartSearch)
{
	for (int i = whereToStartSearch+1; i < globalData.totalNumberOfNode; ++i)
	{
		if (globalData.nodes[i].PosX == 0 && globalData.nodes[i].PosY == 0)
		{
			currentSystemStatus = SystemStatus::LOCATING_NODES;
			currentNodeonProcess = i;
			statuslbl->setString("Waiting to locate the " + std::to_string(i) + "th Node");
			return;
		}
	}
	///////////////////////////
	currentSystemStatus = SystemStatus::RENDERING;
	UpdateNodesSprite(true);
	RunDrawingAlgorithm();
}

void WorldLayer::UpdateNodesSprite(bool noAnimation=false)
{
	for (int i = 0; i < globalData.totalNumberOfNode; ++i)
	{
		nodesSprite[i]->stopAllActions();
		if(noAnimation)
			nodesSprite[i]->setPosition(Vec2(globalData.nodes[i].PosX, globalData.nodes[i].PosY));
		else
			nodesSprite[i]->runAction(MoveTo::create(simulatorIntervalSec,Vec2(globalData.nodes[i].PosX, globalData.nodes[i].PosY)));
	}
	///////////////
	int currentEdgeIndex = 0;
	for (int i = 0; i < globalData.totalNumberOfNode; ++i)
	{
		for (int j = i + 1; j < globalData.totalNumberOfNode; ++j)
		{
			if (globalData.getMesure(i, j) != 0)
			{
				edges[currentEdgeIndex]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
				edges[currentEdgeIndex]->setScaleX(dis(globalData.nodes[i], globalData.nodes[j]));
				edges[currentEdgeIndex]->setScaleY(2);
				edges[currentEdgeIndex]->setPosition(Vec2(globalData.nodes[i].PosX, globalData.nodes[i].PosY));
				double dx = globalData.nodes[j].PosX - globalData.nodes[i].PosX;
				double dy = globalData.nodes[j].PosY - globalData.nodes[i].PosY;
				edges[currentEdgeIndex]->setRotation(atan2(dy, dx) * -180 / PI);
				currentEdgeIndex++;
			}
		}
	}
}

void WorldLayer::RunDrawingAlgorithm()
{
	sim = new SpringMethodSimulator(&globalData);
	//sim = new BarycenterSimulator(&globalData);

	schedule( [=](float dt) {
		statuslbl->setString("Running:  " + sim->GetAlgoName() +"\nStep:         " + std::to_string(sim->GetCurrentStep()));
		sim->SimulateOneFrame();
		UpdateNodesSprite(true);
	}, simulatorIntervalSec, "SprintSimulator");
	
}

void WorldLayer::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif    
}

void WorldLayer::HandleTouch()
{
	auto handleListener = EventListenerTouchOneByOne::create();
    handleListener->setSwallowTouches(true);
	///
	handleListener->onTouchBegan = [this](Touch* touch, Event* event) {
		if (currentSystemStatus == SystemStatus::LOCATING_NODES)
		{
			auto target = static_cast<Sprite*>(event->getCurrentTarget());
			Point locationInNode = target->convertToNodeSpace(touch->getLocation());
			////////////
			globalData.nodes[currentNodeonProcess].PosX = locationInNode.x;
			globalData.nodes[currentNodeonProcess].PosY = locationInNode.y;
			nodesSprite[currentNodeonProcess]->setPosition(Vec2(globalData.nodes[currentNodeonProcess].PosX, globalData.nodes[currentNodeonProcess].PosY));
			return true;
		}
        else if (currentSystemStatus == SystemStatus::RENDERING)
        {
            return true;
        }
		return false;
	};

	handleListener->onTouchMoved = [this](Touch* touch, Event* event)
	{
		if (currentSystemStatus == SystemStatus::LOCATING_NODES)
		{
			auto target = static_cast<Sprite*>(event->getCurrentTarget());
			Point locationInNode = target->convertToNodeSpace(touch->getLocation());
			////////////
			globalData.nodes[currentNodeonProcess].PosX = locationInNode.x;
			globalData.nodes[currentNodeonProcess].PosY = locationInNode.y;
			nodesSprite[currentNodeonProcess]->setPosition(Vec2(globalData.nodes[currentNodeonProcess].PosX, globalData.nodes[currentNodeonProcess].PosY));
		}
        else if (currentSystemStatus == SystemStatus::RENDERING)
        {
            auto delta = touch->getDelta();
            renderBox->setPosition(renderBox->getPosition()+delta);
        }
	};

	handleListener->onTouchEnded = [this](Touch* touch, Event* event)
	{
		if (currentSystemStatus == SystemStatus::LOCATING_NODES)
		{
			LocateNextUnlocatedNodeAfterIndex(currentNodeonProcess);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	_eventDispatcher->addEventListenerWithSceneGraphPriority(handleListener, this);
}

void WorldLayer::CreateTouchHandlerForDraggingNode(Sprite *targetNode)
{
	auto handleListener = EventListenerTouchOneByOne::create();
    handleListener->setSwallowTouches(true);
	///
	handleListener->onTouchBegan = [this](Touch* touch, Event* event) {
		if (currentSystemStatus == SystemStatus::RENDERING)
		{
			auto target = static_cast<Sprite*>(event->getCurrentTarget());
			Point locationInNode = target->convertToNodeSpace(touch->getLocation());
			//////
			Size s = target->getContentSize();
			Rect rect = Rect(-s.width / 2, -s.height, s.width * 2, s.height * 2);
			if (rect.containsPoint(locationInNode))
			{
				target->setScale(target->getScale() * 2);
				target->setOpacity(120);
				currentNodeonProcess = target->getTag();
				return true;
			}
			return false;
			return true;
		}
		return false;
	};

	handleListener->onTouchMoved = [this](Touch* touch, Event* event)
	{
		if (currentSystemStatus == SystemStatus::RENDERING)
		{
			auto target = static_cast<Sprite*>(event->getCurrentTarget());
			Point locationInNode = renderBox->convertToNodeSpace(touch->getLocation());
			////////////
			globalData.nodes[currentNodeonProcess].PosX = locationInNode.x;
			globalData.nodes[currentNodeonProcess].PosY = locationInNode.y;
			nodesSprite[currentNodeonProcess]->setPosition(Vec2(globalData.nodes[currentNodeonProcess].PosX, globalData.nodes[currentNodeonProcess].PosY));
		}
	};

	handleListener->onTouchEnded = [this](Touch* touch, Event* event)
	{
		if (currentSystemStatus == SystemStatus::RENDERING)
		{
			auto target = static_cast<Sprite*>(event->getCurrentTarget());
			////
			target->setScale(target->getScale() / 2);
			target->setOpacity(255);
			currentNodeonProcess = -1;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	_eventDispatcher->addEventListenerWithSceneGraphPriority(handleListener, targetNode);
}
