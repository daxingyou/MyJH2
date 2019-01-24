﻿#include "MakeResLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "SmithyLayer.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "BuyCoinLayer.h"
#include "CannotTouchLayer.h"

USING_NS_CC;

MakeResLayer::MakeResLayer()
{

}

MakeResLayer::~MakeResLayer()
{
	if (!NewGuideLayer::checkifNewerGuide(75) && NewGuideLayer::checkifNewerGuide(76))
	{
		SmithyLayer* layer = (SmithyLayer*)g_mainScene->getChildByName("2smithy");
		if (layer != NULL)
		{
			layer->delayShowNewerGuide(0);
		}
	}
}


MakeResLayer* MakeResLayer::create(void* data, int actiontype)
{
	MakeResLayer *pRet = new(std::nothrow)MakeResLayer();
	if (pRet && pRet->init(data, actiontype))
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

// on "init" you need to initialize your instance
bool MakeResLayer::init(void* data, int actiontype)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_data = data;
	m_actiontype = actiontype;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	csbnode = CSLoader::createNode(ResourcePath::makePath("makeResLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	std::string titlestr;		//标题
	std::string btn1_text;//按钮1中的文字图片
	std::string btn2_text;//按钮2中的文字图


	btn1_text = "makeresbtn_text";
	btn2_text = "drmakeresbtn_text";
	std::string resid = (char*)m_data;
	titlestr = ResourceLang::map_lang["makerestitle"];

	vec_res = GlobalInstance::map_AllResources[resid].vec_needres;

	int costcount = GlobalInstance::map_AllResources[resid].silverval/10;

	costcoindv.setValue(costcount);

	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(titlestr);
	//装备名称
	cocos2d::ui::Text* equipname = (cocos2d::ui::Text*)csbnode->getChildByName("equipname");
	equipname->setString(GlobalInstance::map_AllResources[resid].name);
	
	std::string needresstr;
	Node* resbgnode = csbnode->getChildByName("resbgnode");

	//制作的装备
	cocos2d::ui::ImageView* equipres = (cocos2d::ui::ImageView*)resbgnode->getChildByName("equipres");
	std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());

	equipres->loadTexture(ResourcePath::makePath(resstr), cocos2d::ui::Widget::TextureResType::PLIST);

	//3个资源的展示
	for (unsigned int i = 0; i < 3; i++)
	{
		if (i < vec_res.size())
		{
			std::map<std::string, int> map_res = vec_res[i];
			std::map<std::string, int>::iterator map_it = map_res.begin();

			std::string resid = map_it->first;
			std::string boxnamestr = StringUtils::format("makeresbox%d", i);
			cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbgnode->getChildByName(boxnamestr)->getChildByName("res");

			std::string str = StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

			countlbl[i] = (cocos2d::ui::TextBMFont*)resbgnode->getChildByName(boxnamestr)->getChildByName("count");

			str = StringUtils::format("%d/%d", MyRes::getMyResCount(resid), map_res[resid]);

			countlbl[i]->setString(str);

			if (map_res[resid] > 0)
			{
				std::string onestr = StringUtils::format("%s%d", GlobalInstance::map_AllResources[resid].name.c_str(), map_res[resid]);
				if (needresstr.length() > 0)
					needresstr.append(ResourceLang::map_lang["dunhao"]);
				needresstr.append(onestr);
			}
		}
		else
		{
			std::string boxnamestr = StringUtils::format("makeresbox%d", i);
			cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)resbgnode->getChildByName(boxnamestr);
			resbox->setVisible(false);
		}
	}

	cocos2d::ui::Text* needresdesc = (cocos2d::ui::Text*)csbnode->getChildByName("needresdesc");
	needresstr = StringUtils::format(ResourceLang::map_lang["needresdesc"].c_str(), needresstr.c_str());
	needresdesc->setString(needresstr);

	//按钮1
	cocos2d::ui::Widget* makebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("makebtn");
	makebtn->setTag(1000);
	makebtn->addTouchEventListener(CC_CALLBACK_2(MakeResLayer::onBtnClick, this));
	//按钮1文字
	cocos2d::ui::ImageView* lvupbtntxt = (cocos2d::ui::ImageView*)makebtn->getChildByName("text");
	lvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn1_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* drmakebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drmakebtn");
	drmakebtn->setTag(1001);
	drmakebtn->addTouchEventListener(CC_CALLBACK_2(MakeResLayer::onBtnClick, this));
	//按钮2文字
	cocos2d::ui::ImageView* drlvupbtntxt = (cocos2d::ui::ImageView*)drmakebtn->getChildByName("text");
	drlvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn2_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	coincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	std::string coinstr = StringUtils::format("x%d", costcoindv.getValue());
	coincountlbl->setString(coinstr);

	std::vector<float> vec_rnds = makeResRnd();
	std::string rndstr;
	std::string rnddescname;
	for (int i = 0; i < 5; i++)
	{
		rnddescname = StringUtils::format("rnddesc%d", i);
		cocos2d::ui::Text* rnddesc = (cocos2d::ui::Text*)csbnode->getChildByName(rnddescname);

		rnddescname = StringUtils::format("makeresrnd_%d", i);

		std::string kstr;
		
		if (vec_rnds[i] < 0.0001)
			kstr = "--";
		else
			kstr = StringUtils::format("%.2f%%", vec_rnds[i]);
		rndstr = StringUtils::format(ResourceLang::map_lang[rnddescname].c_str(), kstr.c_str());
		rnddesc->setString(rndstr);
	}

	updateUI(0);
	this->schedule(schedule_selector(MakeResLayer::updateUI), 1);

	this->scheduleOnce(schedule_selector(MakeResLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MakeResLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(74))
	{
		if (NewGuideLayer::checkifNewerGuide(75))
		{
			showNewerGuide(75);
		}
	}
}

void MakeResLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 75)
	{
		nodes.push_back(csbnode->getChildByName("makebtn"));
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void MakeResLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 1000://制作
			if (checkResIsEnough())//资源足够
			{	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];
				
				//减掉资源

				for (unsigned int i = 0; i < vec_res.size(); i++)
				{
					std::map<std::string, int> map_res = vec_res[i];
					std::map<std::string, int>::iterator map_it = map_res.begin();
					std::string resid = map_it->first;
					ResBase* res = MyRes::getMyRes(resid);
					MyRes::Use(res, map_res[resid]);
				}
				showMakeAnim();
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
			}
			break;
		case 1001://立即制作
			{
				if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= costcoindv.getValue())
				{
					GlobalInstance::getInstance()->costMyCoinCount(costcoindv);
					//action();
					showMakeAnim();
				}
				else
				{
					MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);

					Layer* layer = BuyCoinLayer::create(costcoindv.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
					Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
				}
			}
			break;
		default:
			break;
		}
	}
}

void MakeResLayer::showMakeAnim()
{
	CannotTouchLayer* layer = CannotTouchLayer::create();
	this->addChild(layer, 5, "notouchlayer");

	Node* resbgnode = csbnode->getChildByName("resbgnode");
	cocos2d::ui::ImageView* equipres = (cocos2d::ui::ImageView*)resbgnode->getChildByName("equipres");
	for (int i = 0; i < 3; i++)
	{
		std::string boxnamestr = StringUtils::format("makeresbox%d", i);
		Node* resbox = resbgnode->getChildByName(boxnamestr);
		ActionInterval* ac1 = Spawn::create(FadeOut::create(1.5f), EaseSineIn::create(MoveTo::create(1.0f, Vec2(equipres->getPosition()))), NULL);
		resbox->runAction(Sequence::create(ac1, NULL));
	}

	equipres->runAction(Sequence::create(DelayTime::create(1.0f), EaseSineIn::create(RotateTo::create(2.0f, 720 * 2)), CallFuncN::create(CC_CALLBACK_0(MakeResLayer::finishMakeAnim, this)), NULL));

}

void MakeResLayer::finishMakeAnim()
{
	Node* resbgnode = csbnode->getChildByName("resbgnode");
	cocos2d::ui::ImageView* equipres = (cocos2d::ui::ImageView*)resbgnode->getChildByName("equipres");
	equipres->runAction(Sequence::create(ScaleTo::create(0.2f, 3), DelayTime::create(0.5f), CallFuncN::create(CC_CALLBACK_0(MakeResLayer::action, this)), NULL));
}

void MakeResLayer::action()
{
	AnimationEffect::closeAniEffect((Layer*)this);
	std::string rid = (char*)m_data;
	SmithyLayer* smithyLayer = (SmithyLayer*)this->getParent();
	smithyLayer->makeRes(rid);
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_MAKERES);
}

void MakeResLayer::updateUI(float dt)
{
	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (unsigned int i = 0; i < vec_res.size(); i++)
	{
		std::map<std::string, int> map_res = vec_res[i];
		std::map<std::string, int>::iterator map_it = map_res.begin();

		std::string resid = map_it->first;
		int mycount = MyRes::getMyResCount(resid);
		std::string str = StringUtils::format("%d/%d", MyRes::getMyResCount(resid), map_res[resid]);

		countlbl[i]->setString(str);
		if (mycount < map_res[resid])
			countlbl[i]->setColor(Color3B(255, 0, 0));
		else
			countlbl[i]->setColor(Color3B(255, 255, 255));
	}

	//std::string str = StringUtils::format("x%d", (m_building->level.getValue() + 1) * 100);
	//coincountlbl->setString(str);
}


//资源足够升级
bool MakeResLayer::checkResIsEnough()
{
	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (unsigned int i = 0; i < vec_res.size(); i++)
	{
		std::map<std::string, int> map_res = vec_res[i];
		std::map<std::string, int>::iterator map_it = map_res.begin();

		std::string resid = map_it->first;
		int mycount = MyRes::getMyResCount(resid);

		if (mycount < map_res[resid])
			return false;
	}
	return true;
}

std::vector<float> MakeResLayer::makeResRnd()
{
	Building* smithyBuilding = Building::map_buildingDatas["2smithy"];
	std::vector<float> vec_ret;
	if (smithyBuilding->level.getValue() < 4)
	{
		vec_ret.push_back(100.00f);
		vec_ret.push_back(0.00f);
		vec_ret.push_back(0.00f);
		vec_ret.push_back(0.00f);
		vec_ret.push_back(0.00f);
	}
	else if (smithyBuilding->level.getValue() < 9)
	{
		vec_ret.push_back(80.00f);
		vec_ret.push_back(20.00f);
		vec_ret.push_back(0.00f);
		vec_ret.push_back(0.00f);
		vec_ret.push_back(0.00f);
	}
	else if (smithyBuilding->level.getValue() < 14)
	{
		vec_ret.push_back(60.00f);
		vec_ret.push_back(30.00f);
		vec_ret.push_back(10.00f);
		vec_ret.push_back(0.00f);
		vec_ret.push_back(0.00f);
	}
	else
	{
		vec_ret.push_back(55.00f);
		vec_ret.push_back(30.00f);
		vec_ret.push_back(10.00f);
		vec_ret.push_back(5.00f);
		vec_ret.push_back(0.00f);
	}
	return vec_ret;
}

void MakeResLayer::onExit()
{
	Layer::onExit();
}


