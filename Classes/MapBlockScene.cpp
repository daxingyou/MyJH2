﻿#include "MapBlockScene.h"
#include "Resource.h"
#include "FightHeroNode.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "CommonFuncs.h"
#include "MainScene.h"

MapBlockScene::MapBlockScene()
{

}


MapBlockScene::~MapBlockScene()
{

}

Scene* MapBlockScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto mapLayer = MapBlockScene::create();

	// add layer as a child to scene
	scene->addChild(mapLayer);

	// return the scene
	return scene;
}

bool MapBlockScene::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mapBlockLayer.csb"));
	this->addChild(csbnode);

	carrycountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrycount");
	foodcountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("r001count");
	solivercountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("solivercountlbl");

	cocos2d::ui::ScrollView* mapscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("ScrollView");
	mapscroll->setVisible(false);

	createMap();

	updateLabel();

	cocos2d::ui::Widget* gocitybtn = (cocos2d::ui::Widget*)csbnode->getChildByName("gocitybtn");
	gocitybtn->setTag(BTN_GOCITY);
	gocitybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(60 + 120 * i, 1196);
		fightHeroNode->setScale(0.84f);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i]);
		addChild(fightHeroNode);
	}

	return true;
}

void MapBlockScene::updateLabel()
{
	std::string str = StringUtils::format("%d/%d", MyRes::getMyPackageResCount(), GlobalInstance::getInstance()->getTotalCaryy());
	carrycountlbl->setString(str);
	str = StringUtils::format("%d", MyRes::getMyResCount("r001", MYPACKAGE));
	foodcountlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	foodcountlbl->setString(str);
}

void MapBlockScene::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case BTN_GOCITY:
			Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MainScene::createScene()));
			break;
		default:
			break;
		}
	}
}

void MapBlockScene::createMap()
{
	Sprite* scrollcontainer = Sprite::create("images/mapbg.jpg");
	scrollcontainer->setPosition(Vec2(0, 0));

	//Node *scrollcontainer = LayerColor::create(Color4B(255, 0, 0, 255));
	//scrollcontainer->setContentSize(Size(2424, 1600));
	ScrollView* scrollView = ScrollView::create();
	scrollView->setViewSize(Size(720, 800));
	scrollView->setPosition(0, 245);

	scrollView->setContainer(scrollcontainer);

	scrollView->setTouchEnabled(true);
	scrollView->setDirection(ScrollView::Direction::BOTH);
	scrollView->setMinScale(0.5);
	scrollView->setMaxScale(2);
	scrollView->setBounceable(false);
	scrollView->setDelegate(this);
	this->addChild(scrollView);
	float offsetx = scrollcontainer->getContentSize().width / 2 - scrollView->getViewSize().width / 2;
	float offsety = scrollcontainer->getContentSize().height / 2 - scrollView->getViewSize().height / 2;
	scrollView->setContentOffset(Vec2(-offsetx, -offsety));
}

void MapBlockScene::scrollViewDidScroll(ScrollView* view)
{
	return;
}

void MapBlockScene::scrollViewDidZoom(ScrollView* view)
{
	return;

}