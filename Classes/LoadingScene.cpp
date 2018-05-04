﻿#include "LoadingScene.h"
#include "Resource.h"
#include "MainScene.h"
#include "DataSave.h"
#include "GlobalInstance.h"
#include "Building.h"

USING_NS_CC;

LoadingScene::LoadingScene()
{

}

LoadingScene::~LoadingScene()
{
	
}

Scene* LoadingScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	auto mainLayer = LoadingScene::create();

    // add layer as a child to scene
	scene->addChild(mainLayer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoadingScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("LoadingLayer.csb"));
	this->addChild(csbnode);

	this->scheduleOnce(schedule_selector(LoadingScene::delayLoadData), 0.2f);

    return true;
}


void LoadingScene::delayLoadData(float dt)
{
	int langtype = DataSave::getInstance()->getLocalLang();
	GlobalInstance::getInstance()->setLang(langtype);

	ResourceLang::load(langtype);

	Building::parseData();

	this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 0.2f);
}

void LoadingScene::showNextScene(float dt)
{
	Director::getInstance()->replaceScene(MainScene::createScene());
}

void LoadingScene::onExit()
{
	Layer::onExit();
}
