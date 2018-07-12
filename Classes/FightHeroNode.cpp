﻿#include "FightHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "FighterAttrLayer.h"
#include "FightingLayer.h"
#include "LoadingBarProgressTimer.h"
#include "GlobalInstance.h"

FightHeroNode::FightHeroNode()
{
	atkspeed = 0.0f;
	timedt = 0.0f;
	ispause = false;
	hurtup = 0.0f;
}


FightHeroNode::~FightHeroNode()
{

}

FightHeroNode* FightHeroNode::create()
{
	FightHeroNode *pRet = new(std::nothrow)FightHeroNode();
	if (pRet && pRet->init())
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

bool FightHeroNode::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightHeroNode.csb"));
	this->addChild(csbnode);

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	headbox->addTouchEventListener(CC_CALLBACK_2(FightHeroNode::onClick, this));

	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");

	statusimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("statusicon");
	statusimg->setVisible(false);

	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	//血量进度条
	hp_bar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("hpbar");

	atkspeed_bar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("atkspeedbar");

	atkspeed_barbg = (cocos2d::ui::Widget*)csbnode->getChildByName("heroatkspeedbarbg");

	atkspeed_bar->setVisible(false);
	atkspeed_barbg->setVisible(false);

	retbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("ret");
	rettext = (cocos2d::ui::ImageView*)retbox->getChildByName("text");
	retbox->setVisible(false);

	winexplbl = (cocos2d::ui::Text*)csbnode->getChildByName("winexp");
	winexplbl->setVisible(false);

	return true;
}

void FightHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_Data != NULL)
		{
			FighterAttrLayer* layer = FighterAttrLayer::create(m_Data);
			this->getParent()->addChild(layer, 0, this->getTag());
			if (m_state == FS_READY)
				layer->setContentPos(Vec2(360, 1000));
			else if (m_state == FS_FIGHTING)
			{
				if (m_datatype == F_HERO)
					layer->setContentPos(Vec2(360, 270));
				else if (m_datatype == F_NPC)
					layer->setContentPos(Vec2(360, 900));
			}
		}
	}
}

void FightHeroNode::setData(Npc* data, FIGHTDATA_TYPE datatype, FIGHTNODE_STATE state)
{
	m_Data = data;
	m_datatype = datatype;
	m_state = state;
	if (data != NULL && data->getHp() > 0)
	{
		std::string str;
		
		if (datatype == F_HERO)
		{
			int sex = ((Hero*)m_Data)->getSex();
			str = StringUtils::format("cardh_%d_%d.png", data->getVocation(), sex);
			str = ResourcePath::makeImagePath(str);
			headimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
		}
		else if (datatype == F_NPC)
		{
			str = StringUtils::format("mapui/%s.png", data->getId().c_str());
			headimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		//headimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(true);

		str = StringUtils::format("cardherobox_%d.png", data->getPotential());
		headbox->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

		namelbl->setString(data->getName());
		namelbl->setVisible(true);
		float percent = data->getHp()*100/ data->getMaxHp();
		hp_bar->setPercent(percent);
		if (state == FS_FIGHTING)
		{
			atkspeed_bar->setVisible(true);
			atkspeed_barbg->setVisible(true);
			atkspeed_bar->setPercent(0);
			atkspeed = 1.0f / data->getAtkSpeed();
			this->scheduleUpdate();
		}
	}
	else
	{
		setBlankBox();
	}
}

void FightHeroNode::update(float dt)
{
	if (ispause)
		return;

	timedt += dt;
	if (timedt >= atkspeed)
	{
		timedt = 0.0f;
		FightingLayer* fighting = (FightingLayer*)this->getParent();
		fighting->pauseAtkSchedule();
		this->runAction(Sequence::create(ScaleTo::create(0.2f, 1.5f), ScaleTo::create(0.1f, 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::atkAnimFinish, this)), NULL));
	}
	
	atkspeed_bar->setPercent(timedt * 100 / atkspeed);
}

void FightHeroNode::pauseTimeSchedule()
{
	if (m_Data != NULL)
	{
		ispause = true;
	}
}

void FightHeroNode::resumeTimeSchedule()
{
	if (m_Data != NULL)
		ispause = false;
}


void FightHeroNode::hurt(float hp)
{
	if (m_Data != NULL && this->isVisible())
	{
		hurtup = hp;
		statusimg->loadTexture("mapui/hurticon.png", cocos2d::ui::Widget::TextureResType::PLIST);
		ActionInterval* ac1 = Spawn::create(Show::create(), FadeIn::create(0.15f), EaseSineIn::create(ScaleTo::create(0.15f, 1)), NULL);
		statusimg->runAction(Sequence::create(ac1, CallFunc::create(CC_CALLBACK_0(FightHeroNode::hpAnim, this)), DelayTime::create(0.2f), Hide::create(), NULL));
	}
}

void FightHeroNode::atkAnimFinish()
{
	FightingLayer* fighting = (FightingLayer*)this->getParent();
	fighting->showAtk(m_Data);
}

void FightHeroNode::hpAnim()
{
	float lefthp = m_Data->getHp() - hurtup;
	if (lefthp < 0)
		lefthp = 0;
	m_Data->setHp(lefthp);
	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), CallFunc::create(CC_CALLBACK_0(FightHeroNode::hurtAnimFinish, this)), NULL));
}

void FightHeroNode::hurtAnimFinish()
{
	FightingLayer* fighting = (FightingLayer*)this->getParent();
	updateHp();

	if (m_datatype == F_HERO)
	{
		if (m_Data->getHp() <= 0.000001f)
		{
			this->unscheduleUpdate();
			((Hero*)m_Data)->setState(HS_DEAD);
			((Hero*)m_Data)->setPos(0);
		}
		fighting->updateMapHero(this->getTag());
	}

	fighting->resumeAtkSchedule();
}


void FightHeroNode::updateHp()
{
	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	hp_bar->setPercent(percent);
	if (m_Data->getHp() <= 0.000001f)
	{
		setBlankBox();
	}
}

void FightHeroNode::setBlankBox()
{
	if (m_state == FS_READY)
	{
		headbox->loadTexture(ResourcePath::makeImagePath("cardherobox_.png"), cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(false);
		namelbl->setVisible(false);
		hp_bar->setVisible(false);
		atkspeed_bar->setVisible(false);
		atkspeed_barbg->setVisible(false);
	}
	else if (m_state == FS_FIGHTING)
	{
		this->setVisible(false);
	}
	else if (m_state == FS_SUCC)
	{
		if (m_Data == NULL)
			this->setVisible(false);
	}
}

void FightHeroNode::setWinState(int winexp)
{
	int langtype = GlobalInstance::getInstance()->getLang();
	Hero* myhero = (Hero*)m_Data;
	if (myhero->getState() != HS_DEAD)
	{
		int mylv = myhero->getLevel();
		DynamicValueInt vl;
		vl.setValue(myhero->getExp().getValue() + winexp);
		myhero->setExp(vl);
		hp_bar->loadTexture("mapui/winexpbar.png", cocos2d::ui::Widget::TextureResType::PLIST);

		std::string str = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), winexp);
		winexplbl->setString(str);
		winexplbl->setVisible(true);
		FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
		FiniteTimeAction* moveandout = Spawn::create(MoveBy::create(0.5f, Vec2(0, 20)), FadeOut::create(0.5f), NULL);
		winexplbl->runAction(Sequence::create(scales, moveandout, NULL));

		int maxlv = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp.size();
		int curlv = -1;
		for (int i = 0; i < maxlv; i++)
		{
			if (myhero->getExp().getValue() >= GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[i])
				curlv = i;
			else
				break;
		}

		int moreexp = 0;
		int needexp = 0;

		int nextlv = curlv + 1;

		if (nextlv >= maxlv)
		{
			nextlv = maxlv - 1;
			DynamicValueInt vl;
			vl.setValue(GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[nextlv]);
			myhero->setExp(vl);
		}

		if (curlv < 0)
		{
			moreexp = myhero->getExp().getValue() - 0;
			needexp = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[0];
		}
		else
		{
			moreexp = myhero->getExp().getValue() - GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[curlv];
			needexp = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[nextlv] - GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[nextlv - 1];
		}

		float percent = moreexp * 100 / needexp;
		hp_bar->setPercent(percent);
		if (curlv > mylv)//升级
		{
			retbox->setVisible(true);

			rettext->loadTexture(ResourcePath::makeTextImgPath("winlvup_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

			FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
			FiniteTimeAction* moveandout = Spawn::create(MoveBy::create(0.5f, Vec2(0, 20)), FadeOut::create(0.5f), NULL);
			rettext->runAction(Sequence::create(scales, moveandout, NULL));
		}
	}
	else
	{
		retbox->setVisible(true);
		rettext->loadTexture(ResourcePath::makeTextImgPath("windeath_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		CommonFuncs::changeGray(headbox);
		CommonFuncs::changeGray(headimg);
		CommonFuncs::changeGray(hp_bar);
		CommonFuncs::changeGray(retbox);
	}
}

