﻿#include "PopNewHeroLayer.h"
#include "CommonFuncs.h"
#include "AnimationEffect.h"
#include "Resource.h"
#include "Const.h"
#include "GlobalInstance.h"
#include "MainMapScene.h"
#include "RandHeroLayer.h"

PopNewHeroLayer::PopNewHeroLayer()
{

}


PopNewHeroLayer::~PopNewHeroLayer()
{
	RandHeroLayer* randlayer = (RandHeroLayer*)g_mainScene->getChildByName("RandHeroLayer");
	randlayer->delayShowNewerGuide(0);
}

PopNewHeroLayer* PopNewHeroLayer::create(Hero* hero)
{
	PopNewHeroLayer *pRet = new(std::nothrow)PopNewHeroLayer();
	if (pRet && pRet->init(hero))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool PopNewHeroLayer::init(Hero* hero)
{
	if (!Layer::init())
		return false;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 220));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("popNewHeroLayer.csb"));
	this->addChild(csbnode, 0, "csbnode");
	
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("newherotitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("heroimg");
	headimg->ignoreContentAdaptWithSize(true);
	headimg->setScale(0.5f);

	vocimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("herovoc");
	std::string vocimgstr = StringUtils::format("newherovoc%d_text", hero->getVocation());
	vocimg->loadTexture(ResourcePath::makeTextImgPath(vocimgstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//品质
	quimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("heroqu");

	std::string quimgstr = StringUtils::format("heroattrqu_%d", hero->getPotential());
	quimg->loadTexture(ResourcePath::makeTextImgPath(quimgstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	std::string str = StringUtils::format("hfull_%d_%d.png", hero->getVocation(), hero->getSex());
	headimg->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	light = csbnode->getChildByName("light");

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};

	this->scheduleOnce(schedule_selector(PopNewHeroLayer::delayShowLightAnim), 0.5f);
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void PopNewHeroLayer::delayShowLightAnim(float dt)
{
	light->runAction(RepeatForever::create(RotateTo::create(10, 720)));
}