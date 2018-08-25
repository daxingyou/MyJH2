﻿#include "FightHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "FighterAttrLayer.h"
#include "FightingLayer.h"
#include "LoadingBarProgressTimer.h"
#include "GlobalInstance.h"
#include "MapBlockScene.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "SoundManager.h"
#include "SkillStartLayer.h"

FightHeroNode::FightHeroNode()
{
	atkspeed = 0.0f;
	timedt = 0.0f;
	ispause = false;
	hurtup = 0.0f;
	atkspeedbns = 0.0f;
	dfbns = 0.0f;
	skillIndex = -1;
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
	if (!Node::init())
		return false;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightHeroNode.csb"));
	this->addChild(csbnode);

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	headbox->addTouchEventListener(CC_CALLBACK_2(FightHeroNode::onClick, this));

	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");

	statusimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("statusicon");
	statusimg->ignoreContentAdaptWithSize(true);
	statusimg->setVisible(false);

	critnumbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("critnumbg");
	critnumbg->setVisible(false);

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

	vocationbox = (cocos2d::ui::Widget*)csbnode->getChildByName("vocationbox");

	vocationicon = (cocos2d::ui::ImageView*)vocationbox->getChildByName("v");

	numfnt = (cocos2d::ui::TextBMFont*)csbnode->getChildByName("numfnt");
	numfnt->setVisible(false);

	skilltextbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("skilltextbox");
	skilltextbox->setVisible(false);

	skilltext = (cocos2d::ui::ImageView*)skilltextbox->getChildByName("skilltext");

	return true;
}

void FightHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		//if (m_Data != NULL)
		//{
		//	FighterAttrLayer* layer = FighterAttrLayer::create(m_Data);
		//	this->getParent()->addChild(layer, 0, this->getTag());
		//	if (m_state == FS_READY)
		//		layer->setContentPos(Vec2(360, 1000));
		//	else if (m_state == FS_FIGHTING)
		//	{
		//		if (m_datatype == F_HERO)
		//			layer->setContentPos(Vec2(360, 270));
		//		else if (m_datatype == F_NPC)
		//			layer->setContentPos(Vec2(360, 900));
		//	}
		//}
	}
}

void FightHeroNode::setData(Npc* data, FIGHTDATA_TYPE datatype, FIGHTNODE_STATE state)
{
	m_Data = data;
	m_datatype = datatype;
	m_state = state;
	if (data != NULL && (data->getHp() > 0 || state == FS_SUCC || state == FS_FAIL))
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
			str = StringUtils::format("mapui/%s.png", GlobalInstance::map_Npcs[data->getId()].icon.c_str());
			headimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		int v = m_Data->getVocation();
		if (v >= 4)
		{
			str = StringUtils::format("ui/cardvocation%d.png", v);
			vocationicon->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);
			vocationbox->setVisible(true);
		}
		else
			vocationbox->setVisible(false);
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


Npc* FightHeroNode::getData()
{
	return m_Data;
}

void FightHeroNode::update(float dt)
{
	if (ispause)
		return;

	timedt += dt;
	if (timedt >= getAtkSpeed() && this->isVisible())
	{
		timedt = 0.0f;
		FightingLayer* fighting = (FightingLayer*)this->getParent();
		fighting->pauseAtkSchedule();
		if (skillIndex != 3)
		{
			showAtkOrHurtAnim(0);
			this->runAction(Sequence::create(ScaleTo::create(0.2f, 1.5f), ScaleTo::create(0.1f, 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::atkAnimFinish, this)), NULL));
		}
		else
			this->runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::atkAnimFinish, this)), NULL));
	}
	
	atkspeed_bar->setPercent(timedt * 100 / getAtkSpeed());
}

float FightHeroNode::getAtkSpeed()
{
	float s = 1.0f / atkspeed;
	s = (1 + atkspeedbns / 100)*s;
	return atkspeed = 1.0f / s;
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


void FightHeroNode::hurt(float hp, int stat)
{
	if (m_Data != NULL && this->isVisible())
	{
		if (hp < m_Data->getDf())
			hp = 0.1 * hp;
		else
			hp -= m_Data->getDf()*(1 + dfbns/100);

		if (hp > 0 || stat == 2)
		{
			hurtup = hp;

			std::string hurtstr = StringUtils::format("-%d", (int)hurtup);
			if (stat == 1)
			{
				numfnt->setFntFile("fonts/crithurtnum.fnt");
				int langtype = GlobalInstance::getInstance()->getLang();
				statusimg->loadTexture(ResourcePath::makeTextImgPath("crit_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
				numfnt->setScale(5);
				numfnt->runAction(Sequence::create(Show::create(), EaseRateAction::create(ScaleTo::create(0.15f, 0.6f), 5), EaseRateAction::create(ScaleTo::create(0.1f, 1),5), DelayTime::create(0.15f), Hide::create(), NULL));
				critnumbg->runAction(Sequence::create(Show::create(), EaseRateAction::create(ScaleTo::create(0.15f, 0.6f), 5), EaseRateAction::create(ScaleTo::create(0.1f, 1),5), DelayTime::create(0.15f), Hide::create(), NULL));
			}
			else if (stat == 2)
			{
				hurtup = 0;
				int langtype = GlobalInstance::getInstance()->getLang();
				statusimg->loadTexture(ResourcePath::makeTextImgPath("dodge_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
			else
			{
				numfnt->setFntFile("fonts/normalhurtnum.fnt");
				statusimg->loadTexture("ui/blank.png", cocos2d::ui::Widget::TextureResType::PLIST);
				if (skillIndex < 0)
					showAtkOrHurtAnim(1);
				numfnt->runAction(Sequence::create(Show::create(), MoveBy::create(0.3f, Vec2(0, 10)), DelayTime::create(0.1f), Hide::create(), MoveBy::create(0.02f, Vec2(0, -10)), NULL));
			}
			numfnt->setString(hurtstr);

			ActionInterval* ac1 = Spawn::create(Show::create(), FadeIn::create(0.15f), EaseSineIn::create(ScaleTo::create(0.15f, 1)), NULL);
			statusimg->runAction(Sequence::create(ac1, CallFunc::create(CC_CALLBACK_0(FightHeroNode::hpAnim, this)), DelayTime::create(0.2f), Hide::create(), NULL));
		
		}
		else
		{
			hurtAnimFinish();
		}
	}
} 

void FightHeroNode::atkAnimFinish()
{
	if (this->isVisible())
	{
		FightingLayer* fighting = (FightingLayer*)this->getParent();
		fighting->showAtk(this->getTag());
	}
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
		if (m_Data->getHp() <= 0)
		{
			removePlaySkillAnim(0);
			fighting->clearSkill(this->getTag());

			if (checkReviveSkill())
			{
				playSkill(SKILL_13, NULL);
				attackedSkill(SKILL_13, this->getTag());

				return;
			}
			else
			{
				this->unscheduleUpdate();
				((Hero*)m_Data)->setState(HS_DEAD);

				((Hero*)m_Data)->setPos(0);

				int v = ((Hero*)m_Data)->getVocation();

				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DIE);
			}
		}

		fighting->updateMapHero(this->getTag());
	}
	else
	{
		if (m_Data->getHp() <= 0)
		{
			for (int i = 0; i < 6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getWhoSufferskill() == (this->getTag() - 6))
				{
					fighting->clearSkill(i);
					break;
				}
			}
		}

	}
	skillIndex = -1;
	fighting->resumeAtkSchedule();

}

bool FightHeroNode::checkReviveSkill()
{
	GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_WG, m_Data->getName());

	gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, m_Data->getName());
	if (gf != NULL)
	{
		if (GlobalInstance::map_GF[gf->getId()].skill == SKILL_13)
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(1);
				return true;
			}
		}
		else
		{
			gf->setSkillCount(0);
		}
	}
	return false;
}

void FightHeroNode::reviveOnce(float hp)
{
	m_Data->setHp(hp);
	this->runAction(Sequence::create(Show::create(), FadeIn::create(0.3f), NULL));
	updateHp();

	FightingLayer* fighting = (FightingLayer*)this->getParent();
	fighting->updateMapHero(this->getTag());
	fighting->resumeAtkSchedule();
}

void FightHeroNode::updateHp()
{
	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	//hp_bar->setPercent(percent);
	hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));
	if (m_Data->getHp() <= 0)
	{
		setBlankBox();
	}
}

void FightHeroNode::setBlankBox()
{
	vocationbox->setVisible(false);
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
		showDeathAnim();
	}
	else if (m_state == FS_SUCC || m_state == FS_FAIL)
	{
		if (m_Data == NULL)
			this->setVisible(false);
	}
}

void FightHeroNode::setFightState(int winexp)
{
	int langtype = GlobalInstance::getInstance()->getLang();
	Hero* myhero = (Hero*)m_Data;

	int mylv = myhero->getLevel();
	hp_bar->loadTexture("mapui/winexpbar.png", cocos2d::ui::Widget::TextureResType::PLIST);

	if (winexp > 0 && myhero->getState() != HS_DEAD)
	{
		std::string str;
		if ((myhero->getLevel() + 1) / 10 == myhero->getChangeCount() + 1)
		{
			//MovingLabel::show(ResourceLang::map_lang["changebreak"]);
			str = ResourceLang::map_lang["changebreak"];
		}
		else
		{
			DynamicValueInt dv;
			dv.setValue(myhero->getExp().getValue() + winexp);
			myhero->setExp(dv);

			str = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), winexp);
		}
		winexplbl->setString(str);
		winexplbl->setVisible(true);
		FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
		FiniteTimeAction* moveandout = Spawn::create(MoveBy::create(1.5f, Vec2(0, 10)), NULL);
		winexplbl->runAction(Sequence::create(scales, moveandout, NULL));
	}

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

	if (myhero->getState() != HS_DEAD)
	{
		if (curlv+1 > mylv)//升级
		{
			retbox->setVisible(true);

			rettext->loadTexture(ResourcePath::makeTextImgPath("winlvup_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

			FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
			FiniteTimeAction* moveandout = Spawn::create(EaseSineOut::create(MoveBy::create(1.2f, Vec2(0, 40))), NULL);
			retbox->runAction(Sequence::create(scales, moveandout, NULL));
			myhero->setHp(myhero->getMaxHp());
			if (g_MapBlockScene != NULL)
				g_MapBlockScene->updateHeroUI(this->getTag());

		}
		else
		{
			GlobalInstance::getInstance()->saveHero(myhero);
		}
	}
	else
	{
		rettext->loadTexture(ResourcePath::makeTextImgPath("windeath_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		retbox->setVisible(true);
		retbox->setPositionY(retbox->getPositionY() + 40);
		retbox->setScale(3);

		retbox->runAction(Speed::create(ScaleTo::create(1.2f, 1.0f), 5));

		CommonFuncs::changeGray(headbox);
		CommonFuncs::changeGray(headimg);
		CommonFuncs::changeGray(hp_bar->getVirtualRenderer());
		CommonFuncs::changeGray(retbox);
	}
}

void FightHeroNode::playSkill(int stype, Npc* data)
{
	bool isPlayAnim = false;
	int gftype = T_WG;

	if (stype > SKILL_8)
		gftype = T_NG;

	GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(gftype, m_Data->getName());

	skillIndex = stype;
	if (m_Data->getId().length() <= 0)//是否是自己的英雄
	{
		if (stype == SKILL_1)//释放技能后吸收对方%.2f血量。
		{
			float eff = GlobalInstance::map_GF[gf->getId()].skilleff1;
			m_Data->setHp(m_Data->getHp() + eff*data->getMaxHp() / 100);
			if (m_Data->getHp() + eff*data->getHp() / 100 > m_Data->getMaxHp())
				m_Data->setHp(m_Data->getMaxHp());

			this->setLocalZOrder(0);
			float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
			hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));
			isPlayAnim = true;
		}
		else if (stype == SKILL_2)//释放技能后造成%d倍伤害。
		{
			isPlayAnim = true;
		}

		else if (stype == SKILL_3)//被攻击目标%d回合内无法进行攻击。
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_4)//释放技能后所有敌人攻击你%d回合。
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_9)//增加自身攻击速度%.2f。持续%d回合
		{
			if (gf->getSkillCount() <= 0)
			{
				atkspeedbns = GlobalInstance::map_GF[gf->getId()].skilleff1;
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_10)//降低对方攻击速度%.2f。持续%d回合
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_11)// 降低对方防御%.2f，持续%d回合。
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_12)//触发时提升自身攻击%.2f，持续%d回合。
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_13)//死亡后复活一次。复活后生命值为%.2f。
		{
			isPlayAnim = true;
		}
		else if (stype == SKILL_14)//触发时减少%.2f伤害，持续%d回合。
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_15)
		{
			isPlayAnim = true;
		}
		else if (stype == SKILL_16)
		{
			dfbns = GlobalInstance::map_GF[gf->getId()].skilleff1;
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_17)
		{
			dfbns = -GlobalInstance::map_GF[gf->getId()].skilleff1;
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_18)//
		{
			if (gf->getSkillCount() <= 0)
			{
				gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
				isPlayAnim = true;
			}
		}
		else if (stype == SKILL_19)//
		{
			isPlayAnim = true;
		}
		else if (stype == SKILL_20)//
		{
			isPlayAnim = true;
		}
		else
		{
			isPlayAnim = true;
		}
		float delay = 0.0f;
		if (stype == 1)
			delay = 1.0f;

		if (stype != 7 && stype != 8)
		{
			if (isPlayAnim)
			{
				FightingLayer* fighting = (FightingLayer*)this->getParent();
				fighting->pauseAtkSchedule();
				headbox->scheduleOnce(schedule_selector(FightHeroNode::removeAtkOrHurtAnim), 0.05f);
				if (stype != 5)
					headbox->runAction(Sequence::create(DelayTime::create(delay + 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::playSkillEffect, this, stype)), NULL));

				if (stype >= 9)
				{
					std::string textstr = StringUtils::format("bufpreskill%dtext", stype);
					skilltext->loadTexture(ResourcePath::makeTextImgPath(textstr, GlobalInstance::getInstance()->getLang()), cocos2d::ui::Widget::TextureResType::PLIST);
					skilltextbox->setScale(0);
					skilltextbox->setVisible(true);
					skilltextbox->runAction(Sequence::create(Spawn::create(EaseRateAction::create(ScaleTo::create(0.2f, 1), 10), FadeIn::create(0.2f), NULL), DelayTime::create(0.4f), Hide::create(), NULL));
				}
				else
				{
					if (this->getParent()->getChildByName("skillstart") == NULL)
					{
						SkillStartLayer* layer = SkillStartLayer::create(m_Data->getVocation(), stype);
						this->getParent()->addChild(layer, 10, "skillstart");
					}
				}
				//暂停其它英雄的动作
				for (int i = 0; i < 6; i++)
				{
					if (GlobalInstance::myCardHeros[i] != NULL && i != this->getTag())
					{
						FightHeroNode* fnode = (FightHeroNode*)this->getParent()->getChildByTag(i);
						fnode->pauseAction();
					}
				}
			}
		}
	}
}

void FightHeroNode::attackedSkill(int stype, int myHeroPos)
{
	float dt1 = 1.0f;
	float dt2 = 1.0f;
	if (stype == 1)
	{
		dt1 = 0;
		dt2 = 0.8f;
	}
	else if (stype == 2)
	{
		dt1 = 0.3f;
		dt2 = 1.0f;
	}
	else if (stype == 3)
	{
		dt1 = 0.6f;
		dt2 = 1.0f;
	}
	else if (stype == 4)
	{
		dt1 = 1.2f;
		dt2 = 1.7f;
	}
	else if (stype == 5)
	{
		dt1 = 0.2f;
		dt2 = 1.0f;
	}
	else if (stype == 6)
	{
		dt1 = 0.8f;
		dt2 = 1.2f;
	}
	else if (stype == 7 || stype == 8)
	{
		dt1 = 0.2f;
		dt2 = 1.5f;
	}
	else if (stype == 9)
	{
		dt1 = 1.0f;
		dt2 = 1.3f;
	}
	else if (stype == 10)
	{
		dt1 = 1.0f;
		dt2 = 1.5f;
	}
	else if (stype == 12)
	{
		dt1 = 1.0f;
		dt2 = 1.2f;
	}
	else if (stype == 13)
	{
		dt1 = 1.0f;
		dt2 = 1.75f;
	}
	else if (stype == 15)
	{
		dt1 = 0.8f;
		dt2 = 1.2f;
	}
	else if (stype == 17)
	{
		dt1 = 0.0f;
		dt2 = 0.5f;
	}
	else if (stype == 18)
	{
		dt1 = 0.0f;
		dt2 = 1.5f;
	}
	else if (stype == 20)
	{
		dt1 = 0.3f;
		dt2 = 1.5f;
	}

	skillIndex = stype;
	if (stype != 4 && stype != 9 && stype != 12 && stype != 13 && stype != 17 && stype != 18)
	{
		int gftype = T_WG;

		if (stype > SKILL_8)
			gftype = T_NG;
		GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(gftype, GlobalInstance::myCardHeros[myHeroPos]->getName());

		if (gf == NULL)
			return;
		int roundcount = (int)GlobalInstance::map_GF[gf->getId()].skilleff2;
		if ((roundcount > 0 && gf->getSkillCount() == roundcount) || roundcount == 0 || stype == 5 || stype == 6 || stype == 8)
		{
			headimg->runAction(Sequence::create(DelayTime::create(dt1 + 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::attackedSkillEffect, this, stype, myHeroPos)), NULL));
			FightingLayer* fighting = (FightingLayer*)this->getParent();
			fighting->pauseAtkSchedule();
		}
		else
		{
			if (stype != 3)
				showAtkOrHurtAnim(1);
			else
				hurt(GlobalInstance::myCardHeros[myHeroPos]->getAtk());
			dt2 = 0;
		}

		if (stype == 7 || stype == 8)
		{
			SkillStartLayer* layer = SkillStartLayer::create(m_Data->getVocation(), stype);
			this->getParent()->addChild(layer, 10);
		}
	}

	namelbl->runAction(Sequence::create(DelayTime::create(dt2 + 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::attackedSkillCB, this, stype, myHeroPos)), NULL));
}

void FightHeroNode::attackedSkillCB(int stype, int myHeroPos)
{
	FightingLayer* fighting = (FightingLayer*)this->getParent();

	int gftype = T_WG;

	if (stype > SKILL_8)
		gftype = T_NG;

	Hero *data = GlobalInstance::myCardHeros[myHeroPos];
	GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(gftype, data->getName());

	if (stype == SKILL_1)//释放技能后吸收对方%.2f血量。
	{
		float eff = GlobalInstance::map_GF[gf->getId()].skilleff1;
		hurt(eff*m_Data->getMaxHp() / 100);
		fighting->clearSkill(myHeroPos);
	}
	else if (stype == SKILL_2)//释放技能后造成%d倍伤害。
	{
		float eff = GlobalInstance::map_GF[gf->getId()].skilleff1;
		hurt(eff*data->getAtk());
		fighting->clearSkill(myHeroPos);
	}
	else if (stype == SKILL_3)//被攻击目标%d回合内无法进行攻击。
	{
		nextRound();
	}
	else if (stype == SKILL_4)//释放技能后所有敌人攻击你%d回合。
	{
		nextRound();
	}
	else if (stype == SKILL_5 || stype == SKILL_6)//同时攻击%d个目标。
	{
		fighting->skillAction(stype, myHeroPos);
		fighting->clearSkill(myHeroPos);
	}
	else if (stype == SKILL_7 || stype == SKILL_8)//恢复单个队友血量%.2f。恢复%d个队友血量%.2f。
	{
		fighting->skillAction(stype, myHeroPos);
		fighting->clearSkill(myHeroPos);
		nextRound();
	}
	else if (stype == SKILL_9 || stype == SKILL_10)//增加自身攻击速度%.2f。
	{
		nextRound();
	}
	else if (stype == SKILL_11)
	{
		nextRound();
	}
	else if (stype == SKILL_12)
	{
		if (GlobalInstance::map_GF[gf->getId()].skilleff2 > 1 && gf->getSkillCount() < GlobalInstance::map_GF[gf->getId()].skilleff2 - 1)
		{
			hurt((1 + GlobalInstance::map_GF[gf->getId()].skilleff1)*data->getAtk());
		}
		else
		{
			nextRound();
		}
	}
	else if (stype == SKILL_13)//死亡后复活
	{
		reviveOnce(m_Data->getMaxHp()*GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);
		fighting->clearSkill(myHeroPos);
	}
	else if (stype == SKILL_14)
	{
		nextRound();
	}
	else if (stype == SKILL_15)
	{
		hurt(m_Data->getAtk()*GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);
		fighting->clearSkill(myHeroPos);

	}
	else if (stype == SKILL_16)
	{
		nextRound();
	}
	else if (stype == SKILL_17)
	{
		if (GlobalInstance::map_GF[gf->getId()].skilleff2 > 1 && gf->getSkillCount() < GlobalInstance::map_GF[gf->getId()].skilleff2 - 1)
		{
			hurt((1 + GlobalInstance::map_GF[gf->getId()].skilleff1)*data->getAtk());
		}
		else
		{
			nextRound();
		}
	}
	else if (stype == SKILL_18)
	{
		nextRound();
	}
	else if (stype == SKILL_19)
	{
		fighting->skillAction(stype, myHeroPos);
		fighting->clearSkill(myHeroPos);
	}
	else if (stype == SKILL_20)
	{
		fighting->skillAction(stype, myHeroPos);
		fighting->clearSkill(myHeroPos);
	}
}

void FightHeroNode::recoveHp(float hp)
{
	float eff = hp;
	m_Data->setHp(m_Data->getHp() + eff);
	if (m_Data->getHp() + eff > m_Data->getMaxHp())
		m_Data->setHp(m_Data->getMaxHp());

	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), CallFunc::create(CC_CALLBACK_0(FightHeroNode::nextRound, this)),  NULL));

	std::string hpstr = StringUtils::format("+%d", (int)eff);
	numfnt->setString(hpstr);
	numfnt->setFntFile("fonts/addhpnum.fnt");
	numfnt->runAction(Sequence::create(Show::create(), DelayTime::create(0.35f), Hide::create(), NULL));
}

void FightHeroNode::nextRound()
{
	skillIndex = -1;
	FightingLayer* fighting = (FightingLayer*)this->getParent();
	fighting->resumeAtkSchedule();
}

void FightHeroNode::playSkillEffect(int stype)
{
	std::string effectname = StringUtils::format("effect/skill_%d_0.csb", stype);
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(effectnode, 10, "playskillani");

	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);

	this->scheduleOnce(schedule_selector(FightHeroNode::removePlaySkillAnim), action->getDuration() / 60.0f);
}

void FightHeroNode::playMoreSkillEffect(int stype, int enemyindex)
{
	headbox->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::playMoreSkillEffectCB, this, stype, enemyindex)), NULL));
}

void FightHeroNode::playMoreSkillEffectCB(int stype, int enemyindex)
{
	std::string effectname = StringUtils::format("effect/skill_%d_0.csb", stype);
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(effectnode, 10, "playskillani");


	float offsety = 450;
	float angle = 0;
	Vec2 mypos = this->getPosition();
	Vec2 enemypos = this->getParent()->getChildByTag(enemyindex + 6)->getPosition();
	float tanx = enemypos.x - mypos.x;
	float tany = enemypos.y - mypos.y;
	float tanz = sqrt(tanx*tanx + tany*tany);
	float tan_yx = std::fabs(tany) / std::fabs(tanx);
	angle = 90 - atan(tan_yx) * 180 / M_PI;

	float movex = 0;
	if (tanx > 0)
	{
		effectnode->setRotation(angle);
		movex = tanx - offsety * sin(angle * M_PI / 180);
	}
	else
	{
		effectnode->setRotation(-angle);
		movex = -tanx + offsety * sin(angle * M_PI / 180);
	}
	float movey = tany - offsety * cos(angle * M_PI / 180);


	if (tanz > 420)
		effectnode->runAction(MoveTo::create(1.0f, Vec2(movex, movey)));

	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);

	this->scheduleOnce(schedule_selector(FightHeroNode::removePlaySkillAnim), action->getDuration() / 60.0f);
}

void FightHeroNode::attackedSkillEffect(int stype, int myHeroPos)
{
	std::string effectname = StringUtils::format("effect/skill_%d_1.csb", stype);
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(effectnode, 10, "sufferskillani");

	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, stype==3);

	if (stype != 3)
		this->scheduleOnce(schedule_selector(FightHeroNode::removeSufferSkillAnim), action->getDuration()/60.0f);
	

	if (stype == 1)
	{
		float offsety = 450;
		float angle = 0;
		Vec2 pos = this->getPosition();
		Vec2 mypos = this->getParent()->getChildByTag(myHeroPos)->getPosition();
		float tanx = pos.x - mypos.x;
		float tany = pos.y - mypos.y;
		float tanz = sqrt(tanx*tanx + tany*tany);
		float tan_yx = std::fabs(tany) / std::fabs(tanx);
		angle = 90 - atan(tan_yx) * 180 / M_PI;
		
		float movex = 0;
		if (tanx > 0)
		{
			effectnode->setRotation(angle);
			movex = -tanx + offsety * sin(angle * M_PI / 180);
		}
		else
		{
			effectnode->setRotation(-angle);
			movex = -tanx - offsety * sin(angle * M_PI / 180);
		}
		float movey = -tany + offsety * cos(angle * M_PI / 180);


		if (tanz > 420)
			effectnode->runAction(MoveTo::create(1.0f, Vec2(movex, movey)));
		this->scheduleOnce(schedule_selector(FightHeroNode::resetZorder), action->getDuration() / 60.0f);
	}
}

void FightHeroNode::resetZorder(float dt)
{
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			FightHeroNode* myheronode = (FightHeroNode*)this->getParent()->getChildByTag(i);
			myheronode->setLocalZOrder(2);
		}
	}
}

void FightHeroNode::showAtkOrHurtAnim(int type)
{
	std::string effectname;
	if (type == 0)
	{
		effectname ="effect/atkanim.csb";
	}
	else
	{
		effectname = "effect/hurtanim.csb";
	}
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(effectnode, 10, "atkhurt");
	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	this->scheduleOnce(schedule_selector(FightHeroNode::removeAtkOrHurtAnim), 0.33f);
}

void FightHeroNode::showDeathAnim()
{
	std::string effectname = "effect/carddeath.csb";
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(effectnode, 10, "death");
	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	this->runAction(FadeOut::create(0.6f));
	this->scheduleOnce(schedule_selector(FightHeroNode::HideMe), action->getDuration() / 60.0f - 0.5f);
	this->scheduleOnce(schedule_selector(FightHeroNode::removeDeathAnim), action->getDuration()/60.0f);
}

void FightHeroNode::removeDeathAnim(float dt)
{
	this->removeChildByName("death");
}

void FightHeroNode::HideMe(float dt)
{
	this->setVisible(false);
}

void FightHeroNode::removeAtkOrHurtAnim(float dt)
{
	this->removeChildByName("atkhurt");
}

void FightHeroNode::removePlaySkillAnim(float dt)
{
	this->removeChildByName("playskillani");
}

void FightHeroNode::removeSufferSkillAnim(float dt)
{
	this->removeChildByName("sufferskillani");
}

void FightHeroNode::pauseAction()
{
	headbox->pause();
	namelbl->pause();
}

void FightHeroNode::resumeAction()
{
	headbox->resume();
	namelbl->resume();
}